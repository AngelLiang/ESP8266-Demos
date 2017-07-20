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

#include "user_spi.h"
#include "oled.h"

/* 最好使用SOFT_SPI */


/* 引脚连接
 * NodeMCU - ESP8266 - Function - OLED
 * D5 - GPIO14 - HSCLK - CLK
 * D6 - GPIO12 - HMISO - NA
 * D7 - GPIO13 - HMOSI - MOSI
 * D8 - GPIO15 - HCS - CS1
 * D1 - GPIO5 - GPIO - DC
 * D2 - GPIO4 - GPIO - FSO
 * D4 - GPIO2 - GPIO - CS2
 */


/* OLED初始化数据 */
static u8 init_data[] = {
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
static void ICACHE_FLASH_ATTR
oled_pin_init(void)
{
	user_spi_pin_init();

	ROM_CS_PIN_INIT();
	DC_PIN_INIT();
	FSO_PIN_INIT();
}


/*
 * 函数：oled_write_byte
 * 说明：对OLED写一个字节
 */
static void ICACHE_FLASH_ATTR
oled_write_byte(u8 data)
{
	user_spi_write_byte(data);
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
	oled_write_cmd(0xb0 + column);			//设置页地址
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

void ICACHE_FLASH_ATTR
display_graphic_16x16(u16 page, u16 column, u8 *dp)
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

/*
 * 函数：send_command_to_ROM
 * 说明：送指令到晶联讯字库IC
 */
static void ICACHE_FLASH_ATTR
send_command_to_ROM(u8 data)
{
	oled_write_byte(data);
}

/*
 * 函数：get_data_from_ROM
 * 说明：从晶联讯字库IC获取汉字或字符数据（1字节）
 */
static u8 ICACHE_FLASH_ATTR
get_data_from_ROM(void)
{
	u8 ret_data = 0;

#if defined(SOFT_SPI)
	u8 i;
	SCK_1();
	for(i=0; i<8; i++){
		SCK_0();
		ret_data = ret_data<<1;
		if(FSO_IS_HIGH()){
			ret_data = ret_data + 1;
		}
		SCK_1();
	}
#else
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
#endif

	return ret_data;
}

static void ICACHE_FLASH_ATTR
get_n_bytes_data_from_ROM(u8 addrHigh, u8 addrMid, u8 addrLow,
		u8 *pBuff, u8 DataLen )
{
	u8 i;

	ROM_CS_0();
	send_command_to_ROM(0x03);
	send_command_to_ROM(addrHigh);
	send_command_to_ROM(addrMid);
	send_command_to_ROM(addrLow);
	for(i = 0; i < DataLen; i++ ){
	     *(pBuff+i) = get_data_from_ROM();
	}
	ROM_CS_1();
}



void ICACHE_FLASH_ATTR
display_string_5x7(u8 y, u8 x, u8 *text)
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

void ICACHE_FLASH_ATTR
display_GB2312_string(u8 y,u8 x,u8 *text)
{
	u32  fontaddr = 0;
	u8 i = 0;
	u8 addrHigh, addrMid, addrLow ;
	u8 fontbuf[32];

	while((text[i]>0x00)){
		if(((text[i]>=0xb0) &&(text[i]<=0xf7))&&(text[i+1]>=0xa1)){
			/*国标简体（GB2312）汉字在晶联讯字库IC中的地址由以下公式来计算：*/
			/*Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0*/
			/*由于担心8位单片机有乘法溢出问题，所以分三部取地址*/
			fontaddr = (text[i]- 0xb0)*94;
			fontaddr += (text[i+1]-0xa1)+846;
			fontaddr = (u32)(fontaddr*32);

			addrHigh = (fontaddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			addrMid = (fontaddr&0xff00)>>8;      /*地址的中8位,共24位*/
			addrLow = fontaddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,32 );/*取32个字节的数据，存到"fontbuf[32]"*/
			display_graphic_16x16(y,x,fontbuf);/*显示汉字到LCD上，y为页地址，x为列地址，fontbuf[]为数据*/
			i+=2;
			x+=16;
		}else if(((text[i]>=0xa1) &&(text[i]<=0xa3))&&(text[i+1]>=0xa1)){
			/*国标简体（GB2312）15x16点的字符在晶联讯字库IC中的地址由以下公式来计算：*/
			/*Address = ((MSB - 0xa1) * 94 + (LSB - 0xA1))*32+ BaseAdd;BaseAdd=0*/
			/*由于担心8位单片机有乘法溢出问题，所以分三部取地址*/
			fontaddr = (text[i]- 0xa1)*94;
			fontaddr += (text[i+1]-0xa1);
			fontaddr = (u32)(fontaddr*32);

			addrHigh = (fontaddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			addrMid = (fontaddr&0xff00)>>8;      /*地址的中8位,共24位*/
			addrLow = fontaddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,32 );/*取32个字节的数据，存到"fontbuf[32]"*/
			display_graphic_16x16(y,x,fontbuf);/*显示汉字到LCD上，y为页地址，x为列地址，fontbuf[]为数据*/
			i+=2;
			x+=16;
		}else if((text[i]>=0x20) &&(text[i]<=0x7e)){
			u8 fontbuf[16];
			fontaddr = (text[i]- 0x20);
			fontaddr = (u32)(fontaddr*16);
			fontaddr = (u32)(fontaddr+0x3cf80);
			addrHigh = (fontaddr&0xff0000)>>16;
			addrMid = (fontaddr&0xff00)>>8;
			addrLow = fontaddr&0xff;

			/*取16个字节的数据，存到"fontbuf[32]"*/
			get_n_bytes_data_from_ROM(addrHigh, addrMid, addrLow, fontbuf, 16 );
			/*显示8x16的ASCII字到LCD上，y为页地址，x为列地址，fontbuf[]为数据*/
			display_graphic_8x16(y, x, fontbuf);

			i+=1;
			x+=8;
		}else{
			i++;
		}
	}
}



/*
 * 函数：oled_init
 * 说明：OLED总体初始化
 */
void ICACHE_FLASH_ATTR
oled_init(void)
{
	u8 i;
	oled_pin_init();

	u8 len = sizeof(init_data);

	// 初始化
	for (i=0; i<len; i++){
		oled_write_cmd(init_data[i]);
	}

	oled_clear_screen();
	os_printf("oled_init\r\n");
}

/*******************************************************************************/

u8 jiong1[]={
0x00,0xFE,0x82,0x42,0xA2,0x9E,0x8A,0x82,0x86,0x8A,0xB2,0x62,0x02,0xFE,0x00,0x00,
0x00,0x7F,0x40,0x40,0x7F,0x40,0x40,0x40,0x40,0x40,0x7F,0x40,0x40,0x7F,0x00,0x00
};

/*
 * 函数：oled_test_init
 * 说明：测试例程
 */
void ICACHE_FLASH_ATTR
oled_test_init(void)
{
	u8 i;
	oled_init();

	//display_graphic_16x16(0, 0, jiong1);
	display_string_5x7(1, 0, "Hello World!");

	// 「测试」的GB2312编码
	u8 ceshi[] = {0xB2, 0xE0+2, 0xCA, 0xD0+4};
	display_GB2312_string(2, 0, ceshi);
}

