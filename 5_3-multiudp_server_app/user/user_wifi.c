/*
 * user_wifi.c
 *
 *  Created on: 2017��7��4��
 *      Author: Administrator
 */

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

#include "driver/uart.h"
#include "udp_server.h"


#define WIFI_SSID 		"your ssid"
#define WIFI_PASSWORD	"your password"


/*
 * function: got_ip_cb
 * parameter: void
 * description: station��ȡ��ip�Ļص�����
 */
void ICACHE_FLASH_ATTR
got_ip_cb(void)
{
	// TODO:
	multiudp_init();
}


/*
 * function: user_set_station_config
 * parameter: u8* ssid - Wi-Fi������
 *            u8 password - Wi-Fi������
 */
void ICACHE_FLASH_ATTR
user_set_station_config(u8* ssid, u8* password)
{
	 static struct	station_config stationConf;
	 stationConf.bssid_set = 0;		//need not check MAC address of AP
	 os_memcpy(&stationConf.ssid, ssid, 32);
	 os_memcpy(&stationConf.password, password, 64);
	 wifi_station_set_config(&stationConf);
}

/*
 * function: wifi_handle_event_cb
 * parameter: System_Event_t *evt
 * description: WiFi�¼��ص�
 */
void ICACHE_FLASH_ATTR
wifi_handle_event_cb(System_Event_t *evt)
{
  //os_printf("event %x\n", evt->event);

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

        got_ip_cb();

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
 * function: user_wifi_init
 * description: Wi-Fi���ó�ʼ��
 */
void ICACHE_FLASH_ATTR
user_wifi_init(void)
{
	wifi_set_opmode(STATIONAP_MODE);
	user_set_station_config(WIFI_SSID, WIFI_PASSWORD);
	wifi_station_connect();
    wifi_set_event_handler_cb(wifi_handle_event_cb);
}
