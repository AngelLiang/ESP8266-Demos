/*
 * oled.h
 *
 *  Created on: 2017Äê7ÔÂ19ÈÕ
 *      Author: Administrator
 */

#ifndef _OLED_H_
#define _OLED_H_

#include "eagle_soc.h"
#include "osapi.h"
#include "user_interface.h"

#include "driver/spi.h"
#include "driver/spi_interface.h"
#include "user_spi.h"

#if !defined(SOFT_SPI)
	#error "It must define SOFT_SPI!"
#endif

#define OLED_DC_PIN		5

#define OLED_FSO_PIN	4
#define OLED_CS2_PIN	2

/* ROM_CS_PIN */
#define ROM_CS_PIN_INIT()	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2)
#define ROM_CS_1()	GPIO_OUTPUT_SET(OLED_CS2_PIN, 1)
#define ROM_CS_0()	GPIO_OUTPUT_SET(OLED_CS2_PIN, 0)

/* DC_PIN */
#define DC_PIN_INIT()	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5)
#define DC_1()		GPIO_OUTPUT_SET(OLED_DC_PIN, 1)
#define DC_0()		GPIO_OUTPUT_SET(OLED_DC_PIN, 0)

/* FSO_PIN */
#define FSO_PIN_INIT()	do{\
PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);\
	GPIO_DIS_OUTPUT(OLED_FSO_PIN);\
}while(0)

#define FSO_IS_HIGH()	(GPIO_INPUT_GET(OLED_FSO_PIN) == 1)

//#define FSO_1()		GPIO_OUTPUT_SET(OLED_FSO_PIN, 1)
//#define FSO_0()		GPIO_OUTPUT_SET(OLED_FSO_PIN, 0)


extern void oled_init(void);
extern void oled_clear_screen(void);
extern void display_string_5x7(u8 y, u8 x, u8 *text);
extern void display_GB2312_string(u8 y,u8 x,u8 *text);

extern void oled_test_init(void);

#endif /* _OLED_H_ */
