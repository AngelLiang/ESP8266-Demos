/*
 * udp_server.c
 *
 *  Created on: 2017年7月3日
 *      Author: Administrator
 */

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "mem.h"
#include "espconn.h"

/************* UDP Server ********/
#include "udp_server.h"
#define UDP_BUFF_SIZE		128
static u8 g_udp_buff[UDP_BUFF_SIZE];


/************* CoAP **************/
#include "coap.h"
coap_packet_t pkt;

u8 buf[4096];
u8 scratch_raw[4096];
coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)};


/*
 * function: udp_get_peer_conn
 * decription: get peer connect info
 */
static u8 ICACHE_FLASH_ATTR
udp_get_peer_conn(void *arg)
{
	struct espconn *pesp_conn = arg;
	remot_info *premot = NULL;

	if (espconn_get_connection_info(pesp_conn, &premot, 0) == ESPCONN_OK){
		pesp_conn->proto.udp->remote_port = premot->remote_port;
		os_memcpy(pesp_conn->proto.udp->remote_ip, premot->remote_ip, 4);
		return 1;
	}
	return 0;
}

/*
 * function: udp_print_conn
 * decription: 打印UDP对面连接的信息
 */
static void ICACHE_FLASH_ATTR
udp_print_conn(void *arg)
{
  struct espconn *pesp_conn = arg;

  os_printf("[INFO] udp recv:"IPSTR":%d\r\n",
    IP2STR(pesp_conn->proto.udp->remote_ip), pesp_conn->proto.udp->remote_port);
}

/*
 * function: udp_server_sent_cb
 * decription: UDP sent callback
 */
static void ICACHE_FLASH_ATTR
udp_server_sent_cb(void *arg)
{
	// TODO:
}

/*
 * function: udp_server_recv
 * decription: UDP Server receive callback
 */
static void ICACHE_FLASH_ATTR
udp_server_recv(void *arg, char *pdata, unsigned short len)
{
	struct espconn *pesp_conn = arg;
	u16 send_data_len = 0;

	udp_get_peer_conn(arg);
	udp_print_conn(arg);

	os_memcpy(buf, pdata, len);

	// TODO:
	int rc;
    if (0 != (rc = coap_parse(&pkt, buf, len))){
        os_printf("Bad packet rc=%d\n", rc);
    }else{
        size_t rsplen = sizeof(buf);
        coap_packet_t rsppkt;
#ifdef DEBUG
        coap_dumpPacket(&pkt);
#endif
        coap_handle_req(&scratch_buf, &pkt, &rsppkt);

        if (0 != (rc = coap_build(buf, &rsplen, &rsppkt))){
        	os_printf("coap_build failed rc=%d\n", rc);
        }else{
#ifdef DEBUG
        	os_printf("Sending: ");
            coap_dump(buf, rsplen, true);
            os_printf("\n");
#endif
#ifdef DEBUG
            coap_dumpPacket(&rsppkt);
#endif

            //sendto(fd, buf, rsplen, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
            espconn_send(pesp_conn, buf, rsplen);
        }
    }


//	espconn_send(pesp_conn, g_udp_buff, send_data_len);
}


/*
 * function： udp_server_init
 * decription: UDP Server init
 */
void ICACHE_FLASH_ATTR
udp_server_init(uint32 port)
{
	static struct espconn s_udp_server;
	static esp_udp s_espudp;

	s_udp_server.type = ESPCONN_UDP;
	s_udp_server.state = ESPCONN_NONE;
	s_udp_server.proto.udp = &s_espudp;
	s_udp_server.proto.udp->local_port = port;

	espconn_regist_recvcb(&s_udp_server, udp_server_recv);		//注册UDP接收回调函数
	espconn_regist_sentcb(&s_udp_server, udp_server_sent_cb); 	//注册UDP发送回调函数
	espconn_create(&s_udp_server);

	os_printf("udp_server_init\r\n");
}
