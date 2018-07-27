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
#include "user_config.h"

// 如果不需要debug，则注释
#define DEBUG	1

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


/**********************************************************/
#define GPIO_HIGH(x)	GPIO_OUTPUT_SET(x, 1)
#define GPIO_LOW(x)		GPIO_OUTPUT_SET(x, 0)
// GPIO reverse
#define GPIO_REV(x)		GPIO_OUTPUT_SET(x, (1-GPIO_INPUT_GET(x)))
/**********************************************************/

#if TIMER_MODE==WIFI_CHECK_MODE
// 每隔2s检查一次wifi状态
#define WiFi_CHECK_TIMER_INTERVAL	2000
static os_timer_t g_wifi_check_timer;
#endif

#if TIMER_MODE==WIFI_LED_MODE
// 每隔200ms发生一次wifi led事件
#define WiFi_LED_INTERVAL	200
static os_timer_t g_wifi_led_timer;
#endif

static struct ip_info g_ipConfig;
static os_timer_t g_wifi_connect_timer;
static os_timer_t g_smartconig_led_timer;


/*************************************************************/

u32 ICACHE_FLASH_ATTR
get_station_ip(void) {
	return g_ipConfig.ip.addr;
}

/*************************************************************/

// 定时检查wifi模式
#if TIMER_MODE==WIFI_CHECK_MODE

static void ICACHE_FLASH_ATTR
wifi_led_timer_cb(void *arg) {
	//wifi_get_ip_info(STATION_IF, &g_ipConfig);
	u8 status = wifi_station_get_connect_status();

	// 如果wifi连接成功，保持led常亮（低电平）
	// 否则，led闪烁
	// 闪烁间隔由 WiFi_LED_INTERVAL 确定
	if (status == STATION_GOT_IP) {
		GPIO_LOW(WIFI_STATUS_LED_PIN);
	} else {
		GPIO_REV(WIFI_STATUS_LED_PIN);
	}
}

#endif

void ICACHE_FLASH_ATTR
wifi_status_led_init(void) {
	static u8 s_wifi_led_init_flag = 0;

	// 如果已经初始化过了就不需要再初始化了
	if (1 == s_wifi_led_init_flag) {
		return;
	}

#if TIMER_MODE==WIFI_CHECK_MODE
	// 使用定时器控制led显示wifi状态
	debug("[INFO] WiFi_LED_STATUS_TIMER_ENABLE\r\n");
	os_timer_disarm(&g_wifi_connect_timer);
	os_timer_setfn(&g_wifi_connect_timer, (os_timer_func_t *) wifi_led_timer_cb, NULL);
	os_timer_arm(&g_wifi_connect_timer, WiFi_LED_INTERVAL, 1);
#elif ESP_MODE==WIFI_CHECK_MODE
	wifi_status_led_install(HUMITURE_WIFI_LED_IO_NUM, HUMITURE_WIFI_LED_IO_MUX,
			HUMITURE_WIFI_LED_IO_FUNC);
#else
#endif
	// set led init successful flag
	s_wifi_led_init_flag = 1;
}

/*************************************************************/

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
	//esptouch_set_timeout(30);
	smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS); //SC_TYPE_ESPTOUCH,SC_TYPE_AIRKISS,SC_TYPE_ESPTOUCH_AIRKISS
	smartconfig_start(smartconfig_done);
	debug("[INFO] smartconfig start!\r\n");
	//user_smartconfig_led_timer_init();
	GPIO_HIGH(WIFI_STATUS_LED_PIN);
}

/*************************************************************/
// wifi定时检查模式
#if TIMER_MODE==WIFI_CHECK_MODE

/*
 * function: wifi_check_timer_cb
 */
static void ICACHE_FLASH_ATTR
wifi_check_timer_cb(void) {
	static u8 from_disconnect_to_connect = 1;
	u8 status = wifi_station_get_connect_status();

	if (STATION_GOT_IP==status) {
		if (1==from_disconnect_to_connect){
			from_disconnect_to_connect = 0;
			debug("[INFO] wifi connected!\r\n");
			// TODO:
			// wifi连接成功后
		}
	} else {	// wifi断开
		from_disconnect_to_connect = 1;
	}
}
// 以下是事件检查wifi连接代码
#elif EVENT_MODE==WIFI_CHECK_MODE

/*
 * function: wifi_handle_event_cb
 */
void ICACHE_FLASH_ATTR
wifi_handle_event_cb(System_Event_t *evt)
{
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
		debug("mode: %d -> %d\n",
				evt->event_info.auth_change.old_mode,
				evt->event_info.auth_change.new_mode);
		break;
		case EVENT_STAMODE_GOT_IP:
		debug("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR,
				IP2STR(&evt->event_info.got_ip.ip),
				IP2STR(&evt->event_info.got_ip.mask),
				IP2STR(&evt->event_info.got_ip.gw));
		debug("\n");

		// TODO:
		// 当成功wifi连接成功后（获取到IP），需要做的事情

		break;
		case EVENT_SOFTAPMODE_STACONNECTED:
		debug("station: " MACSTR "join, AID = %d\n",
				MAC2STR(evt->event_info.sta_connected.mac),
				evt->event_info.sta_connected.aid);
		break;
		case EVENT_SOFTAPMODE_STADISCONNECTED:
		debug("station: " MACSTR "leave, AID = %d\n",
				MAC2STR(evt->event_info.sta_disconnected.mac),
				evt->event_info.sta_disconnected.aid);
		break;
		default:
		break;
	}
}
#endif	// WIFI_CHECK_MODE

/*
 * function: wifi_check_init
 * description: wifi检查初始化״̬
 */
void ICACHE_FLASH_ATTR
wifi_check_init(void) {
#if TIMER_MODE==WIFI_CHECK_MODE	// 定时模式
	os_timer_disarm(&g_wifi_check_timer);
	os_timer_setfn(&g_wifi_check_timer, (os_timer_func_t *) wifi_check_timer_cb, NULL);
	os_timer_arm(&g_wifi_check_timer, WiFi_CHECK_TIMER_INTERVAL, 1);
#elif EVENT_MODE==WIFI_CHECK_MODE	// 事件模式
	wifi_set_event_handler_cb(wifi_handle_event_cb);
#else
	debug("[INFO] disable Wi-Fi check!\r\n");
#endif

}

/*************************************************************/

/*
 * function: wifi_connect_timer_cb
 */
static void ICACHE_FLASH_ATTR
wifi_connect_timer_cb(void *arg) {
	static u8 s_smartconig_flag = 0;

	os_timer_disarm(&g_wifi_connect_timer);	// stop g_wifi_connect_timer

	wifi_get_ip_info(STATION_IF, &g_ipConfig);
	u8 wifi_status = wifi_station_get_connect_status();

	user_smartconfig_led_timer_stop();	// 停止smartconfig控制led

	wifi_check_init();		// wifi check
	wifi_status_led_init();	// wifi led


	wifi_station_set_reconnect_policy(TRUE);	//
	wifi_station_set_auto_connect(TRUE);		// auto connect wifi

	// 检查wifi是否连接
	if (STATION_GOT_IP == wifi_status && g_ipConfig.ip.addr != 0) {
		debug("[INFO] Wi-Fi connected!\r\n");
	} else {
		// 如果上电后第一次使用smartconfig而wifi没有连上，则需要停止smartconfig
		// 之后是wifi自动重连，就不需要停止smartconfig了
		if (0 == s_smartconig_flag) {
			debug("[INFO] smartconfig stop!\r\n");
			smartconfig_stop();
			s_smartconig_flag = 1;
		} else {
			debug("[ERROR] Wi-Fi connect fail!\r\n");
		}
		wifi_station_disconnect();
		wifi_station_connect();		// 尝试重连
	}
}

/*
 * function: wifi_connect_timer_init
 * description:
 */
static void ICACHE_FLASH_ATTR
wifi_connect_timer_init(void)
{
	os_timer_disarm(&g_wifi_connect_timer);
	os_timer_setfn(&g_wifi_connect_timer, (os_timer_func_t *) wifi_connect_timer_cb, NULL);
	os_timer_arm(&g_wifi_connect_timer, 20000, 0);	// 20s
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
wifi_connect(void) {
	wifi_set_opmode(STATION_MODE);		// set wifi mode
	wifi_station_disconnect();

#ifdef WIFI_SSID_ENABLE
	user_set_station_config(WIFI_SSID, WIFI_PASS);
	wifi_station_connect();

	wifi_check_init();		// wifi check
	wifi_status_led_init();	// wifi led

	/* WIFI_SSID_ENABLE */
#elif defined(SMARTCONFIG_ENABLE)
	debug("[INFO] SMARTCONFIG_ENABLE\r\n");
	wifi_station_set_reconnect_policy(FALSE);
	wifi_station_set_auto_connect(FALSE);

	user_smartconfig_init();

	wifi_connect_timer_init();

#if EVENT_MODE==WIFI_CHECK_MODE
	debug("[INFO] WIFI_EVENT_ENABLE\r\n");
	wifi_set_event_handler_cb(wifi_handle_event_cb);
#endif	/* WIFI_CHECK_MODE */

#else	// SMARTCONFIG_DISABLE
	wifi_station_set_reconnect_policy(TRUE);
	wifi_station_set_auto_connect(TRUE);
	wifi_station_connect();
	wifi_status_led_init();
	wifi_check_init();
#endif	/* SMARTCONFIG_ENABLE */
}

