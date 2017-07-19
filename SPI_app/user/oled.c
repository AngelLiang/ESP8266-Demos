/*
 * oled.c
 *
 *  Created on: 2017年7月19日
 *      Author: Administrator
 */

#include "eagle_soc.h"
#include "osapi.h"
#include "user_interface.h"

#include "driver/spi.h"
#include "driver/spi_interface.h"

/* 引脚连接
 * NodeMCU - ESP8266 - Function - OLED
 * D5 - GPIO14 - HSCLK - CLK
 * D6 - GPIO12 - HMISO - NA
 * D7 - GPIO13 - HMOSI - MOSI
 * D8 - GPIO15 - HCS - CS1
 * D1 - GPIO5 - GPIO - DC
 */

#define ROM_CS_PIN		4
#define OLED_DC_PIN		5

#define ROM_CS_1()	GPIO_OUTPUT_SET(ROM_CS_PIN, 1)
#define ROM_CS_0()	GPIO_OUTPUT_SET(ROM_CS_PIN, 0)

#define DC_1()		GPIO_OUTPUT_SET(OLED_DC_PIN, 1)
#define DC_0()		GPIO_OUTPUT_SET(OLED_DC_PIN, 0)

u8 init_data[] = {
	0xAE, 0x20, 0x10, 0xb0,
	0xc8, 0x00, 0x10, 0x40,
	0x81, 0x7F, 0xA1, 0xA6,
	0xA8, 0x3F, 0xA4, 0xD3,
	0x00, 0xD5, 0xF0, 0xD9,
	0x22, 0xDA, 0x12, 0xDB,
	0x20, 0x8D, 0x14, 0xAF,
};

/*
 * 函数：oled_pin_init
 * 说明：相关引脚初始化
 */
void ICACHE_FLASH_ATTR
oled_pin_init(void)
{

    SpiAttr hSpiAttr;
    hSpiAttr.bitOrder = SpiBitOrder_MSBFirst;
    hSpiAttr.speed = SpiSpeed_0_5MHz;
    hSpiAttr.mode = SpiMode_Master;
    hSpiAttr.subMode = SpiSubMode_0;

    // Init HSPI GPIO
    WRITE_PERI_REG(PERIPHS_IO_MUX, 0x105);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 2);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 2);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 2);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2);

    SPIInit(SpiNum_HSPI, &hSpiAttr);

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);

}

/*
 * 函数：oled_write_byte
 * 说明：对OLED写一个字节
 */
static void ICACHE_FLASH_ATTR
oled_write_byte(u8 data)
{
	SpiData spiData;
	u32 send_data[1] = {data};

    spiData.cmd = MASTER_WRITE_DATA_TO_SLAVE_CMD;
    spiData.cmdLen = 0;
    spiData.addr = NULL;
    spiData.addrLen = 0;
    spiData.data = send_data;
    spiData.dataLen = 1;
    SPIMasterSendData(SpiNum_HSPI, &spiData);
}

/*
 * 函数：oled_write_cmd
 * 说明：对OLED写指令
 */
static void ICACHE_FLASH_ATTR
oled_write_cmd(u8 data)
{
	DC_0();
	oled_write_byte(data);
}

/*
 * 函数：olcd_wirte_data
 * 说明：对OLED写数据
 */
static void ICACHE_FLASH_ATTR
oled_wirte_data(u8 data)
{
	DC_1();
	oled_write_byte(data);
}

/*
 * 函数：oled_clear_screen
 * 说明：清屏
 */
void ICACHE_FLASH_ATTR
oled_clear_screen(void)
{
	unsigned char i, j;
	for(i=0; i<8; i++)
	{
		oled_write_cmd(0xb0+i);
		oled_write_cmd(0x00);
		oled_write_cmd(0x10);

		for(j=0; j<128; j++){
			oled_wirte_data(0x00);
		}
	}
}

void ICACHE_FLASH_ATTR
lcd_address(u16 page, u16 column)
{
	oled_write_cmd(0xb0 + column);	//设置页地址
	oled_write_cmd(((page & 0xf0) >> 4) | 0x10);	// 设置列地址低4位
	oled_write_cmd((page & 0x0f) | 0x00);	// 设置列地址高4位
}

/*
 * 函数：display_graphic_8x16
 * 说明：显示8x16点阵图像、ASCII
 */
void ICACHE_FLASH_ATTR
display_graphic_8x16(u16 page, u16 column, u8 *dp)
{
	u8 i, j;

	for(j=2; j>0; j--){
		lcd_address(column, page);
		for (i=0; i<8; i++){
			oled_wirte_data(*dp++);
		}
		page++;
	}
}

void ICACHE_FLASH_ATTR
display_graphic_5x7(u16 page, u16 column, u8 *dp)
{
	u16 col_cnt;
	u8 page_address;
	u8 column_address_L, column_address_H;
	page_address = 0xb0+page-1;

	column_address_L =(column&0x0f)-1;
	column_address_H =((column>>4)&0x0f)+0x10;

	oled_write_cmd(page_address); 		/*Set Page Address*/
	oled_write_cmd(column_address_H);	/*Set MSB of column Address*/
	oled_write_cmd(column_address_L);	/*Set LSB of column Address*/

	for (col_cnt=0; col_cnt<6; col_cnt++){
		oled_wirte_data(*dp++);
	}
}

void display_graphic_16x16(u16 page, u16 column, u8 *dp)
{
	u8 i,j;
 	ROM_CS_1();
	for(j=2; j>0; j--){
		lcd_address(column, page);
		for (i=0; i<16; i++){
			oled_wirte_data(*dp++);
		}
		page++;
	}
}

static u8 ICACHE_FLASH_ATTR
get_data_from_ROM(void)
{
	u8 ret_data=0;
	SpiData spiData;
	u32 recv_data[1] = {0};

    spiData.cmd = MASTER_READ_DATA_FROM_SLAVE_CMD;
    spiData.cmdLen = 0;
    spiData.addr = NULL;
    spiData.addrLen = 0;
    spiData.data = recv_data;
    spiData.dataLen = 1;
    SPIMasterRecvData(SpiNum_HSPI, &spiData);
    ret_data = (u8)(recv_data[0] && 0xFF);
	return(ret_data);
}

void ICACHE_FLASH_ATTR
get_n_bytes_data_from_ROM(u8 addrHigh, u8 addrMid, u8 addrLow,
		u8 *pBuff, u8 DataLen )
{
	u8 i;

	ROM_CS_0();
	oled_write_cmd(0x03);
	oled_write_cmd(addrHigh);
	oled_write_cmd(addrMid);
	oled_write_cmd(addrLow);
	for(i = 0; i < DataLen; i++ ){
	     *(pBuff+i) = get_data_from_ROM();
	}
	ROM_CS_1();
}



void display_string_5x7(u8 y, u8 x, u8 *text)
{
	u32 fontaddr=0;
	u8 i= 0;
	u8 addrHigh, addrMid, addrLow ;
	while((text[i]>0x00)){
		if((text[i]>=0x20) &&(text[i]<=0x7e)){
			u8 fontbuf[8];
			fontaddr = (text[i]- 0x20);
			fontaddr = (u32)(fontaddr*8);
			fontaddr = (u32)(fontaddr+0x3bfc0);
			addrHigh = (fontaddr&0xff0000)>>16;
			addrMid = (fontaddr&0xff00)>>8;
			addrLow = fontaddr&0xff;

			get_n_bytes_data_from_ROM(addrHigh, addrMid, addrLow, fontbuf, 8);
			display_graphic_5x7(y, x, fontbuf);

			i+=1;
			x+=6;
		}else{
			i++;
		}
	}

}


u8 jiong1[]={
0x00,0xFE,0x82,0x42,0xA2,0x9E,0x8A,0x82,0x86,0x8A,0xB2,0x62,0x02,0xFE,0x00,0x00,
0x00,0x7F,0x40,0x40,0x7F,0x40,0x40,0x40,0x40,0x40,0x7F,0x40,0x40,0x7F,0x00,0x00
};

void ICACHE_FLASH_ATTR
oled_test_init(void)
{
	u8 i;
	oled_pin_init();

	u8 len = sizeof(init_data);

	// 初始化
	for (i=0; i<len; i++){
		oled_write_cmd(init_data[i]);
	}

	oled_clear_screen();
	os_printf("oled_test_init\r\n");

	display_graphic_16x16(0, 0, jiong1);
	//display_string_5x7(0, 0, "Hello World!");
}

