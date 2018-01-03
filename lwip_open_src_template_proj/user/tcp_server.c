/*
 * tcp_server.c
 *
 *  Created on: 2017骞�7鏈�3鏃�
 *      Author: Administrator
 */

#include "tcp_server.h"

#define TCP_BUFF_SIZE		128
static u8 g_tcp_buff[TCP_BUFF_SIZE];



/*
 * function: tcp_server_sent_cb
 * description: TCP Server发送成功回调
 */
static void ICACHE_FLASH_ATTR
tcp_server_sent_cb(void *arg)
{
	// TODO:
}

/*
 * function: tcp_server_discon_cb
 */
static void ICACHE_FLASH_ATTR
tcp_server_discon_cb(void *arg)
{
	// TODO:
}

/*
 * function: tcp_server_recv
 */
static void ICACHE_FLASH_ATTR
tcp_server_recv(void *arg, char *pdata, unsigned short len)
{
	struct espconn *pesp_conn = arg;
	u16 send_data_len = 0;

	// TODO:
	espconn_send(pesp_conn, pdata, len);
}

/*
 * function: tcp_server_listen
 */
static void ICACHE_FLASH_ATTR
tcp_server_listen(void *arg)
{
    struct espconn *pesp_conn = arg;

    espconn_regist_disconcb(pesp_conn, tcp_server_discon_cb);
    espconn_regist_recvcb(pesp_conn, tcp_server_recv);
    espconn_regist_sentcb(pesp_conn, tcp_server_sent_cb);
    //espconn_regist_reconcb(pesp_conn, client_recon_cb);

}

/*
 * function: tcp_server_init
 */
void ICACHE_FLASH_ATTR
tcp_server_init(uint32 port)
{
    static struct espconn s_tcp_server;
    static esp_tcp s_esptcp;

	s_tcp_server.type = ESPCONN_TCP;
	s_tcp_server.state = ESPCONN_NONE;
	s_tcp_server.proto.tcp = &s_esptcp;
	s_tcp_server.proto.tcp->local_port = port;
	espconn_regist_connectcb(&s_tcp_server, tcp_server_listen);

    espconn_accept(&s_tcp_server);
	espconn_regist_time(&s_tcp_server, 60, 0);	// TCP server link time out
	
    os_printf("tcp_server_init\r\n");
}
