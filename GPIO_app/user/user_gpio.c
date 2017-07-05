/*
 * user_gpio.c
 *
 *  Created on: 2017年7月5日
 *      Author: Administrator
 */

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

void user_gpio_interrupt(void *arg)
{
	u32 gpio_status;
	gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	//clear interrupt status
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);

	u8 gpio2 = GPIO_INPUT_GET(2);
	if (0==gpio2){
		os_delay_us(20*1000);		// 延时20ms，去抖动
		if (0==gpio2){
			os_printf("GPIO 2: %d\r\n", gpio2);
		}
	}


}

void ICACHE_FLASH_ATTR
user_gpio_interrupt_init(void)
{

#if 0
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);

	//PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
	GPIO_OUTPUT_SET(0, 1);
	GPIO_OUTPUT_SET(2, 0);
#endif

	// 管脚功能选择
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	// 设置管脚为输入
	GPIO_DIS_OUTPUT(2);
	/*
	 * GPIO_PIN_INTR_DISABLE - 禁能
	 * GPIO_PIN_INTR_POSEDGE - 上升沿
	 * GPIO_PIN_INTR_NEGEDGE - 下降沿
	 * GPIO_PIN_INTR_ANYEDGE - 双边沿
	 * GPIO_PIN_INTR_LOLEVEL - 低电平
	 * GPIO_PIN_INTR_HILEVEL - 高电平
	 */

	gpio_pin_intr_state_set(GPIO_ID_PIN(2), GPIO_PIN_INTR_NEGEDGE);

	// 注册 GPIO 中断处理函数
	ETS_GPIO_INTR_ATTACH(user_gpio_interrupt, NULL);

	// 关 GPIO 中断
	//ETS_GPIO_INTR_DISABLE();

	// 开 GPIO 中断
	ETS_GPIO_INTR_ENABLE();

	os_printf("user_gpio_interrupt_init\r\n");
}

#define GPIO_SET_INTERVAL	1000

void ICACHE_FLASH_ATTR
gpio_timer_cb(void *arg)
{
	u8 gpio0 = GPIO_INPUT_GET(0);
	GPIO_OUTPUT_SET(0, 1-gpio0);
	os_printf("GPIO 0: %d\r\n", gpio0);
}

/*
 * 函数：user_gpio_timer_init
 * 说明：定时改变GPIO的状态
 */
void ICACHE_FLASH_ATTR
user_gpio_timer_init(void)
{
	static volatile os_timer_t gpio_timer;

	// 管脚功能选择
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);

    os_timer_disarm(&gpio_timer);
    os_timer_setfn(&gpio_timer, (os_timer_func_t *)gpio_timer_cb, NULL);
    os_timer_arm(&gpio_timer, GPIO_SET_INTERVAL, 1);

    os_printf("user_gpio_timer_init\r\n");
}

