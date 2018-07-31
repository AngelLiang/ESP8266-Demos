/*
 * user_wifi.h
 */

#ifndef _USER_WIFI_H_
#define _USER_WIFI_H_

#include "os_type.h"
#include "wifi_config.h"

typedef void (*WifiCallback)(uint8_t);

extern void wifi_connect(WifiCallback cb);
extern void smartconfig_connect(WifiCallback cb);

extern void wifi_check_init(u16);
extern void wifi_status_led_init(void);

extern void user_smartconfig_led_timer_init(void);
extern void user_smartconfig_led_timer_stop(void);

extern void wifi_smartconfig_timer_init(void);
extern void wifi_smartconfig_timer_stop(void);

extern u32 get_station_ip(void);

#endif /* _USER_WIFI_H_ */
