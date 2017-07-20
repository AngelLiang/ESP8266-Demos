/*
 * ad7705.c
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
#include "TM7705.h"

/* 建议使用SOFT_SPI */


#define ADC1
#define ADC2

#define RESET_PIN	4
#define DRDY_PIN	5

#define TM_7705_GPIO_INIT()		do{\
			PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);\
			PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);\
			PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO5_U);\
			GPIO_DIS_OUTPUT(DRDY_PIN);\
		}while(0)

#define RESET_0()	GPIO_OUTPUT_SET(RESET_PIN, 0)
#define RESET_1()	GPIO_OUTPUT_SET(RESET_PIN, 1)

#define DRDY_IS_LOW()	(GPIO_INPUT_GET(DRDY_PIN) == 0)


#if defined(HARD_SPI)
	#define CS_0()
	#define CS_1()
#elif defined(SOFT_SPI)

#endif

void ICACHE_FLASH_ATTR
bsp_DelayMS(u16 n)
{
	system_soft_wdt_feed();
	while(n--){
		os_delay_us(1000);
	}
}


void ICACHE_FLASH_ATTR
TM7705_pin_init(void)
{
	user_spi_pin_init();
    TM_7705_GPIO_INIT();
}

/*
*********************************************************************************************************
*	函 数 名: TM7705_Delay
*	功能说明: CLK之间的延迟，时序延迟. 对于51，可以不延迟
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void ICACHE_FLASH_ATTR
TM7705_Delay_us(u16 xus)
{
	u16 i;
	for (i = 0; i < xus; i++){
		os_delay_us(1);
	};
}

/*
 * 说明：硬件复位
 */
static void ICACHE_FLASH_ATTR
TM7705_ResetHard(void)
{
	RESET_1();
	bsp_DelayMS(1);
	RESET_0();
	bsp_DelayMS(2);
	RESET_1();
	bsp_DelayMS(1);
	os_printf("TM7705 Reset!\r\n");
}


/*
*********************************************************************************************************
*	函 数 名: TM7705_Recive8Bit
*	功能说明: 从SPI总线接收8个bit数据。 不带CS控制。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static u8 ICACHE_FLASH_ATTR
TM7705_Recive8Bit(void)
{
	u8 read = 0;
	//read = user_spi_read_byte();

#if defined(SOFT_SPI)
	u8 i;
	CS_0();
	for (i = 0; i < 8; i++){
		SCK_0();
		TM7705_Delay_us(50);
		read = read<<1;
		if (MISO_IS_HIGH()){
			read++;
		}
		SCK_1();
		TM7705_Delay_us(50);
	}
	CS_1();
#endif

	return read;
}

/*
*********************************************************************************************************
*	函 数 名: TM7705_ReadByte
*	功能说明: 从AD芯片读取一个字（16位）
*	形    参: 无
*	返 回 值: 读取的字（16位）
*********************************************************************************************************
*/
static uint8_t ICACHE_FLASH_ATTR
TM7705_ReadByte(void)
{
	u8 read;

	CS_0();
	read = TM7705_Recive8Bit();
	CS_1();

	return read;
}

/*
*********************************************************************************************************
*	函 数 名: TM7705_Read2Byte
*	功能说明: 读2字节数据
*	形    参: 无
*	返 回 值: 读取的数据（16位）
*********************************************************************************************************
*/
static uint16_t ICACHE_FLASH_ATTR
TM7705_Read2Byte(void)
{
	u16 read = 0;
#if 0
	read = user_spi_read_2byte();
#else
	u8 i;
	CS_0();
	for (i = 0; i < 16; i++){
		SCK_0();
		//TM7705_Delay_us(20);
		read = read<<1;
		if (MISO_IS_HIGH()){
			read++;
		}
		SCK_1();
		//TM7705_Delay_us(50);
	}
	CS_1();
#endif
	return read;
}


/*
*********************************************************************************************************
*	函 数 名: TM7705_Send8Bit
*	功能说明: 向SPI总线发送8个bit数据。 不带CS控制。
*	形    参: _data : 数据
*	返 回 值: 无
*********************************************************************************************************
*/
static void ICACHE_FLASH_ATTR
TM7705_Send8Bit(u8 data)
{
	//user_spi_write_byte(data);
#if defined(SOFT_SPI)
    u8 i;
    CS_0();
	for(i = 0; i < 8; i++){
		if (data & 0x80){
			MOSI_1();
		}else{
			MOSI_0();
		}
		TM7705_Delay_us(50);
		SCK_0();
		data <<= 1;
		TM7705_Delay_us(50);
		SCK_1();
	}
	CS_1();
#endif
}


/*
*********************************************************************************************************
*	函 数 名: TM7705_WriteByte
*	功能说明: 写入1个字节。带CS控制
*	形    参: _data ：将要写入的数据
*	返 回 值: 无
*********************************************************************************************************
*/
static void ICACHE_FLASH_ATTR
TM7705_WriteByte(u8 data)
{
	CS_0();
	TM7705_Send8Bit(data);
	CS_1();
}


/*
*********************************************************************************************************
*	函 数 名: TM7705_SyncSPI
*	功能说明: 同步TM7705芯片SPI接口时序
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void ICACHE_FLASH_ATTR
TM7705_SyncSPI(void)
{
	/* AD7705串行接口失步后将其复位。复位后要延时500us再访问 */
	CS_0();
	TM7705_Send8Bit(0xFF);
	TM7705_Send8Bit(0xFF);
	TM7705_Send8Bit(0xFF);
	TM7705_Send8Bit(0xFF);
	CS_1();
}

/*
*********************************************************************************************************
*	函 数 名: TM7705_WaitDRDY
*	功能说明: 等待内部操作完成。 自校准时间较长，需要等待。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void ICACHE_FLASH_ATTR
TM7705_WaitDRDY(void)
{
	u32 i;

	for (i = 0; i < 8000; i++){
		if (DRDY_IS_LOW()){
			break;
		}
		//bsp_DelayMS(1);
		os_delay_us(200);
	}

	if (i >= 8000){
		os_printf("TM7705_WaitDRDY() timeout ...\r\n");
		TM7705_SyncSPI();		/* 同步SPI接口时序 */
		bsp_DelayMS(1);
	}
}

/*
*********************************************************************************************************
*	函 数 名: TM7705_CalibSelf
*	功能说明: 启动自校准. 内部自动短接AIN+ AIN-校准0位，内部短接到Vref 校准满位。此函数执行过程较长，
*			  实测约 180ms
*	形    参:  _ch : ADC通道，1或2
*	返 回 值: 无
*********************************************************************************************************
*/
void ICACHE_FLASH_ATTR
TM7705_CalibSelf(uint8_t _ch)
{
	if (_ch == 1){
		/* 自校准CH1 */
		TM7705_WriteByte(REG_SETUP | WRITE | CH_1);	/* 写通信寄存器，下一步是写设置寄存器，通道1 */
		TM7705_WriteByte(MD_CAL_SELF | __CH1_GAIN_BIPOLAR_BUF | FSYNC_0);/* 启动自校准 */
		TM7705_WaitDRDY();	/* 等待内部操作完成 --- 时间较长，约180ms */
	}
	else if (_ch == 2){
		/* 自校准CH2 */
		TM7705_WriteByte(REG_SETUP | WRITE | CH_2);	/* 写通信寄存器，下一步是写设置寄存器，通道2 */
		TM7705_WriteByte(MD_CAL_SELF | __CH2_GAIN_BIPOLAR_BUF | FSYNC_0);	/* 启动自校准 */
		TM7705_WaitDRDY();	/* 等待内部操作完成  --- 时间较长，约180ms */
	}
	os_printf("TM7705_CalibSelf %d\r\n", _ch);
}

/*
*********************************************************************************************************
*	函 数 名: TM7705_SytemCalibZero
*	功能说明: 启动系统校准零位. 请将AIN+ AIN-短接后，执行该函数。校准应该由主程序控制并保存校准参数。
*			 执行完毕后。可以通过 TM7705_ReadReg(REG_ZERO_CH1) 和  TM7705_ReadReg(REG_ZERO_CH2) 读取校准参数。
*	形    参: _ch : ADC通道，1或2
*	返 回 值: 无
*********************************************************************************************************
*/
void ICACHE_FLASH_ATTR
TM7705_SytemCalibZero(uint8_t _ch)
{
	if (_ch == 1)
	{
		/* 校准CH1 */
		TM7705_WriteByte(REG_SETUP | WRITE | CH_1);	/* 写通信寄存器，下一步是写设置寄存器，通道1 */
		TM7705_WriteByte(MD_CAL_ZERO | __CH1_GAIN_BIPOLAR_BUF | FSYNC_0);/* 启动自校准 */
		TM7705_WaitDRDY();	/* 等待内部操作完成 */
	}
	else if (_ch == 2)
	{
		/* 校准CH2 */
		TM7705_WriteByte(REG_SETUP | WRITE | CH_2);	/* 写通信寄存器，下一步是写设置寄存器，通道1 */
		TM7705_WriteByte(MD_CAL_ZERO | __CH2_GAIN_BIPOLAR_BUF | FSYNC_0);	/* 启动自校准 */
		TM7705_WaitDRDY();	/* 等待内部操作完成 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: TM7705_SytemCalibFull
*	功能说明: 启动系统校准满位. 请将AIN+ AIN-接最大输入电压源，执行该函数。校准应该由主程序控制并保存校准参数。
*			 执行完毕后。可以通过 TM7705_ReadReg(REG_FULL_CH1) 和  TM7705_ReadReg(REG_FULL_CH2) 读取校准参数。
*	形    参:  _ch : ADC通道，1或2
*	返 回 值: 无
*********************************************************************************************************
*/
void ICACHE_FLASH_ATTR
TM7705_SytemCalibFull(uint8_t _ch)
{
	if (_ch == 1)
	{
		/* 校准CH1 */
		TM7705_WriteByte(REG_SETUP | WRITE | CH_1);	/* 写通信寄存器，下一步是写设置寄存器，通道1 */
		TM7705_WriteByte(MD_CAL_FULL | __CH1_GAIN_BIPOLAR_BUF | FSYNC_0);/* 启动自校准 */
		TM7705_WaitDRDY();	/* 等待内部操作完成 */
	}
	else if (_ch == 2)
	{
		/* 校准CH2 */
		TM7705_WriteByte(REG_SETUP | WRITE | CH_2);	/* 写通信寄存器，下一步是写设置寄存器，通道1 */
		TM7705_WriteByte(MD_CAL_FULL | __CH2_GAIN_BIPOLAR_BUF | FSYNC_0);	/* 启动自校准 */
		TM7705_WaitDRDY();	/* 等待内部操作完成 */
	}
}


/*
*********************************************************************************************************
*	函 数 名: TM7705_ReadAdc1
*	功能说明: 读通道1或2的ADC数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t ICACHE_FLASH_ATTR
TM7705_ReadAdc(u8 _ch)
{
	u8 i;
	u16 read = 0;

	/* 为了避免通道切换造成读数失效，读2次 */
	for (i = 0; i < 2; i++){
		TM7705_WaitDRDY();		/* 等待DRDY口线为0 */
		if (_ch == 1){
			TM7705_WriteByte(0x38);
		}else if (_ch == 2){
			TM7705_WriteByte(0x39);
		}
		read = TM7705_Read2Byte();
	}
	return read;
}


/*
*********************************************************************************************************
*	函 数 名: bsp_InitTM7705
*	功能说明: 配置STM32的GPIO和SPI接口，用于连接 TM7705
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void ICACHE_FLASH_ATTR
bsp_InitTM7705(void)
{
	bsp_DelayMS(10);

	TM7705_ResetHard();		/* 硬件复位 */

	/*
		在接口序列丢失的情况下，如果在DIN 高电平的写操作持续了足够长的时间（至少 32个串行时钟周期），
		TM7705 将会回到默认状态。
	*/
	bsp_DelayMS(5);

	TM7705_SyncSPI();		/* 同步SPI接口时序 */

	bsp_DelayMS(5);

	/* 配置时钟寄存器 */
	TM7705_WriteByte(REG_CLOCK | WRITE | CH_1);			/* 先写通信寄存器，下一步是写时钟寄存器 */

	TM7705_WriteByte(CLKDIS_0 | CLK_4_9152M | FS_50HZ);	/* 刷新速率50Hz */
	//TM7705_WriteByte(CLKDIS_0 | CLK_4_9152M | FS_500HZ);	/* 刷新速率500Hz */

}

/*********************************************************************/

static int volt1, volt2;

s16 ICACHE_FLASH_ATTR
get_tm7705_adc1(void)
{
	return volt1;
}

s16 ICACHE_FLASH_ATTR
get_tm7705_adc2(void)
{
	return volt2;
}

static void ICACHE_FLASH_ATTR
timer_cb(void *arg)
{
	u16 adc1 = 0, adc2 = 0;

#if defined(ADC1)
	adc1 = TM7705_ReadAdc(1);
	volt1 = ((s32)adc1 * 5000) / 65535;
#endif

#if defined(ADC2)
	adc2 = TM7705_ReadAdc(2);
	volt2 = ((s32)adc2 * 5000) / 65535;
#endif

#if 1
	os_printf("CH1=%5ld (%5dmV)\tCH2=%5ld (%5dmV)\r\n",
			(long int)adc1, volt1, (long int)adc2, volt2);
#endif
}

/*
 * 函数：user_ad7705_read_timer_init
 * 说明：定时读取ad7705值
 */
void ICACHE_FLASH_ATTR
user_TM7705_read_timer_init(void)
{
	static os_timer_t s_timer;

    os_timer_disarm(&s_timer);
    os_timer_setfn(&s_timer, timer_cb , NULL);
    os_timer_arm(&s_timer, (1*1000), 1);

    os_printf("user_TM7705_read_timer_init\r\n");
}

void ICACHE_FLASH_ATTR
user_tm7705_init(void)
{
	TM7705_pin_init();
	bsp_InitTM7705();

#if defined(ADC1)
	TM7705_CalibSelf(1);
	//adc = TM7705_ReadAdc(1);
#endif

#if defined(ADC2)
	TM7705_CalibSelf(2);
	//adc = TM7705_ReadAdc(2);
#endif

	user_TM7705_read_timer_init();
}


void ICACHE_FLASH_ATTR
user_TM7705_test_init(void)
{
	wifi_set_opmode(NULL_MODE);
	user_tm7705_init();
}
