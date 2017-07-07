/*
 * user_wifi.c
 *
 *  Created on: Dec 30, 2014
 *      Author: yannaxiu
 */


#include "user_interface.h"
#include "osapi.h"
#include "espconn.h"
#include "os_type.h"
#include "mem.h"

#include "user_smart_config.h"

/*************************************************************/

#define HUMITURE_WIFI_LED_IO_MUX 	PERIPHS_IO_MUX_GPIO0_U
#define HUMITURE_WIFI_LED_IO_NUM 	0
#define HUMITURE_WIFI_LED_IO_FUNC 	FUNC_GPIO0

void ICACHE_FLASH_ATTR
wifi_status_len_init(void)
{
	// 使用 GPIO0 作为 WiFi 状态 LED
	wifi_status_led_install(HUMITURE_WIFI_LED_IO_NUM,
			HUMITURE_WIFI_LED_IO_MUX,
			HUMITURE_WIFI_LED_IO_FUNC
		);
}

/*************************************************************/

static os_timer_t g_wifi_timer;
static os_timer_t g_smartconig_led_timer;

/*
 * 函数：smartconfig_led_timer_cb
 * 说明：smartconfig状态灯定时器回调函数
 */
static void ICACHE_FLASH_ATTR
smartconfig_led_timer_cb(void *arg)
{
	u8 led = GPIO_INPUT_GET(0);
	GPIO_OUTPUT_SET(0, 1-led);		// 取反
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

	os_timer_disarm(&g_smartconig_led_timer);
	os_timer_setfn(&g_smartconig_led_timer, (os_timer_func_t *)smartconfig_led_timer_cb, NULL);
	os_timer_arm(&g_smartconig_led_timer, 1000, 1);
}

/*
 * 函数：wifi_status_change_cb
 * 说明：WiFi状态改变回调函数
 */
static void ICACHE_FLASH_ATTR
wifi_status_change_cb(void)
{
	u8 status = wifi_station_get_connect_status();
	os_printf("[INFO] wifi_connect_cb\r\n");

	if(status == STATION_GOT_IP){
		// TODO:
	} else {
		// TODO:
	}

}

/*
 * 函数：wifi_start_timer_cb
 * 说明：Wi-Fi首次连接定时器回调函数
 */
static void ICACHE_FLASH_ATTR
wifi_start_timer_cb(void *arg)
{
	static u8 s_flag = 0;
	static u8 s_wifi_led_init_flag = 0;
	struct ip_info ipConfig;

	os_timer_disarm(&g_wifi_timer);	// 停止该定时器
	wifi_get_ip_info(STATION_IF, &ipConfig);
	u8 wifi_status = wifi_station_get_connect_status();

	if(0==s_wifi_led_init_flag){
		s_wifi_led_init_flag = 1;
		// WiFi状态灯初始化
		wifi_status_len_init();
	}

	// 当WiFi连接成功后
	if (STATION_GOT_IP == wifi_status && ipConfig.ip.addr != 0){
		wifi_station_set_reconnect_policy(TRUE);	// 当从AP断开后自动重连
		wifi_station_set_auto_connect(TRUE);		// 开启自动连接
		os_printf("[INFO] Wi-Fi Connected!\r\n");
	}else{
		if(0==s_flag){	// 没有获取到smartconfig
			os_printf("[INFO] smartconfig stop!\r\n");
			smartconfig_stop();
			os_timer_disarm(&g_smartconig_led_timer);
			s_flag = 1;		// 标志不再进入该代码
		}else{
			os_printf("[ERROR] Wi-Fi Connect Fail!\r\n");
		}

		wifi_station_disconnect();
		wifi_station_connect();		// 尝试进行连接

		os_timer_setfn(&g_wifi_timer, (os_timer_func_t *)wifi_start_timer_cb, NULL);
		os_timer_arm(&g_wifi_timer, 10000, 0);	// 10秒后进行一次回调
	}
}


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

	wifi_status_change_cb();
}


void ICACHE_FLASH_ATTR
wifi_connect(void)
{
	wifi_set_opmode_current(STATION_MODE);		// 设置为STATION模式
	wifi_station_disconnect();
	wifi_station_set_reconnect_policy(FALSE);	// 连接 AP 失败或断开后是否重连。
	wifi_station_set_auto_connect(FALSE);		// 关闭自动连接

	// 首先启动SmartConfig
	user_smartconfig_init();

	// 一段时间后检查Wi-Fi连接状态
	os_timer_disarm(&g_wifi_timer);
	os_timer_setfn(&g_wifi_timer, (os_timer_func_t *)wifi_start_timer_cb, NULL);
	os_timer_arm(&g_wifi_timer, 20000, 0);	// 20秒后进行一次回调

	// 注册 Wi-Fi 事件
	wifi_set_event_handler_cb(wifi_handle_event_cb);
}


