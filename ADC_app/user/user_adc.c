/*
 * user_adc.c
 *
 *  Created on: 2017Äê7ÔÂ5ÈÕ
 *      Author: Administrator
 */


#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

#include "user_adc.h"

#define ADC_READ_INTERVAL	3000

static os_timer_t adc_timer;

void ICACHE_FLASH_ATTR
adc_timer_cb(void *arg)
{
	u16 adc = system_adc_read();

	os_printf("ADC: %d\r\n", adc);
}

void ICACHE_FLASH_ATTR
adc_timer_init(void)
{
    os_timer_disarm(&adc_timer);
    os_timer_setfn(&adc_timer, (os_timer_func_t *) adc_timer_cb, NULL);
    os_timer_arm(&adc_timer, ADC_READ_INTERVAL, 1);
}




