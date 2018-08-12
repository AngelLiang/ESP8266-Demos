/*
 * udp_server.c
 *
 *  Created on: 2017��7��3��
 *      Author: Administrator
 */

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "mem.h"
#include "espconn.h"

#include "udp_server.h"

#define UDP_BUFF_SIZE		2000
static u8 g_udp_buff[UDP_BUFF_SIZE];

u32 udp_recv_byte_count = 0;

static u8 ICACHE_FLASH_ATTR
udp_get_peer_conn(void *arg) {
	struct espconn *pesp_conn = arg;
	remot_info *premot = NULL;

	if (espconn_get_connection_info(pesp_conn, &premot, 0) == ESPCONN_OK) {
		pesp_conn->proto.udp->remote_port = premot->remote_port;
		os_memcpy(pesp_conn->proto.udp->remote_ip, premot->remote_ip, 4);
		return 1;
	}
	return 0;
}

static void ICACHE_FLASH_ATTR
udp_print_conn(void *arg) {
	struct espconn *pesp_conn = arg;

	os_printf("[INFO] udp recv:"IPSTR":%d\r\n",
			IP2STR(pesp_conn->proto.udp->remote_ip),
			pesp_conn->proto.udp->remote_port);
}

static void ICACHE_FLASH_ATTR
udp_server_sent_cb(void *arg) {
	// TODO:
}

static void ICACHE_FLASH_ATTR
udp_server_recv(void *arg, char *pdata, unsigned short len) {
	struct espconn *pesp_conn = arg;
	u16 send_data_len = 0;

//	udp_get_peer_conn(arg);
//	udp_print_conn(arg);

// TODO:

	udp_recv_byte_count += len;
//	espconn_send(pesp_conn, pdata, len);
}

void ICACHE_FLASH_ATTR
udp_server_init(uint32 port) {
	static struct espconn s_udp_server;
	static esp_udp s_espudp;

	s_udp_server.type = ESPCONN_UDP;
	s_udp_server.state = ESPCONN_NONE;
	s_udp_server.proto.udp = &s_espudp;
	s_udp_server.proto.udp->local_port = port;

	espconn_regist_recvcb(&s_udp_server, udp_server_recv);
	espconn_regist_sentcb(&s_udp_server, udp_server_sent_cb);
	espconn_create(&s_udp_server);

	os_printf("udp_server_init\r\n");
}
