/*
 * telnet_server.c
 *
 * 基于raw/callback API的Telnet服务器代码
 */

#include "telnet_server.h"

#include "lwip/ip_addr.h"
#include "lwip/tcp.h"

// sndbuf 发送缓冲区
// cmdbuf 命令行缓冲区
char sndbuf[128], cmdbuf[32];

/*
 * function: telnet_recv
 */
static ICACHE_FLASH_ATTR
err_t telnet_recv(void *arg, struct tcp_pcb *pcb,
		struct pbuf *p, err_t err)
{
	s16 ret;
	u16 len;
	u8 *pdat;
	char *pend;
	u16 datlen;

	if(p==NULL){
		return tcp_close(pcb);
	}

	tcp_recved(pcb, p->tot_len);
	len = p->len;
	pdat = (u8*)p->payload;

	os_printf("len:%d\r\n", len);
	os_printf("pdat:%s\r\n", pdat);

	// 检查接收命令长度
	if(len>31){
		datlen = os_sprintf(sndbuf,"Command is too long!\r\n");
		tcp_write(pcb, sndbuf, datlen, TCP_WRITE_FLAG_COPY);
		goto shell;
	}

	// 查找"\r\n"
	pend = strstr(pdat, "\r\n");
	if(NULL==pend){
		os_printf("Not Found \"\\r\\n\"");
		goto exit;
	}

	if(len>2){
		//memset(cmdbuf, sizeof(cmdbuf), 0);
		// 去除"\r\n"
		strncpy(cmdbuf, pdat, len-2);
		cmdbuf[len-2] = '\0';
		os_printf("cmdbuf:%s\r\n", cmdbuf);

		if(0==strncmp(cmdbuf, "date", strlen("date"))) {
			datlen = os_sprintf(sndbuf,"Now\r\n");
		}else if (0==strncmp(cmdbuf, "hello", strlen("hello"))) {
			datlen = os_sprintf(sndbuf,"Hello\r\n");
		}else if (0==strncmp(cmdbuf, "more", strlen("more"))) {
			datlen = os_sprintf(sndbuf,"Add whatever you need in this way!\r\n");
		}else if (0==strncmp(cmdbuf, "help", strlen("help"))) {
			datlen = os_sprintf(sndbuf,"Supported Command: date hello more help quit\r\n");
		}else if (0==strncmp(cmdbuf, "quit", strlen("quit"))) {
			pbuf_free(p);
			return tcp_close(pcb);
		}else{
			datlen = os_sprintf(sndbuf, "Unkonwn Command: %s. \r\n", cmdbuf);
		}
		tcp_write(pcb, sndbuf, datlen, TCP_WRITE_FLAG_COPY);
	}

shell:
	datlen = os_sprintf(sndbuf, "\r\nESP8266_Shell>>");
	tcp_write(pcb, sndbuf, datlen, TCP_WRITE_FLAG_COPY);

exit:
	pbuf_free(p);
	return ERR_OK;
}

/*
 * function: telnet_accept
 */
static ICACHE_FLASH_ATTR
err_t telnet_accept(void *arg, struct tcp_pcb* pcb, err_t err)
{
	u16 strlen;
	tcp_recv(pcb, telnet_recv);
	strlen = os_sprintf(sndbuf, "##Welcome to demo TELET based on LwIP##\r\n");
	tcp_write(pcb, sndbuf, strlen, TCP_WRITE_FLAG_COPY);

	strlen = os_sprintf(sndbuf, "##quit: quit\thelp: help\t##\r\n");
	tcp_write(pcb, sndbuf, strlen, TCP_WRITE_FLAG_COPY);

	strlen = os_sprintf(sndbuf, "\r\nESP8266_Shell>>");
	tcp_write(pcb, sndbuf, strlen, TCP_WRITE_FLAG_COPY);

	return ERR_OK;
}

/*
 * function: telnet_init
 */
void ICACHE_FLASH_ATTR
telnet_init(void)
{
	struct tcp_pcb *pcb;
	pcb = tcp_new();
	tcp_bind(pcb, IP_ADDR_ANY, 23);
	pcb = tcp_listen(pcb);
	tcp_accept(pcb, telnet_accept);
	os_printf("telnet_init\r\n");
}
