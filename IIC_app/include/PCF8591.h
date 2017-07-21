/*
 * PCF8951.h
 *
 *  Created on: 2017年7月21日
 *      Author: Administrator
 */

#ifndef _PCF8951_H_
#define _PCF8951_H_

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

#include "driver/i2c_master.h"

extern void pcf8591_init(void);
extern u8 user_read_adc(u8 channel);
extern void user_pcf8591_test_init(void);

#endif /* _PCF8951_H_ */
