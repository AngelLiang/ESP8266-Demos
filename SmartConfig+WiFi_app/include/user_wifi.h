/*
 * wifi.h
 *
 *  Created on: Dec 30, 2014
 *      Author: Minh
 */

#ifndef _USER_WIFI_H_
#define _USER_WIFI_H_

#include "os_type.h"

#define WiFi_CHECK_TIMER_ENABLE			1
#define WiFi_LED_STATUS_TIMER_ENABLE	1

extern void wifi_connect(void);

extern void wifi_status_led_init(void);
extern void user_smartconfig_led_timer_init(void);
extern void user_smartconfig_led_timer_stop(void);
extern u32 get_station_ip(void);

#endif /* _USER_WIFI_H_ */
