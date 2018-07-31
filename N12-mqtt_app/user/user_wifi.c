/*
 * user_wifi.c
 */

#include "user_interface.h"
#include "osapi.h"
#include "espconn.h"
#include "os_type.h"
#include "mem.h"

#include "user_smartconfig.h"
#include "user_wifi.h"

//***********************************************************************/

// 如果不需要debug，则注释
#define DEBUG	1

// 如果连接成功，每隔 2s检查一次wifi状态
#define WIFI_CHECK_TIMER_INTERVAL	(2*1000)
// 如果连接失败，每500ms检测一次
#define WIFI_CLOSE_CHECK_TIMER_INTERVAL		(500)
// 每隔200ms发生一次wifi led事件
#define WIFI_LED_INTERVAL	200
// smartconfig 上电后等待时间
#define SMARTCONFIG_WAIT_TIME	(20*1000)

//***********************************************************************/
// debug
#define PR	os_printf

#ifdef DEBUG
#define debug(fmt, args...) PR(fmt, ##args)
#define debugX(level, fmt, args...) if(DEBUG>=level) PR(fmt, ##args);
#else
#define debug(fmt, args...)
#define debugX(level, fmt, args...)
#endif	/* DEBUG */

//***********************************************************************/
// define
#define GPIO_HIGH(x)	GPIO_OUTPUT_SET(x, 1)
#define GPIO_LOW(x)		GPIO_OUTPUT_SET(x, 0)
// GPIO reverse
#define GPIO_REV(x)		GPIO_OUTPUT_SET(x, (1-GPIO_INPUT_GET(x)))

//***********************************************************************/
// gloabl variable
static os_timer_t g_wifi_check_timer;
static os_timer_t g_wifi_led_timer;

static struct ip_info g_ipConfig;
static os_timer_t g_wifi_smartconfig_timer;
static os_timer_t g_smartconig_led_timer;

// 用户wifi回调函数
WifiCallback wifiCb = NULL;
// 记录wifi状态
static u8 wifiStatus = STATION_IDLE, lastWifiStatus = STATION_IDLE;

//***********************************************************************/

u32 ICACHE_FLASH_ATTR
get_station_ip(void) {
	return g_ipConfig.ip.addr;
}

/*************************************************************/

static void ICACHE_FLASH_ATTR
wifi_led_timer_cb(void *arg) {
	//wifi_get_ip_info(STATION_IF, &g_ipConfig);
	u8 status = wifi_station_get_connect_status();

	// 如果wifi连接成功，保持led常亮（低电平）
	// 否则，led闪烁
	// 闪烁间隔由 WIFI_LED_INTERVAL 确定
	if (status == STATION_GOT_IP) {
		GPIO_LOW(WIFI_STATUS_LED_PIN);
	} else {
		GPIO_REV(WIFI_STATUS_LED_PIN);
	}
}

void ICACHE_FLASH_ATTR
wifi_status_led_init(void) {
	// 使用定时器控制led显示wifi状态
	debug("[INFO] WiFi_LED_STATUS_TIMER_ENABLE\r\n");
	os_timer_disarm(&g_wifi_smartconfig_timer);
	os_timer_setfn(&g_wifi_smartconfig_timer,
			(os_timer_func_t *) wifi_led_timer_cb,
			NULL);
	os_timer_arm(&g_wifi_smartconfig_timer, WIFI_LED_INTERVAL, 1);

}

/*************************************************************/
// smartconfig led config
/*
 * function: smartconfig_led_timer_cb
 */
static void ICACHE_FLASH_ATTR
smartconfig_led_timer_cb(void *arg) {
	GPIO_REV(WIFI_STATUS_LED_PIN);		// led闪烁
}

/*
 * function: user_smartconfig_led_timer_init
 * description: smartconfig闪烁led事件，间隔1000ms
 */
void ICACHE_FLASH_ATTR
user_smartconfig_led_timer_init(void) {
	os_timer_disarm(&g_smartconig_led_timer);
	os_timer_setfn(&g_smartconig_led_timer,
			(os_timer_func_t *) smartconfig_led_timer_cb, NULL);
	os_timer_arm(&g_smartconig_led_timer, 1000, 1);
}

/*
 * function: user_smartconfig_led_timer_stop
 * description: 停止smartconfig led闪烁ֹ
 */
void ICACHE_FLASH_ATTR
user_smartconfig_led_timer_stop(void) {
	os_timer_disarm(&g_smartconig_led_timer);
}

/*
 * function: user_smartconfig_init
 */
static void ICACHE_FLASH_ATTR
user_smartconfig_init(void) {
	// smartconfig 仅支持在单 Station 模式下调接口
	wifi_set_opmode(STATION_MODE);		// set wifi mode
	// FALSE - 执行完 user_init 函数后不连接
	wifi_station_set_auto_connect(FALSE);

	//esptouch_set_timeout(30);
	//SC_TYPE_ESPTOUCH,SC_TYPE_AIRKISS,SC_TYPE_ESPTOUCH_AIRKISS
	smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS);
	smartconfig_start(smartconfig_done);

	GPIO_HIGH(WIFI_STATUS_LED_PIN);
	debug("[INFO] smartconfig start!\r\n");
}

/*************************************************************/
static void ICACHE_FLASH_ATTR
wifi_handle_event_cb(System_Event_t *evt) {
#if 0
	debug("event %x\n", evt->event);
	switch (evt->event) {
		case EVENT_STAMODE_CONNECTED:
		debug("connect to ssid %s, channel %d\n",
				evt->event_info.connected.ssid,
				evt->event_info.connected.channel);
		break;
		case EVENT_STAMODE_DISCONNECTED:
		debug("disconnect from ssid %s, reason %d\n",
				evt->event_info.disconnected.ssid,
				evt->event_info.disconnected.reason);
		break;
		case EVENT_STAMODE_AUTHMODE_CHANGE:
		debug("mode: %d -> %d\n", evt->event_info.auth_change.old_mode,
				evt->event_info.auth_change.new_mode);
		break;
		case EVENT_STAMODE_GOT_IP:
		debug("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR,
				IP2STR(&evt->event_info.got_ip.ip),
				IP2STR(&evt->event_info.got_ip.mask),
				IP2STR(&evt->event_info.got_ip.gw));
		debug("\n");
		break;
		case EVENT_SOFTAPMODE_STACONNECTED:
		debug("station:	" MACSTR "join,	AID	=	%d\n",
				MAC2STR(evt->event_info.sta_connected.mac),
				evt->event_info.sta_connected.aid);
		break;
		case EVENT_SOFTAPMODE_STADISCONNECTED:
		debug("station:	" MACSTR "leave, AID	=	%d\n",
				MAC2STR(evt->event_info.sta_disconnected.mac),
				evt->event_info.sta_disconnected.aid);
		break;
		default:
		break;
	}
#endif
}

/*************************************************************/
// wifi定时检查
/*
 * function: wifi_check_timer_cb
 * description: wiif检查回调函数
 *              wiif连接成功时每 2s 检查一次
 *              wiif断开时每 500ms 检查一次
 */
static void ICACHE_FLASH_ATTR
wifi_check_timer_cb(void) {

	wifi_get_ip_info(STATION_IF, &g_ipConfig);
	wifiStatus = wifi_station_get_connect_status();

	if (wifiStatus == STATION_GOT_IP && g_ipConfig.ip.addr != 0) {
		wifi_check_init(WIFI_CHECK_TIMER_INTERVAL);

	} else {	// wifi断开
		debug("wifi connect fail!\r\n");
		wifi_check_init(WIFI_CLOSE_CHECK_TIMER_INTERVAL);
		//wifi_station_disconnect();
		//wifi_station_connect();		// 尝试重连
	}

	// user callback
	if (wifiStatus != lastWifiStatus) {
		lastWifiStatus = wifiStatus;
		if (wifiCb) {
			wifiCb(wifiStatus);
		}
	}

}

/*
 * function: wifi_check_init
 * parameter: u16 interval - 定时回调时间
 * description: wifi检查初始化״̬
 */
void ICACHE_FLASH_ATTR
wifi_check_init(u16 interval) {
	/*
	 * 调用 wifi_station_set_reconnect_policy 关闭重连功能，
	 * 且未调用 wifi_set_event_handler_cb 注册 Wi-Fi 事件回调，
	 * 则 wifi_station_get_connect_status接口  失效，无法准确获得连接状态。
	 */

	// 设置 ESP8266 Station 连接 AP 失败或断开后是否重连。
	wifi_station_set_reconnect_policy(TRUE);
	wifi_set_event_handler_cb(wifi_handle_event_cb);

	os_timer_disarm(&g_wifi_check_timer);
	os_timer_setfn(&g_wifi_check_timer, (os_timer_func_t *) wifi_check_timer_cb,
	NULL);
	os_timer_arm(&g_wifi_check_timer, interval, 0);
}

/*************************************************************/

/*
 * function: wifi_connect_timer_cb
 */
static void ICACHE_FLASH_ATTR
wifi_smartconfig_timer_cb(void *arg) {
	struct station_config stationConf;

	debug("wifi_smartconfig_timer_cb\r\n");

	os_timer_disarm(&g_wifi_smartconfig_timer);	// stop g_wifi_connect_timer

	user_smartconfig_led_timer_stop();	// 停止smartconfig控制led

	debug("[INFO] smartconfig stop!\r\n");
	smartconfig_stop();			// 无论smartconfig是否成功都要释放

	// 查询 Wi-Fi Station 接口保存在 Flash 中的配置参数。
	wifi_station_get_config_default(&stationConf);
	debug("wifi ssid:%s \r\nwifi pass:%s \r\n", stationConf.ssid,
			stationConf.password);
	//wifi_station_set_config(&stationConf);

	wifi_station_disconnect();
	wifi_station_connect();		// 尝试连接

	wifi_check_init(WIFI_CHECK_TIMER_INTERVAL);		// wifi check
	wifi_status_led_init();	// wifi led

}

/*
 * function: wifi_connect_timer_init
 * description:
 */
void ICACHE_FLASH_ATTR
wifi_smartconfig_timer_init(void) {
	os_timer_disarm(&g_wifi_smartconfig_timer);
	os_timer_setfn(&g_wifi_smartconfig_timer,
			(os_timer_func_t *) wifi_smartconfig_timer_cb,
			NULL);
	os_timer_arm(&g_wifi_smartconfig_timer, SMARTCONFIG_WAIT_TIME, 0);
}

void ICACHE_FLASH_ATTR
wifi_smartconfig_timer_stop(void) {
	os_timer_disarm(&g_wifi_smartconfig_timer);
}

/*************************************************************/

/*
 * function: user_set_station_config
 * parameter: u8* ssid - WiFi SSID
 *            u8 password - WiFi password
 * return: void
 */
void ICACHE_FLASH_ATTR
user_set_station_config(u8* ssid, u8* password) {
	struct station_config stationConf;
	stationConf.bssid_set = 0;		//need not check MAC address of AP
	os_memcpy(&stationConf.ssid, ssid, 32);
	os_memcpy(&stationConf.password, password, 64);
	wifi_station_set_config(&stationConf);
}

/*
 * function: wifi_connect
 */
void ICACHE_FLASH_ATTR
wifi_connect(WifiCallback cb) {
	// smartconfig 仅支持在单 Station 模式下调接口
	//wifi_set_opmode(STATION_MODE);		// set wifi mode
	wifiCb = cb;
	wifi_station_disconnect();

	// 上电是否自动连接
	// TRUE - 执行完 user_init 函数后则自动连接
	wifi_station_set_auto_connect(TRUE);

	user_set_station_config(WIFI_SSID, WIFI_PASS);
	wifi_station_connect();

	wifi_status_led_init();	// wifi led
	wifi_check_init(WIFI_CHECK_TIMER_INTERVAL);	// wifi check

}

void ICACHE_FLASH_ATTR
smartconfig_connect(WifiCallback cb) {
	wifiCb = cb;
	wifi_station_disconnect();

	user_smartconfig_init();
	wifi_smartconfig_timer_init();
}

