/*
 * tcp_server.c
 *
 *  Created on: 2017年7月3日
 *      Author: Administrator
 */


#include "ets_sys.h"
#include "ip_addr.h"
#include "espconn.h"
#include "osapi.h"
#include "user_interface.h"
#include "mem.h"

#include "driver/uart.h"

#include "tcp_server.h"


#define TCP_BUFF_SIZE		128
static u8 tcp_buff[TCP_BUFF_SIZE];

static struct espconn tcp_server;
static esp_tcp esptcp;

/*
 * 函数：tcp_server_sent_cb
 * 参数：void *arg - 连接结构体
 * 返回：void
 * 说明：TCP Server发送回调
 */
static void ICACHE_FLASH_ATTR
tcp_server_sent_cb(void *arg)
{
	os_printf("TCP Client Send Call Back\n");
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
	os_printf("TCP Client discon\n");
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

	static char temp[64];

	os_printf("lenth:%d\r\ndata:%s\r\n",len, pdata);

  // TODO:

  espconn_send(pesp_conn, tcp_buff, os_strlen(tcp_buff));
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
	tcp_server.type = ESPCONN_TCP;
	tcp_server.state = ESPCONN_NONE;
	tcp_server.proto.tcp = &esptcp;
	tcp_server.proto.tcp->local_port = port;
    espconn_regist_connectcb(&tcp_server, tcp_server_listen);

    espconn_accept(&tcp_server);

    os_printf("tcp_server_init\r\n");
}
