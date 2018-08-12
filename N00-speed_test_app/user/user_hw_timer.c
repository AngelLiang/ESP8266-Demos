/*
 * user_hw_timer.c
 */

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "mem.h"
#include "espconn.h"
#include "driver/hw_timer.h"
#include "user_config.h"

extern u32 tcp_recv_byte_count;
extern u32 udp_recv_byte_count;

void user_hw_timer_cb(void) {
	// TODO:
#ifdef TCP_SEPPD_TEST
	os_printf("TCP recv speed: %d Byte/s = %d KB/s", tcp_recv_byte_count,
			tcp_recv_byte_count / 1024);
	tcp_recv_byte_count = 0;
#endif

#ifdef UDP_SEPPD_TEST
	os_printf("\tUDP recv speed: %d Byte/s = %d KB/s", udp_recv_byte_count,
			udp_recv_byte_count / 1024);
	udp_recv_byte_count = 0;
#endif
	os_printf("\r\n");

}

// 这个函数不需要改动
// HW timer
void hw_test_timer_cb(void) {
	static uint32 tick_now2 = 0;
	static uint16 j = 0;
	j++;

	if ((WDEV_NOW() - tick_now2) >= 1000000) {
		static u32 idx = 1;
		tick_now2 = WDEV_NOW();
		//os_printf("b%u:%d\n", idx++, j);
		j = 0;
		user_hw_timer_cb();
	}
}

void ICACHE_FLASH_ATTR
user_hw_timer_init(void) {
	hw_timer_init(FRC1_SOURCE, 1);
	hw_timer_set_func(hw_test_timer_cb);
	hw_timer_arm(1000);
}

void ICACHE_FLASH_ATTR
user_hw_timer_delete(void) {
	hw_timer_set_func(NULL);
}
