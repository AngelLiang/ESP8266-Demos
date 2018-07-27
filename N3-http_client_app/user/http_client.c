/*
 * http_client.c
 */

#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "espconn.h"
#include "http_client.h"
#include "user_config.h"

#define TCP_BUF_LEN		2000
static u8 tcp_rev_buff[TCP_BUF_LEN];
static u16 rev_data_len;

static struct espconn g_tcp_client_conn;
static struct espconn *g_ptcp_conn = &g_tcp_client_conn;

/*
 * function: tcp_client_send_cb
 */
static void ICACHE_FLASH_ATTR
tcp_client_send_cb(void *arg) {
	os_printf("tcp_client_send_cb\n");
}

/*
 * function:tcp_client_discon_cb
 */
static void ICACHE_FLASH_ATTR
tcp_client_discon_cb(void *arg) {
	os_printf("tcp_client_discon_cb\n");
}

/*
 * function: tcp_client_recv
 */
static void ICACHE_FLASH_ATTR
tcp_client_recv(void *arg, char *pdata, unsigned short len) {
	os_printf("tcp_client_recv\n");

	os_printf("lenth:%d\r\n", len);
	os_printf("data:%s\r\n", pdata);

	// TODO:
#if 0
	os_memset(tcp_rev_buff, 0, TCP_BUF_LEN);
	os_memcpy(tcp_rev_buff, pdata, len);
	rev_data_len = len;
#endif

}

/*
 * function: tcp_client_recon_cb
 */
static void ICACHE_FLASH_ATTR
tcp_client_recon_cb(void *arg) {
	struct espconn *pesp_conn = arg;

	os_printf("tcp_client_recon_cb\r\n");
}

/*
 * function: tcp_client_connect_cb
 */
static void ICACHE_FLASH_ATTR
tcp_client_connect_cb(void *arg) {
	struct espconn *pesp_conn = arg;

	espconn_regist_disconcb(pesp_conn, tcp_client_discon_cb);
	espconn_regist_recvcb(pesp_conn, tcp_client_recv);
	espconn_regist_sentcb(pesp_conn, tcp_client_send_cb);
	//espconn_regist_reconcb(pesp_conn, tcp_client_recon_cb);

	os_printf("tcp_client_connect_cb\r\n");

#if SSL_CLIENT_ENABLE
	espconn_secure_send(pesp_conn, hello, os_strlen(hello));
#else
	//espconn_send(pesp_conn, hello, os_strlen(hello));
	espconn_send(pesp_conn, HTTP_REQUEST, os_strlen(HTTP_REQUEST));
#endif
}

/*
 * function: http_client_init
 * parameter: u8* ip - server ip
 *            u16 port - server port
 */
void ICACHE_FLASH_ATTR
http_client_init(u8* ip, u16 port) {
	static esp_tcp esptcp;
	esp_tcp *pesptcp = &esptcp;

	u32 u32_ip = ipaddr_addr(ip);

	os_printf("http_client_init\n");

//	g_tcp_client_conn.type = ESPCONN_TCP;
//	g_tcp_client_conn.state = ESPCONN_NONE;
//	g_tcp_client_conn.proto.tcp = pesptcp;

	g_ptcp_conn->type = ESPCONN_TCP;
	g_ptcp_conn->state = ESPCONN_NONE;
	g_ptcp_conn->proto.tcp = pesptcp;

	os_memcpy(pesptcp->remote_ip, &u32_ip, 4); //set server ip
	pesptcp->remote_port = port;
	pesptcp->local_port = espconn_port();

//	os_memcpy(g_ptcp_conn->proto.tcp->remote_ip, &u32_ip, 4); //set server ip
//	g_ptcp_conn->proto.tcp->remote_port = port;			//set server port
//	g_ptcp_conn->proto.tcp->local_port = espconn_port();

	espconn_regist_connectcb(g_ptcp_conn, tcp_client_connect_cb);

}

void ICACHE_FLASH_ATTR
http_client_connect(void) {
	s8 rc = 0;
	espconn_disconnect(g_ptcp_conn);
#if SSL_CLIENT_ENABLE
	//espconn_secure_ca_enable(0x01, SSL_CA_ADDR);
	espconn_secure_cert_req_enable(0x01, SSL_CLIENT_KEY_ADDR);
	rc = espconn_secure_connect(g_ptcp_conn);
#else
	rc = espconn_connect(g_ptcp_conn);
#endif
	os_printf("local "IPSTR":%d\r\n", IP2STR(g_ptcp_conn->proto.tcp->local_ip),
			g_ptcp_conn->proto.tcp->local_port);
	os_printf("connect to "IPSTR":%d rc=%d\r\n",
			IP2STR(g_ptcp_conn->proto.tcp->remote_ip),
			g_ptcp_conn->proto.tcp->remote_port, rc);
}

