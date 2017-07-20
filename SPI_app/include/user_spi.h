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


extern void user_spi_pin_init(void);
extern u8 user_spi_read_byte(void);
extern u16 user_spi_read_2byte(void);
extern u32 user_spi_read_4byte(void);
extern void user_spi_write_byte(u8);

#endif /* _USER_SPI_H_ */
