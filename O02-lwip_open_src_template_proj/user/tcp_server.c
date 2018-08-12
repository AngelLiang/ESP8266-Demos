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
 * description: TCP Server send callbcak successful
 * TCP Server发送成功回调
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
	struct espconn *pesp_conn = arg;
	// TODO:

    os_printf(IPSTR":%d disconnect!\r\n",
      IP2STR(pesp_conn->proto.tcp->remote_ip), pesp_conn->proto.tcp->remote_port);
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
    os_printf(IPSTR":%d say:\r\n%s\r\n",
      IP2STR(pesp_conn->proto.tcp->remote_ip), pesp_conn->proto.tcp->remote_port,
	  pdata);
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

    os_printf(IPSTR":%d connected!\r\n",
      IP2STR(pesp_conn->proto.tcp->remote_ip), pesp_conn->proto.tcp->remote_port);
}

static void ICACHE_FLASH_ATTR
espconn_init(struct espconn *p_espconn, esp_tcp *p_esptcp, uint32 port)
{
	p_espconn->type = ESPCONN_TCP;
	p_espconn->state = ESPCONN_NONE;
	p_espconn->proto.tcp = p_esptcp;
	p_espconn->proto.tcp->local_port = port;
	espconn_regist_connectcb(p_espconn, tcp_server_listen);

    espconn_accept(p_espconn);
	espconn_regist_time(p_espconn, 60, 0);	// TCP server link time out
}

/*
 * function: tcp_server_init
 */
void ICACHE_FLASH_ATTR
tcp_server_init(uint32 port)
{
	static struct espconn s_tcp_server;
	static esp_tcp s_esp_tcp;

    espconn_init(&s_tcp_server, &s_esp_tcp, port);

    os_printf("tcp_server_init\r\n");
}
