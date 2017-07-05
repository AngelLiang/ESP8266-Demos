/*
 * tcp_client.c
 *
 *  Created on: 2017年7月3日
 *      Author: Administrator
 */


#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "espconn.h"

#include "tcp_client.h"


#define SSL_CLIENT_ENABLE		1


#if SSL_CLIENT_ENABLE
	#define SSL_CLIENT_KEY_ADDR		0x9A
	#define SSL_CA_ADDR				0x9B
#endif

#define TCP_BUF_LEN		512
static u8 tcp_rev_buff[TCP_BUF_LEN];
static u16 rev_data_len;


static struct espconn tcp_client_conn;
struct espconn *ptcp_conn = &tcp_client_conn;

/*
 * 函数：tcp_client_send_cb
 * 说明：TCP客户端发送回调函数
 */
static void ICACHE_FLASH_ATTR
tcp_client_send_cb(void *arg)
{
	os_printf("tcp_client_send_cb\n");
}

/*
 * 函数：tcp_client_discon_cb
 * 说明：TCP客户端断开连接回调函数
 */
static void ICACHE_FLASH_ATTR
tcp_client_discon_cb(void *arg)
{
	os_printf("tcp_client_discon_cb\n");
}

/*
 * 函数：tcp_client_recv
 * 说明：TCP客户端接收回调函数
 */
static void ICACHE_FLASH_ATTR
tcp_client_recv(void *arg, char *pdata, unsigned short len)
{
	os_printf("tcp_client_recv\n");

	os_printf("lenth:%d\r\n",len);
	os_printf("data:%s\r\n",pdata);

	// TODO:
#if 0
	os_memset(tcp_rev_buff, 0, TCP_BUF_LEN);
	os_memcpy(tcp_rev_buff, pdata, len);
	rev_data_len = len;
#endif

}

/*
 * 函数：tcp_client_recon_cb
 * 说明：TCP客户端重连回调函数
 */
static void ICACHE_FLASH_ATTR
tcp_client_recon_cb(void *arg)
{
    struct espconn *pesp_conn = arg;

    os_printf("tcp_client_recon_cb\r\n");
}

/*
 * 函数：tcp_client_connect_cb
 * 说明：TCP客户端连接成功回调函数
 */
static void ICACHE_FLASH_ATTR
tcp_client_connect_cb(void *arg)
{
    struct espconn *pesp_conn = arg;

    espconn_regist_disconcb(pesp_conn, tcp_client_discon_cb);	//注册TCP客户端断开回调函数
    espconn_regist_recvcb(pesp_conn, tcp_client_recv);			//注册TCP客户端接收回调函数
    espconn_regist_sentcb(pesp_conn, tcp_client_send_cb); 		//注册TCP客户端发送回调函数
    //espconn_regist_reconcb(pesp_conn, tcp_client_recon_cb);

    os_printf("tcp_client_connect_cb\r\n");

    u8 *hello = "Hello World!";

#if SSL_CLIENT_ENABLE
    espconn_secure_send(pesp_conn, hello, os_strlen(hello));
#else
    espconn_send(pesp_conn, hello, os_strlen(hello));
#endif
}

/*
 * 函数：tcp_client_init
 * 参数：u8* ip - 服务器主机
 * 	   u16 port - 服务器端口
 * 说明：TCP客户端连接成功回调函数
 */
void ICACHE_FLASH_ATTR
tcp_client_init(u8* ip, u16 port)
{
	static esp_tcp esptcp;
    u32 u32_ip = ipaddr_addr(ip);
    s8 rc = 0;

    os_printf("tcp_client_init\n");

    tcp_client_conn.type = ESPCONN_TCP;
    tcp_client_conn.state = ESPCONN_NONE;
    tcp_client_conn.proto.tcp = &esptcp;

    os_memcpy(tcp_client_conn.proto.tcp->remote_ip, &u32_ip, 4);	//设置服务器IP
    tcp_client_conn.proto.tcp->remote_port = port;				//设置服务器PORT

    tcp_client_conn.proto.tcp->local_port = espconn_port();

    espconn_regist_connectcb(&tcp_client_conn, tcp_client_connect_cb);


#if SSL_CLIENT_ENABLE
    //espconn_secure_ca_enable(0x01, SSL_CA_ADDR);
    espconn_secure_cert_req_enable(0x01, SSL_CLIENT_KEY_ADDR);
    rc = espconn_secure_connect(&tcp_client_conn);
#else
    rc = espconn_connect(&tcp_client_conn);
#endif

    os_printf("connect to "IPSTR":%d rc=%d\r\n",
    		IP2STR(tcp_client_conn.proto.tcp->remote_ip),
			tcp_client_conn.proto.tcp->remote_port, rc);

}

