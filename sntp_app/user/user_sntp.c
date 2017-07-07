/*
 * user_sntp.c
 *
 *  Created on: 2017年7月7日
 *      Author: Administrator
 */

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

#define SNTP_READ_INTERVAL	5000

static void ICACHE_FLASH_ATTR
sntp_timer_cb(void *arg)
{
    uint32_t time = sntp_get_current_timestamp();
    os_printf("time:%d\t - %s",
    		time, sntp_get_real_time(time));
}

void ICACHE_FLASH_ATTR
user_sntp_init(void)
{
	static os_timer_t user_sntp_timer;

	// 配置sntp服务器
    sntp_setservername(0,"0.cn.pool.ntp.org");
    sntp_setservername(1,"1.cn.pool.ntp.org");
    sntp_setservername(2,"2.cn.pool.ntp.org");
    // sntp初始化
    sntp_init();

    // 定时器初始化
    os_timer_disarm(&user_sntp_timer);
    os_timer_setfn(&user_sntp_timer, sntp_timer_cb , NULL);
    os_timer_arm(&user_sntp_timer, SNTP_READ_INTERVAL, 1);
}
