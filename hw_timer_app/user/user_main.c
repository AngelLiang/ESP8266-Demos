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

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

#include "driver/uart.h"
#include "user_hw_timer.h"

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
 *******************************************************************************/
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void) {
	enum flash_size_map size_map = system_get_flash_size_map();
	uint32 rf_cal_sec = 0;

	switch (size_map) {
	case FLASH_SIZE_4M_MAP_256_256:
		rf_cal_sec = 128 - 5;
		break;

	case FLASH_SIZE_8M_MAP_512_512:
		rf_cal_sec = 256 - 5;
		break;

	case FLASH_SIZE_16M_MAP_512_512:
	case FLASH_SIZE_16M_MAP_1024_1024:
		rf_cal_sec = 512 - 5;
		break;

	case FLASH_SIZE_32M_MAP_512_512:
	case FLASH_SIZE_32M_MAP_1024_1024:
		rf_cal_sec = 1024 - 5;
		break;

	default:
		rf_cal_sec = 0;
		break;
	}

	return rf_cal_sec;
}

void ICACHE_FLASH_ATTR
user_rf_pre_init(void) {
}

void ICACHE_FLASH_ATTR
print_chip_info(void) {
	// 保存芯片MAC地址
	u8 macAddr[6] = { 0 };
	os_printf("\r\n*********************************\r\n");
	// SDK版本信息
	os_printf("SDK version:%s\r\n", system_get_sdk_version());
	// 编译时间
	os_printf("Build Datetime:%s %s\r\n", __DATE__, __TIME__);
	// 芯片序列号
	os_printf("chip ID:%d\r\n", system_get_chip_id());
	// CPU频率
	os_printf("CPU freq:%d\r\n", system_get_cpu_freq());
	// 空闲的堆空间
	os_printf("free heap size:%d\r\n", system_get_free_heap_size());
	// MAC地址
	if (wifi_get_macaddr(STATION_IF, macAddr)) {	// 在init_done_cb_init函数调用才正常
		os_printf("MAC:"MACSTR"\r\n", MAC2STR(macAddr));
	} else {
		os_printf("MAC: null\r\n");
	}

	// boot 版本信息
	os_printf("boot version:%d\r\n", system_get_boot_version());
	// 查询 boot 模式
	//=0 boot 增强模式：支持跳转到任意位置运行程序；
	//=1 boot 普通模式：仅能跳转到固定的 user1.bin（或 user2.bin）位置运行
	os_printf("boot mode: %d\r\n", system_get_boot_mode());
	// 在线升级用，判断是user1.bin还是user2.bin
	os_printf("user bin:%d\r\n", system_upgrade_userbin_check());
	// 当前正在运⾏的 user bin（user1.bin 或者 user2.bin） 的存放地址
	os_printf("user bin addr: 0x%x\r\n", system_get_userbin_addr());

	// 内存信息
	os_printf("meminfo:\r\n");
	system_print_meminfo();

	// 当前的 Flash size 和 Flash map
	os_printf("flash size map: 0x%x\r\n", system_get_flash_size_map());

	os_printf("*********************************\r\n\r\n");
}

void ICACHE_FLASH_ATTR
init_done_cb_init(void) {
	print_chip_info();
	user_hw_timer_init();
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
 *******************************************************************************/
void ICACHE_FLASH_ATTR
user_init(void) {
	//uart_init(BIT_RATE_115200, BIT_RATE_115200);
	os_printf("SDK version:%s\n", system_get_sdk_version());
	system_init_done_cb(init_done_cb_init);
}

