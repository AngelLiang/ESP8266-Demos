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
#include "user_config.h"

/**********************************************************/
#define GPIO_HIGH(x)	GPIO_OUTPUT_SET(x, 1)
#define GPIO_LOW(x)		GPIO_OUTPUT_SET(x, 0)
// GPIO reverse
#define GPIO_REV(x)		GPIO_OUTPUT_SET(x, (1-GPIO_INPUT_GET(x)))
/**********************************************************/

#if 1==WIFI_CHECK_MODE
#define WiFi_CHECK_TIMER_INTERVAL	2000
static os_timer_t g_wifi_check_timer;
#endif

#if 1==WIFI_CHECK_MODE
#define WiFi_LED_INTERVAL	200
static os_timer_t g_wifi_led_timer;
#endif

static struct ip_info g_ipConfig;
static os_timer_t g_wifi_start_timer;
static os_timer_t g_smartconig_led_timer;

/*************************************************************/

#if 1==WIFI_CHECK_MODE

static void ICACHE_FLASH_ATTR
wifi_led_timer_cb(void *arg) {
	//wifi_get_ip_info(STATION_IF, &g_ipConfig);
	u8 status = wifi_station_get_connect_status();

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

	// check wifi led is init?
	if (1 == s_wifi_led_init_flag) {
		return;
	}

#if 1==WIFI_CHECK_MODE
	os_printf("WiFi_LED_STATUS_TIMER_ENABLE\r\n");
	os_timer_disarm(&g_wifi_start_timer);
	os_timer_setfn(&g_wifi_start_timer, (os_timer_func_t *) wifi_led_timer_cb,
			NULL);
	os_timer_arm(&g_wifi_start_timer, WiFi_LED_INTERVAL, 1);
#elif 2==WIFI_CHECK_MODE
	wifi_status_led_install(HUMITURE_WIFI_LED_IO_NUM, HUMITURE_WIFI_LED_IO_MUX,
			HUMITURE_WIFI_LED_IO_FUNC);
#else
#endif
	// set led init successful flag
	s_wifi_led_init_flag = 1;
}

/*************************************************************/

u32 ICACHE_FLASH_ATTR
get_station_ip(void) {
	return g_ipConfig.ip.addr;
}

/*
 * function: smartconfig_led_timer_cb
 */
static void ICACHE_FLASH_ATTR
smartconfig_led_timer_cb(void *arg) {
	GPIO_REV(WIFI_STATUS_LED_PIN);
}

/*
 * function: user_smartconfig_led_timer_init
 */
void ICACHE_FLASH_ATTR
user_smartconfig_led_timer_init(void) {
	os_timer_disarm(&g_smartconig_led_timer);
	os_timer_setfn(&g_smartconig_led_timer,
			(os_timer_func_t *) smartconfig_led_timer_cb, NULL);
	os_timer_arm(&g_smartconig_led_timer, 1000, 1);
}

/*
 * function: user_smartconfig_led_timer_stopֹ
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
	os_printf("[INFO] smartconfig start!\r\n");
	//user_smartconfig_led_timer_init();
	GPIO_HIGH(WIFI_STATUS_LED_PIN);
}

#if 1==WIFI_CHECK_MODE

/*
 * function: wifi_check_timer_cb
 */
static void ICACHE_FLASH_ATTR
wifi_check_timer_cb(void) {
	u8 status = wifi_station_get_connect_status();
	//os_printf("[INFO] wifi_connect_cb\r\n");

	// TODO:
	if (status == STATION_GOT_IP) {

	} else {

	}
}
#elif 2==WIFI_CHECK_MODE

/*
 * function: wifi_handle_event_cb
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
 * function: wifi_check_init״̬
 */
void ICACHE_FLASH_ATTR
wifi_check_init(void) {
#if 1==WIFI_CHECK_MODE
	os_timer_disarm(&g_wifi_check_timer);
	os_timer_setfn(&g_wifi_check_timer, (os_timer_func_t *) wifi_check_timer_cb,
			NULL);
	os_timer_arm(&g_wifi_check_timer, WiFi_CHECK_TIMER_INTERVAL, 1);
#elif 2==WIFI_CHECK_MODE
	wifi_set_event_handler_cb(wifi_handle_event_cb);
#else
#endif

}

/*
 * function: wifi_start_timer_cb
 */
static void ICACHE_FLASH_ATTR
wifi_start_timer_cb(void *arg) {
	static u8 s_smartconig_flag = 0;

	os_timer_disarm(&g_wifi_start_timer);
	wifi_get_ip_info(STATION_IF, &g_ipConfig);
	u8 wifi_status = wifi_station_get_connect_status();

	user_smartconfig_led_timer_stop();
	wifi_status_led_init();
	wifi_check_init();

	wifi_station_set_reconnect_policy(TRUE);	//
	wifi_station_set_auto_connect(TRUE);		// auto connect wifi

	if (STATION_GOT_IP == wifi_status && g_ipConfig.ip.addr != 0) {
		os_printf("[INFO] Wi-Fi Connected!\r\n");
	} else {
		if (0 == s_smartconig_flag) {
			os_printf("[INFO] smartconfig stop!\r\n");
			smartconfig_stop();
			s_smartconig_flag = 1;
		} else {
			os_printf("[ERROR] Wi-Fi Connect Fail!\r\n");
		}
		wifi_station_disconnect();
		wifi_station_connect();
	}
}

/*
 * function: wifi_connect
 */
void ICACHE_FLASH_ATTR
wifi_connect(void) {
	wifi_set_opmode(STATION_MODE);		// set wifi mode
	wifi_station_disconnect();

#ifdef SMARTCONFIG_ENABLE
	os_printf("SMARTCONFIG_ENABLE\r\n");
	wifi_station_set_reconnect_policy(FALSE);
	wifi_station_set_auto_connect(FALSE);

	user_smartconfig_init();

	os_timer_disarm(&g_wifi_start_timer);
	os_timer_setfn(&g_wifi_start_timer, (os_timer_func_t *) wifi_start_timer_cb,
			NULL);
	os_timer_arm(&g_wifi_start_timer, 20000, 0);

#if 2==WIFI_CHECK_MODE
	os_printf("WIFI_EVENT_ENABLE\r\n");
	wifi_set_event_handler_cb(wifi_handle_event_cb);
#endif	/* WIFI_EVENT_ENABLE */
#else
	wifi_station_set_reconnect_policy(TRUE);
	wifi_station_set_auto_connect(TRUE);
	wifi_station_connect();
	wifi_status_led_init();
	wifi_check_init();

#endif	/* SMARTCONFIG_DISABLE */
}

