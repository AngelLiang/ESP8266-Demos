/*
 * user_wifi.c
 *
 *  Created on: 2017年7月4日
 *      Author: Administrator
 */

#include "driver/uart.h"
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

#include "tcp_client.h"


#define WIFI_SSID 		"your ssid"
#define WIFI_PASSWORD	"your password"

#define HOST	"192.168.10.70"
#define PORT	2000

/*
 * 函数：user_set_station_config
 * 参数：u8* ssid - WiFi的名称
 * 	   u8 password - WiFi的密码
 */
void ICACHE_FLASH_ATTR
user_set_station_config(u8* ssid, u8* password)
{
	 //u8	ssid[32] = "your ssid";
	 //u8	password[64] = "your password";
	 struct	station_config stationConf;
	 stationConf.bssid_set = 0;		//need	not	check	MAC	address	of	AP
	 os_memcpy(&stationConf.ssid, ssid, 32);
	 os_memcpy(&stationConf.password, password, 64);
	 wifi_station_set_config(&stationConf);
}

/*
 * 函数：wifi_handle_event_cb
 * 参数：System_Event_t *evt
 * 说明：WiFi事件回调
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
        // WiFi连接后并获取IP后建立TCP连接
        tcp_client_init(HOST, PORT);


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

/*
 * 函数：user_wifi_init
 * 说明：WiFi配置初始化
 */
void ICACHE_FLASH_ATTR
user_wifi_init(void)
{
	wifi_set_opmode(STATION_MODE);
	user_set_station_config(WIFI_SSID, WIFI_PASSWORD);
    wifi_set_event_handler_cb(wifi_handle_event_cb);
}
