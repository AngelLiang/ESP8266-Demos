/*
 * user_spi.h
 *
 *  Created on: 2017Äê7ÔÂ20ÈÕ
 *      Author: Administrator
 */

#ifndef _USER_SPI_H_
#define _USER_SPI_H_

#include "osapi.h"
#include "user_interface.h"
#include "driver/spi.h"
#include "driver/spi_interface.h"

//#define HARD_SPI
#define SOFT_SPI

#define MISO_PIN	12
#define MOSI_PIN	13
#define SCK_PIN		14
#define CS_PIN		15

#if defined(HARD_SPI)
	#define CS_0()
	#define CS_1()
#elif defined(SOFT_SPI)

	#define SOFT_PIN_INIT()	do{\
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);\
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);\
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);\
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15);\
		GPIO_DIS_OUTPUT(MISO_PIN);\
	}while(0)

	#define MOSI_0()	GPIO_OUTPUT_SET(MOSI_PIN, 0)
	#define MOSI_1()	GPIO_OUTPUT_SET(MOSI_PIN, 1)

	#define CS_0()		GPIO_OUTPUT_SET(CS_PIN, 0)
	#define CS_1()		GPIO_OUTPUT_SET(CS_PIN, 1)

	#define SCK_0()		GPIO_OUTPUT_SET(SCK_PIN, 0)
	#define SCK_1()		GPIO_OUTPUT_SET(SCK_PIN, 1)

	#define MISO_IS_HIGH()	(GPIO_INPUT_GET(MISO_PIN) != 0)

#endif

extern void user_spi_pin_init(void);
extern u8 user_spi_read_byte(void);
extern u16 user_spi_read_2byte(void);
extern u32 user_spi_read_4byte(void);
extern void user_spi_write_byte(u8);

#endif /* _USER_SPI_H_ */
