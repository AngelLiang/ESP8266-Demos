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

#include "udp_server.h"


#define UDP_BUFF_SIZE		128
static u8 g_udp_buff[UDP_BUFF_SIZE];


/*
 * 函数: udp_get_peer_conn
 * 说明：获取对端的UDP信息
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
 * 函数：udp_print_conn
 * 说明：打印UDP对面连接的信息
 */
static void ICACHE_FLASH_ATTR
udp_print_conn(void *arg)
{
  struct espconn *pesp_conn = arg;

  os_printf("[INFO] udp recv from:"IPSTR":%d\r\n",
    IP2STR(pesp_conn->proto.udp->remote_ip), pesp_conn->proto.udp->remote_port);
}

/*
 * 函数：udp_server_sent_cb
 * 说明：发送回调
 */
static void ICACHE_FLASH_ATTR
udp_server_sent_cb(void *arg)
{
	// TODO:
}

/*
 * 函数：udp_server_recv
 * 说明：UDP Server接收回调
 */
static void ICACHE_FLASH_ATTR
udp_server_recv(void *arg, char *pdata, unsigned short len)
{
	struct espconn *pesp_conn = arg;
	u16 send_data_len = 0;

	udp_get_peer_conn(arg);
	udp_print_conn(arg);
	os_printf(pdata);
	os_printf("\r\n");

	// TODO:
	espconn_send(pesp_conn, pdata, len);
}

#define MULIT_UDP_IP "239.1.1.1"
/*
 * 函数：multiudp_init
 * 说明：多播UDP初始化
 */
void ICACHE_FLASH_ATTR
multiudp_init(void)
{
	static struct ip_info info;
	static ip_addr_t mulit_udp_ip;

	mulit_udp_ip.addr = ipaddr_addr(MULIT_UDP_IP);
	wifi_get_ip_info(STATION_IF, &info);

    os_printf("local ip:" IPSTR "\r\n", IP2STR(&info.ip));
    os_printf("multi udp ip:" IPSTR "\r\n", IP2STR(&mulit_udp_ip));
    s8 ret = espconn_igmp_join(&info.ip, &mulit_udp_ip);
    if(ESPCONN_OK==ret){
    	os_printf("multi udp join successful!\r\n");
    	udp_server_init(28266);
    }
}


/*
 * 函数：udp_server_init
 * 说明：UDP Server 初始化
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

	espconn_regist_recvcb(&s_udp_server, udp_server_recv);		// 注册UDP接收回调函数
	espconn_regist_sentcb(&s_udp_server, udp_server_sent_cb); 	// 注册UDP发送回调函数
	espconn_create(&s_udp_server);

	os_printf("udp_discover_server_init\r\n");
}
