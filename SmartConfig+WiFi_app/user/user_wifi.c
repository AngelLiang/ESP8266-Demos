/*
 * user_wifi.c
 *
 *  Created on: 2017-07-15
 *      Author: yannanxiu
 */

#include "user_interface.h"
#include "osapi.h"
#include "espconn.h"
#include "os_type.h"
#include "mem.h"

#include "user_smartconfig.h"
#include "user_wifi.h"


/**********************************************************/
#define GPIO_HIGH(x) 	GPIO_OUTPUT_SET(x, 1)
#define GPIO_LOW(x) 	GPIO_OUTPUT_SET(x, 0)
// 引脚取反
#define GPIO_REV(x) 	GPIO_OUTPUT_SET(x, 1-GPIO_INPUT_GET(x))
/**********************************************************/


// Wi-Fi 事件回调使能
#define WIFI_EVENT_ENABLE		0
// Wi-Fi 状态灯定义
#define WIFI_STATUS_LED_PIN		0

// Wi-Fi 定时检查
#if WiFi_CHECK_TIMER_ENABLE
	#define WiFi_CHECK_TIMER_INTERVAL	2000
	static os_timer_t g_wifi_check_timer;

	#define WiFi_LED_INTERVAL	200
	static os_timer_t g_wifi_led_timer;
#endif


static struct ip_info g_ipConfig;
static os_timer_t g_wifi_start_timer;
static os_timer_t g_smartconig_led_timer;

/*************************************************************/


#if WiFi_LED_STATUS_TIMER_ENABLE
/*
 * 函数：wifi_led_timer_cb
 * 说明：LED灯定时回调
 */
static void ICACHE_FLASH_ATTR
wifi_led_timer_cb(void *arg)
{
	//wifi_get_ip_info(STATION_IF, &g_ipConfig);		// 获取station信息
	u8 status = wifi_station_get_connect_status();

	if(status == STATION_GOT_IP){
		GPIO_LOW(WIFI_STATUS_LED_PIN);
	}else{
		GPIO_REV(WIFI_STATUS_LED_PIN);
	}
}
#else
	#define HUMITURE_WIFI_LED_IO_MUX 	PERIPHS_IO_MUX_GPIO0_U
	#define HUMITURE_WIFI_LED_IO_NUM 	0
	#define HUMITURE_WIFI_LED_IO_FUNC 	FUNC_GPIO0
#endif

void ICACHE_FLASH_ATTR
wifi_status_led_init(void)
{
	static u8 s_wifi_led_init_flag = 0;
	// 确保只初始化一次
	if(1==s_wifi_led_init_flag){
		return;
	}

	// 使用 GPIO0 作为 WiFi 状态 LED
#if WiFi_LED_STATUS_TIMER_ENABLE
	os_timer_disarm(&g_wifi_start_timer);
	os_timer_setfn(&g_wifi_start_timer, (os_timer_func_t *)wifi_led_timer_cb, NULL);
	os_timer_arm(&g_wifi_start_timer, WiFi_LED_INTERVAL, 1);
#else
	wifi_status_led_install(HUMITURE_WIFI_LED_IO_NUM,
			HUMITURE_WIFI_LED_IO_MUX,
			HUMITURE_WIFI_LED_IO_FUNC
		);
#endif
	s_wifi_led_init_flag = 1;
}

/*************************************************************/

/*
 * 函数：get_station_ip
 * 说明：获取IP地址
 */
u32 ICACHE_FLASH_ATTR
get_station_ip(void)
{
	return g_ipConfig.ip.addr;
}

/*
 * 函数：smartconfig_led_timer_cb
 * 说明：smartconfig状态灯定时器回调函数
 */
static void ICACHE_FLASH_ATTR
smartconfig_led_timer_cb(void *arg)
{
	GPIO_REV(WIFI_STATUS_LED_PIN);
}

/*
 * 函数：user_smartconfig_led_timer_init
 * 说明：smartconfig LED状态灯定时初始化
 */
void ICACHE_FLASH_ATTR
user_smartconfig_led_timer_init(void)
{
	os_timer_disarm(&g_smartconig_led_timer);
	os_timer_setfn(&g_smartconig_led_timer, (os_timer_func_t *)smartconfig_led_timer_cb, NULL);
	os_timer_arm(&g_smartconig_led_timer, 1000, 1);
}

/*
 * 函数：user_smartconfig_led_timer_stop
 * 说明：smartconfig LED状态灯定时停止
 */
void ICACHE_FLASH_ATTR
user_smartconfig_led_timer_stop(void)
{
	os_timer_disarm(&g_smartconig_led_timer);
}

/*
 * 函数：user_smartconfig_init
 * 说明：smartconfig初始化
 */
static void ICACHE_FLASH_ATTR
user_smartconfig_init(void)
{
	//esptouch_set_timeout(30);
	smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS); //SC_TYPE_ESPTOUCH,SC_TYPE_AIRKISS,SC_TYPE_ESPTOUCH_AIRKISS
	smartconfig_start(smartconfig_done);
	os_printf("[INFO] smartconfig start!\r\n");

	// 应该放在 SC_STATUS_FIND_CHANNEL 初始化
	//user_smartconfig_led_timer_init();
	GPIO_HIGH(WIFI_STATUS_LED_PIN);
}

#if WiFi_CHECK_TIMER_ENABLE

/*
 * 函数：wifi_check_timer_cb
 * 说明：Wi-Fi检查定时回调
 */
static void ICACHE_FLASH_ATTR
wifi_check_timer_cb(void)
{
	u8 status = wifi_station_get_connect_status();
	//os_printf("[INFO] wifi_connect_cb\r\n");

	// TODO:
	if(status == STATION_GOT_IP){

	}else{

	}
}

/*
 * 函数：wifi_check_timer_init
 * 说明：一段时间后检查Wi-Fi连接状态
 */
void ICACHE_FLASH_ATTR
wifi_check_timer_init(void)
{
	static u8 s_flag = 0;
	// 确保只初始化一次
	if(1==s_flag){
		return;
	}
	os_timer_disarm(&g_wifi_check_timer);
	os_timer_setfn(&g_wifi_check_timer, (os_timer_func_t *)wifi_check_timer_cb, NULL);
	os_timer_arm(&g_wifi_check_timer, WiFi_CHECK_TIMER_INTERVAL, 1);	// 每2秒后进行一次回调

}

#endif

/*
 * 函数：wifi_start_timer_cb
 * 说明：上电后smartconfig到期回调
 */
static void ICACHE_FLASH_ATTR
wifi_start_timer_cb(void *arg)
{
	static u8 s_smartconig_flag = 0;

	os_timer_disarm(&g_wifi_start_timer);	// 停止该定时器
	wifi_get_ip_info(STATION_IF, &g_ipConfig);		// 获取station信息
	u8 wifi_status = wifi_station_get_connect_status();

	// 停止smartconfig的led灯闪烁
	user_smartconfig_led_timer_stop();
	// Wi-Fi 状态灯初始化
	wifi_status_led_init();
	// Wi-Fi 检查状态
	wifi_check_timer_init();

	wifi_station_set_reconnect_policy(TRUE);	// AP断开自动重连
	wifi_station_set_auto_connect(TRUE);		// 上电自动重连

	// 当WiFi连接成功后
	if (STATION_GOT_IP == wifi_status && g_ipConfig.ip.addr != 0){
		os_printf("[INFO] Wi-Fi Connected!\r\n");
	}else{
		if(0==s_smartconig_flag){	// 没有获取到smartconfig
			os_printf("[INFO] smartconfig stop!\r\n");
			smartconfig_stop();
			s_smartconig_flag = 1;		// 标志不再进入该代码
		}else{
			os_printf("[ERROR] Wi-Fi Connect Fail!\r\n");
		}
		wifi_station_disconnect();
		wifi_station_connect();		// 尝试进行连接
	}
}


#if WIFI_EVENT_ENABLE
/*
 * 函数：wifi_handle_event_cb
 * 说明：WiFi事件回调函数，暂时没用
 */
void ICACHE_FLASH_ATTR
wifi_handle_event_cb(System_Event_t *evt)
{
	os_printf("event %x\n", evt->event);
	switch (evt->event) {
		case EVENT_STAMODE_CONNECTED:
			os_printf("connect to ssid %s, channel %d\n",
			evt->event_info.connected.ssid,
			evt->event_info.connected.channel);
		break;
		case EVENT_STAMODE_DISCONNECTED:
			os_printf("disconnect from ssid %s, reason %d\n",
			evt->event_info.disconnected.ssid,
			evt->event_info.disconnected.reason);
		break;
		case EVENT_STAMODE_AUTHMODE_CHANGE:
			os_printf("mode: %d -> %d\n",
			evt->event_info.auth_change.old_mode,
			evt->event_info.auth_change.new_mode);
		break;
		case EVENT_STAMODE_GOT_IP:
		os_printf("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR,
			IP2STR(&evt->event_info.got_ip.ip),
			IP2STR(&evt->event_info.got_ip.mask),
			IP2STR(&evt->event_info.got_ip.gw));
			os_printf("\n");

			// TODO:


		break;
			case EVENT_SOFTAPMODE_STACONNECTED:
			os_printf("station: " MACSTR "join, AID = %d\n",
			MAC2STR(evt->event_info.sta_connected.mac),
			evt->event_info.sta_connected.aid);
		break;
		case EVENT_SOFTAPMODE_STADISCONNECTED:
			os_printf("station: " MACSTR "leave, AID = %d\n",
			MAC2STR(evt->event_info.sta_disconnected.mac),
			evt->event_info.sta_disconnected.aid);
		break;
		default:
		break;
	}
}
#endif

/*
 * 函数：wifi_connect
 * 说明：
 */
void ICACHE_FLASH_ATTR
wifi_connect(void)
{
	wifi_set_opmode(STATION_MODE);		// 设置为STATION模式
	wifi_station_disconnect();

#if SMARTCONFIG_DISABLE
	wifi_station_set_reconnect_policy(TRUE);
	wifi_station_set_auto_connect(TRUE);
	wifi_station_connect();
	// Wi-Fi 状态灯初始化
	wifi_status_led_init();
	// Wi-Fi 检查状态
	wifi_check_timer_init();
#else
	wifi_station_set_reconnect_policy(FALSE);	// 连接 AP 失败或断开后是否重连。
	wifi_station_set_auto_connect(FALSE);		// 关闭自动连接

	// 首先启动SmartConfig
	user_smartconfig_init();

	// 一段时间后检查Wi-Fi连接状态
	os_timer_disarm(&g_wifi_start_timer);
	os_timer_setfn(&g_wifi_start_timer, (os_timer_func_t *)wifi_start_timer_cb, NULL);
	os_timer_arm(&g_wifi_start_timer, 20000, 0);	// 20秒后进行一次回调

	// 由于smartconfig的存在，最好还是定时检查Wi-FI连接状态
	// 注册 Wi-Fi 事件
#if WIFI_EVENT_ENABLE
	wifi_set_event_handler_cb(wifi_handle_event_cb);
#endif
#endif
}


