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
#include "http_parse.h"
#include "user_config.h"
#include "spi_flash.h"
#include "upgrade.h"
#include "ringbuf.h"

//************************************************************/
#define DEBUG

#ifdef DEBUG
#define debug(fmt, args...) os_printf(fmt, ##args)
#define debugX(level, fmt, args...) if(DEBUG>=level) os_printf(fmt, ##args);
#else
#define debug(fmt, args...)
#define debugX(level, fmt, args...)
#endif /* DEBUG */

//************************************************************/

static struct espconn g_tcp_client_conn;
static struct espconn *g_ptcp_conn = &g_tcp_client_conn;

// 接收TCP数据缓存
#define RBUF_SIZE (1024*5)
static uint8_t rbuf[RBUF_SIZE];
static RINGBUF ringbuf;
static RINGBUF *pRingBuf = &ringbuf;

// flash写入缓存
#define WBUF_SIZR	4096
static uint8_t wbuf[WBUF_SIZR];

static s32 http_body_size = 0;
static u32 wAddr = USER1_BIN_FLASH_SECTOR;
static u32 g_tcp_pack_count = 0;	// 计算TCP包的个数

//************************************************************/

/*
 * function: tcp_client_send_cb
 */
static void ICACHE_FLASH_ATTR
tcp_client_send_cb(void *arg) {
	struct espconn *pesp_conn = arg;
	debug("tcp_client_send_cb\n");

	RINGBUF_Init(pRingBuf, rbuf, RBUF_SIZE);
	http_body_size = 0;
	g_tcp_pack_count = 0;
	system_upgrade_flag_set(UPGRADE_FLAG_START);
}

/*
 * function:tcp_client_discon_cb
 */
static void ICACHE_FLASH_ATTR
tcp_client_discon_cb(void *arg) {
	struct espconn *pesp_conn = arg;
	debug("tcp_client_discon_cb\n");
}

/*
 * function: tcp_client_recon_cb
 */
static void ICACHE_FLASH_ATTR
tcp_client_recon_cb(void *arg) {
	struct espconn *pesp_conn = arg;

	debug("tcp_client_recon_cb\r\n");
}

/*
 * 以扇区为大小进行烧写
 * function: write_flash_data
 * parameter: u32 addr - Flash addr
 *            u32 *pdata - data
 *            u32 size - size
 */
//static void ICACHE_FLASH_ATTR
static void write_flash_data(u32 addr, u32 *pdata, u32 size) {
	SpiFlashOpResult ret_spi;
	debug("write_flash_data@%d\r\n", addr);
	ret_spi = spi_flash_erase_sector(addr);	// 这个位置如果会卡住！！！是因为编译前没有clean
	switch (ret_spi) {
	case SPI_FLASH_RESULT_OK:
		debug("flash earse@%d\r\n", addr);
		break;
	case SPI_FLASH_RESULT_ERR:
		debug("flash earse error@%d\r\n", addr);
		break;
	case SPI_FLASH_RESULT_TIMEOUT:
		debug("flash earse timeout@%d\r\n", addr);
		break;
	default:
		debug("flash error@%d\r\n", addr);
		break;
	}
	ret_spi = spi_flash_write(addr * 4096, pdata, size);
	switch (ret_spi) {
	case SPI_FLASH_RESULT_OK:
		debug("flash write@%d\r\n", addr);
		break;
	case SPI_FLASH_RESULT_ERR:
		debug("flash write error@%d\r\n", addr);
		break;
	case SPI_FLASH_RESULT_TIMEOUT:
		debug("flash write timeout@%d\r\n", addr);
		break;
	default:
		debug("flash error@%d\r\n", addr);
		break;
	}
}

static int ICACHE_FLASH_ATTR
http_body_handle(const char *pdata, u32 len) {
	int ret = 0;

//	debug("http_body_handle\r\n");
	if (NULL == pdata) {
		return 0;
	}

// 使用RINGBUF进行数据缓存
	int i;
	for (i = 0; i < len; i++) {
		RINGBUF_Put(pRingBuf, pdata[i]);

		// 达到4096字节后就写入flash
		s32 dSize = RINGBUF_Use(pRingBuf);
//		debug("dSize=%d\r\n", dSize);
		if (4096 == dSize) {
//			debug("dSize=%d\r\n", dSize);
#if 0
			RINGBUF_Get(pRingBuf, wbuf, dSize);
//			debug("get finish!\r\n");
			write_flash_data(wAddr, (u32*) wbuf, dSize);
#else
			// 直接使用ringbuf数组
			write_flash_data(wAddr, (u32*) rbuf, dSize);
			RINGBUF_Init(pRingBuf, rbuf, RBUF_SIZE);
#endif
			++wAddr;
		}
	}

	if (http_body_size == 0) {
		u32 remainSize = RINGBUF_Use(pRingBuf);
		u8 exadd = 4 - (remainSize % 4);	// 4字节对齐：计算离4的倍数字节还差多少字节
#if 0
		RINGBUF_Get(pRingBuf, wbuf, remainSize);
		write_flash_data(wAddr, (u32*) wbuf, remainSize + exadd);
#else
		// 直接使用ringbuf数组
		write_flash_data(wAddr, (u32*) rbuf, remainSize + exadd);
		RINGBUF_Init(pRingBuf, rbuf, RBUF_SIZE);
#endif
		debug("http_body_handle finish!\r\n");
	}

	return wAddr;
}

/*
 * function: tcp_client_recv
 */
static void ICACHE_FLASH_ATTR
tcp_client_recv(void *arg, char *pdata, unsigned short len) {
	struct espconn *pesp_conn = arg;

	debug("tcp_client_recv\n");
//	debug("lenth:%d\r\n", len);
//	debug("data:\r\n%s\r\n", pdata);

	char *pBody = NULL;
	u32 header_len = 0;
// 获取http body size
	if (http_body_size == 0) {
		header_len = get_http_body(pdata, &pBody);
		debug("header_len:%d\r\n", header_len);
		http_body_size = get_http_header_length(pdata);
		debug("http_body_size:%d\r\n", http_body_size);
	} else {	// 剩余的TCP包包含HTTP Body
		pBody = pdata;
	}

// 如果HTTP Body还没有接受完
	if (http_body_size > 0) {
		debug("http_body_size:%d\r\n", http_body_size);
		// TODO:
		// 计算剩余未处理的body size
		http_body_size -= (len - header_len);
		// 处理http body
		http_body_handle(pBody, len - header_len);
	}

	g_tcp_pack_count++;
}

/*
 * function: tcp_client_connect_cb
 */
static void ICACHE_FLASH_ATTR
tcp_client_connect_cb(void *arg) {
	struct espconn *pesp_conn = arg;

	debug("tcp_client_connect_cb\r\n");

// 注册各种事件回调函数
	espconn_regist_disconcb(pesp_conn, tcp_client_discon_cb);
	espconn_regist_recvcb(pesp_conn, tcp_client_recv);
	espconn_regist_sentcb(pesp_conn, tcp_client_send_cb);
//espconn_regist_reconcb(pesp_conn, tcp_client_recon_cb);

//TODO:
// 发送请求
#if SSL_CLIENT_ENABLE
	espconn_secure_send(pesp_conn, hello, os_strlen(hello));
#else
//espconn_send(pesp_conn, hello, os_strlen(hello));
	if (1 == system_upgrade_userbin_check()) {
		wAddr = USER1_BIN_FLASH_SECTOR;	// 选择需要烧写的地址
		espconn_send(pesp_conn, HTTP_REQUEST, os_strlen(HTTP_REQUEST));
	} else {
		wAddr = USER2_BIN_FLASH_SECTOR;	// 选择需要烧写的地址
		espconn_send(pesp_conn, HTTP_REQUEST2, os_strlen(HTTP_REQUEST2));
	}

#endif
}

/* HTTP Client Init
 * function: http_client_init
 * parameter: char* ip - server ip
 *            u16 port - server port
 */
void ICACHE_FLASH_ATTR
http_client_init(char* ip, u16 port) {
	static esp_tcp esptcp;
	esp_tcp *pesptcp = &esptcp;
	u32 u32_ip = ipaddr_addr(ip);

	os_printf("http_client_init\n");

#if 0
	g_tcp_client_conn.type = ESPCONN_TCP;
	g_tcp_client_conn.state = ESPCONN_NONE;
	g_tcp_client_conn.proto.tcp = pesptcp;
#else
	g_ptcp_conn->type = ESPCONN_TCP;
	g_ptcp_conn->state = ESPCONN_NONE;
	g_ptcp_conn->proto.tcp = pesptcp;
#endif

#if 1
	os_memcpy(pesptcp->remote_ip, &u32_ip, 4); //set server ip
	pesptcp->remote_port = port;
	pesptcp->local_port = espconn_port();
#else
	os_memcpy(g_ptcp_conn->proto.tcp->remote_ip, &u32_ip, 4); //set server ip
	g_ptcp_conn->proto.tcp->remote_port = port;//set server port
	g_ptcp_conn->proto.tcp->local_port = espconn_port();
#endif
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
	debug("local "IPSTR":%d\r\n", IP2STR(g_ptcp_conn->proto.tcp->local_ip),
			g_ptcp_conn->proto.tcp->local_port);debug(
			"connect to "IPSTR":%d rc=%d\r\n",
			IP2STR(g_ptcp_conn->proto.tcp->remote_ip),
			g_ptcp_conn->proto.tcp->remote_port, rc);
}

/* 在线升级初始化
 * function: user_upgrade_init
 * parameter: void
 * return: void
 */
void ICACHE_FLASH_ATTR
user_upgrade_init(void) {
//	int i;
//	SpiFlashOpResult spi_ret;
//	for (i = 0; i < 512 / 4; i++) {
//		spi_ret = spi_flash_erase_sector(USER1_BIN_FLASH_SECTOR + i);
//		if (SPI_FLASH_RESULT_OK != spi_ret) {
//			os_printf("flash erase error@%d\r\n", i);
//		}
//	}
//	os_printf("flash erase finish!\r\n");
	system_upgrade_flag_set(UPGRADE_FLAG_START);
}

/* 在线升级结束，进行重启
 * function: user_upgrade_finish
 * parameter: void
 * return: void
 */
void ICACHE_FLASH_ATTR
user_upgrade_finish(void) {
	system_upgrade_flag_set(UPGRADE_FLAG_FINISH);
	system_upgrade_reboot();
}

