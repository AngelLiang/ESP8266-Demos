/*
 * lwip_demo.c
 *
 *  Created on: 2018年1月3日
 *      Author: Administrator
 *
 *  基于lwIP的raw/callback API编程
 */

#include "lwip/ip_addr.h"
#include "lwip/udp.h"

#define UDP_ECHO_PORT	7

/*
 * function: udp_demo_cb
 * parameter: void *arg -
 *            struct udp_pcb *upcb -
 *            struct pbuf *p       - recvive data，接收到的数据包
 *            struct ip_addr *addr - remote ip，发送数据的主机ip
 *            u16 port             - remote port，发送数据的主机port
 * description: udp recvive callback function
 *              UDP接收回调函数
 */
static void ICACHE_FLASH_ATTR
udp_demo_cb(void *arg, struct udp_pcb *upcb,
		struct pbuf *p, struct ip_addr* addr, u16 port)
{
	unsigned char* temp = (unsigned char*)addr;
	udp_sendto(upcb, p, addr, port);
	pbuf_free(p);	// 记得要手动清除
}

void ICACHE_FLASH_ATTR
udp_demo_init(void)
{
	struct udp_pcb *upcb;
	upcb = udp_new();
	udp_bind(upcb, IP_ADDR_ANY, UDP_ECHO_PORT);
	udp_recv(upcb, udp_demo_cb, NULL);
}
