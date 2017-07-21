/*
 * PCF8591.c
 *
 *  Created on: 2017年7月21日
 *      Author: Administrator
 */


#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

#include "driver/i2c_master.h"

#include "PCF8591.h"

//PCF8591 地址
#define PCF8591_ADDR	0x90

/*
 * function: pcf8591_init
 * description: init
 */
void ICACHE_FLASH_ATTR
pcf8591_init(void)
{
	i2c_master_gpio_init();
	i2c_master_init();
}

/*
 * function: pcf8591_send_byte
 */
u8 ICACHE_FLASH_ATTR
pcf8591_send_byte(u8 sla, u8 c)
{
	i2c_master_start();
	i2c_master_writeByte(sla);
	if(0==i2c_master_checkAck()){
		return 0;
	}
	i2c_master_writeByte(c);
	if(0==i2c_master_checkAck()){
		return 0;
	}
	i2c_master_stop();

   return 1;
}

/*
 * function: pcf8591_recv_byte
 */
u8 ICACHE_FLASH_ATTR
pcf8591_recv_byte(u8 sla)
{
	u8 c;

	i2c_master_start();
	i2c_master_writeByte(sla+1);
	if(0==i2c_master_checkAck()){
		return 0;
	}
	c = i2c_master_readByte();
	i2c_master_send_nack();
	i2c_master_stop();

	return c;
}

/*
 * function: DAC_conversion
 * description: DAC变换
 */
u8 ICACHE_FLASH_ATTR
DAC_conversion(u8 sla, u8 c, u8 Val)
{
	i2c_master_start();
	i2c_master_writeByte(sla);
	if(0==i2c_master_checkAck()){
		return 0;
	}
	i2c_master_writeByte(c);
	if(0==i2c_master_checkAck()){
		return 0;
	}
	i2c_master_writeByte(Val);
	if(0==i2c_master_checkAck()){
		return 0;
	}
	i2c_master_stop();
	return 1;
}

/********************************************************************/


u8 ICACHE_FLASH_ATTR
user_read_adc(u8 channel)
{
	u8 ret = 0;
	static u8 adc3 = 0;

    switch(channel)
	{
    // ADC：光敏电阻
	case 0:
		pcf8591_send_byte(PCF8591_ADDR, 0x41);
		ret = pcf8591_recv_byte(PCF8591_ADDR);
	break;
	// ADC：热敏电阻
	case 1:
		pcf8591_send_byte(PCF8591_ADDR, 0x42);
		ret = pcf8591_recv_byte(PCF8591_ADDR);
	break;
	// ADC：悬空
	case 2:
		pcf8591_send_byte(PCF8591_ADDR, 0x43);
		ret = pcf8591_recv_byte(PCF8591_ADDR);
	break;
	// ADC：可调0~5V
	case 3:
		pcf8591_send_byte(PCF8591_ADDR, 0x40);
		ret = pcf8591_recv_byte(PCF8591_ADDR);
		adc3 = ret;
	break;
	// DAC：数模转换
	case 4:
		DAC_conversion(PCF8591_ADDR, 0x40, adc3);
	break;
	default:
		break;
	}

    return ret;
}

static void ICACHE_FLASH_ATTR
timer_cb(void *arg)
{
	u8 i;
	u8 data[5] = {0};
	for (i=0; i<5; i++){
		data[i] = user_read_adc(i);
	}

	os_printf("adc0=%d\t"
			"adc1=%d\t"
			"adc2=%d\t"
			"adc3=%d\r\n",
			data[0], data[1], data[2], data[3]);
}

void ICACHE_FLASH_ATTR
user_pcf8591_test_init(void)
{
	static os_timer_t s_timer;

	pcf8591_init();

    os_timer_disarm(&s_timer);
    os_timer_setfn(&s_timer, timer_cb , NULL);
    os_timer_arm(&s_timer, (1*1000), 1);
}
