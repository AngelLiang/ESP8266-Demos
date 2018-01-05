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
#include "lwip/tcp.h"


//*****************************************************************
// UDP Server

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
	os_printf("udp_demo_init\r\n");
}


//*****************************************************************
// TCP Server

/*
 * function: tcpserver_recv
 * parameter: void *arg -
 *            struct udp_pcb *pcb -
 *            struct pbuf *p -
 *            err_t err -
 * return: err_t
 * description:
 */
static ICACHE_FLASH_ATTR
err_t tcpserver_recv(void *arg, struct tcp_pcb *pcb,
		struct pbuf *p, err_t err)
{
	if(p!=NULL){
		tcp_recved(pcb, p->tot_len);	// 通知内核更新接收窗口
		tcp_write(pcb,p->payload,p->len,1);
		pbuf_free(p);			// 释放整个数据包
	}else if(err==ERR_OK){		// 如果发生连接错误
		return tcp_close(pcb);	// 关闭连接
	}
	return ERR_OK;				// 返回操作结果
}

/*
 * function: tcpserver_accept
 * parameter: void *arg -
 *            struct udp_pcb *pcb -
 *            err_t err -
 * return: err_t
 * description:
 */
static ICACHE_FLASH_ATTR
err_t tcpserver_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
	tcp_recv(pcb, tcpserver_recv);	// 向该连接注册函数tcpserver_recv
	return ERR_OK;
}

/*
 * function: tcpserver_init
 * parameter: void
 * return: void
 * description:
 */
void ICACHE_FLASH_ATTR
tcpserver_init(void)
{
	struct tcp_pcb *pcb;
	pcb = (struct tcp_pcb *)tcp_new();	// 申请一个TCP控制块
	tcp_bind(pcb, IP_ADDR_ANY, 6060);	// 将改控制块和本地端口6060绑定

	pcb = (struct tcp_pcb *)tcp_listen(pcb);	// 监听
	tcp_accept(pcb, tcpserver_accept);	// 注册连接回调函数tcpserver_accept
	os_printf("tcpserver_init\r\n");
}
