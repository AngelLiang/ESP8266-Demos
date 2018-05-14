/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define AP_CACHE           0

#if AP_CACHE
#define AP_CACHE_NUMBER    5
#endif

// 上电使用 smartconfig 进行 wifi 配置
#define SMARTCONFIG_ENABLE

// wifi检查连接方式
// 0 - DISABLE：不检查wifi连接
// 1 - timer mode：定时检查wifi连接
// 2 - event mode：注册事件的方式检查wifi连接
#define WIFI_CHECK_MODE		1

// 是否使用定时器控制wifi状态led灯
// 0 - DISABLE：不使用wifi led状态灯
// 1 - timer mode：使用定时器设置wifi led状态灯
// 2 - esp mode ：使用ESP自带的接口配置wifi led状态的
#define WIFI_LED_MODE	1

// wifi led 引脚配置
#if 1==WIFI_LED_MODE
#define WIFI_STATUS_LED_PIN			0
#elif 2==WIFI_LED_MODE
#define HUMITURE_WIFI_LED_IO_MUX 	PERIPHS_IO_MUX_GPIO0_U
#define HUMITURE_WIFI_LED_IO_NUM 	0
#define HUMITURE_WIFI_LED_IO_FUNC 	FUNC_GPIO0
#else
#endif

#endif

