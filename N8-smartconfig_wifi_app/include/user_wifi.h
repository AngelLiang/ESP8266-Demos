/*
 * user_wifi.h
 */

#ifndef _USER_WIFI_H_
#define _USER_WIFI_H_

#include "os_type.h"

extern void wifi_connect(void);

extern void wifi_check_init(void);
extern void wifi_status_led_init(void);

extern void user_smartconfig_led_timer_init(void);
extern void user_smartconfig_led_timer_stop(void);

extern u32 get_station_ip(void);

#endif /* _USER_WIFI_H_ */
