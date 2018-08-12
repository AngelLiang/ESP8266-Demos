/*
 * tcp_server.c
 *
 *  Created on: 2017年7月3日
 *      Author: Administrator
 */

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "mem.h"
#include "espconn.h"

#include "tcp_server.h"

#define TCP_BUFF_SIZE		128
static u8 g_tcp_buff[TCP_BUFF_SIZE];



/*
 * 函数：tcp_server_sent_cb
 * 参数：void *arg - 连接结构体
 * 返回：void
 * 说明：TCP Server发送回调
 */
static void ICACHE_FLASH_ATTR
tcp_server_sent_cb(void *arg)
{
	// TODO:
}

/*
 * 函数：tcp_server_discon_cb
 * 参数：void *arg - 连接结构体
 * 返回：void
 * 说明：TCP Server断开回调
 */
static void ICACHE_FLASH_ATTR
tcp_server_discon_cb(void *arg)
{
	// TODO:
}

/*
 * 函数：tcp_server_recv
 * 参数：void *arg - 连接结构体
 * 	   char *pdata - 接收数据首地址
 * 	   unsigned short len - 接收数据长度
 * 返回：void
 * 说明：TCP Server接收回调
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
 * 函数：tcp_server_listen
 * 参数：void *arg - 连接结构体
 * 返回：void
 * 说明：TCP Server监听
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
 * 函数：tcp_server_init
 * 参数：void *arg - 连接结构体
 * 返回：void
 * 说明：TCP Server初始化
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
	espconn_regist_time(&s_tcp_server, 60, 0);			// TCP server 超时时间
	
    os_printf("tcp_server_init\r\n");
}
