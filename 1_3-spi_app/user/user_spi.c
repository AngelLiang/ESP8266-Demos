/*
 * user_spi.c
 *
 *  Created on: 2017年7月20日
 *      Author: Administrator
 */


#include "eagle_soc.h"
#include "osapi.h"
#include "user_interface.h"
#include "driver/spi.h"
#include "driver/spi_interface.h"

#include "user_spi.h"


void ICACHE_FLASH_ATTR
user_spi_delay_us(u16 us)
{
	os_delay_us(us);
}


/*
 * 函数：user_spi_pin_init
 * 说明：SPI引脚初始化
 */
void ICACHE_FLASH_ATTR
user_spi_pin_init(void)
{

#if defined(HARD_SPI)
    SpiAttr hSpiAttr;
    hSpiAttr.bitOrder = SpiBitOrder_MSBFirst;
    /*
     * SpiSpeed_0_5MHz     = 160,
     * SpiSpeed_1MHz       = 80,
     * SpiSpeed_2MHz       = 40,
     * SpiSpeed_5MHz       = 16,
     * SpiSpeed_8MHz       = 10,
     * SpiSpeed_10MHz      = 8,
     */
    hSpiAttr.speed = SpiSpeed_0_5MHz;
    hSpiAttr.mode = SpiMode_Master;
    hSpiAttr.subMode = SpiSubMode_0;

    // Init HSPI GPIO
    WRITE_PERI_REG(PERIPHS_IO_MUX, 0x105);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 2);//configure io to spi mode
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 2);//configure io to spi mode
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 2);//configure io to spi mode
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2);//configure io to spi mode

    SPIInit(SpiNum_HSPI, &hSpiAttr);

#elif defined(SOFT_SPI)
    SOFT_PIN_INIT();
#else
	#error #error "Please define SPI Interface mode : SOFT_SPI or HARD_SPI"
#endif

}

/*
 * 函数：user_spi_read_byte
 * 说明：SPI读取一个字节
 */
u8 ICACHE_FLASH_ATTR
user_spi_read_byte(void)
{
	u8 read = 0;

#if defined(HARD_SPI)
	u32 recv_data[1];
	SpiData spiData;
    spiData.cmd = MASTER_READ_DATA_FROM_SLAVE_CMD;
    spiData.cmdLen = 0;
    spiData.addr = NULL;
    spiData.addrLen = 0;
    spiData.data = recv_data;
    spiData.dataLen = 1;
    SPIMasterRecvData(SpiNum_HSPI, &spiData);
    read = (u8)(recv_data[0]&0xFF);

#elif defined(SOFT_SPI)
    u8 i;
    CS_0();
	for (i = 0; i < 8; i++){
		SCK_0();
		//user_spi_delay_us(50);
		read = read<<1;
		if (MISO_IS_HIGH()){
			read++;
		}
		SCK_1();
		//user_spi_delay_us(50);
	}
	CS_1();
#endif

	return read;
}

/*
 * 函数：user_spi_read_2byte
 * 说明：SPI读取2个字节
 */
u16 ICACHE_FLASH_ATTR
user_spi_read_2byte(void)
{
	u16 read;

#if defined(HARD_SPI)
	u32 recv_data[1];
	SpiData spiData;
    spiData.cmd = MASTER_READ_DATA_FROM_SLAVE_CMD;
    spiData.cmdLen = 0;
    spiData.addr = NULL;
    spiData.addrLen = 0;
    spiData.data = recv_data;
    spiData.dataLen = 2;
    SPIMasterRecvData(SpiNum_HSPI, &spiData);
    read = (u16)(recv_data[0] && 0xFFFF);

#elif defined(SOFT_SPI)
    CS_0();
	read = user_spi_read_byte();
	read <<= 8;
	read += user_spi_read_byte();
	CS_1();
#endif

	return read;
}

/*
 * 函数：user_spi_read_4byte
 * 说明：SPI读取4个字节
 */
u32 ICACHE_FLASH_ATTR
user_spi_read_4byte(void)
{
	u32 read;

#if defined(HARD_SPI)
	u32 recv_data[1];
	SpiData spiData;
    spiData.cmd = MASTER_READ_DATA_FROM_SLAVE_CMD;
    spiData.cmdLen = 0;
    spiData.addr = NULL;
    spiData.addrLen = 0;
    spiData.data = recv_data;
    spiData.dataLen = 4;
    SPIMasterRecvData(SpiNum_HSPI, &spiData);
    read = recv_data[0];

#elif defined(SOFT_SPI)
    CS_0();
	read = user_spi_read_byte();
	read <<= 8;
	read += user_spi_read_byte();
	read <<= 8;
	read += user_spi_read_byte();
	read <<= 8;
	read += user_spi_read_byte();
	CS_1();
#endif

	return read;
}

/*
 * 函数：user_spi_write_byte
 * 说明：SPI写一个字节
 */
void ICACHE_FLASH_ATTR
user_spi_write_byte(u8 data)
{
#if defined(HARD_SPI)
	u32 send_data[1] = {data};
	SpiData spiData;
    spiData.cmd = MASTER_WRITE_DATA_TO_SLAVE_CMD;
    spiData.cmdLen = 0;
    spiData.addr = NULL;
    spiData.addrLen = 0;
    spiData.data = send_data;
    spiData.dataLen = 1;
    SPIMasterSendData(SpiNum_HSPI, &spiData);

#elif defined(SOFT_SPI)
    u8 i;
    CS_0();
	for(i = 0; i < 8; i++){
		if (data & 0x80){
			MOSI_1();
		}else{
			MOSI_0();
		}
		SCK_0();
		data <<= 1;
		//user_spi_delay_us(50);
		SCK_1();
	}
	CS_1();
#endif
}




