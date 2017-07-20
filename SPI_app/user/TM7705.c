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
#include "TM7705.h"


//#define SOFT_SPI
#define HARD_SPI


#define ADC1
//#define ADC2

#define RESET_PIN	4
#define DRDY_PIN	5

#define DO_PIN		12
#define DI_PIN		13
#define SCK_PIN		14
#define CS_PIN		15

#define RESET_0()	GPIO_OUTPUT_SET(RESET_PIN, 0)
#define RESET_1()	GPIO_OUTPUT_SET(RESET_PIN, 1)

#define DRDY_IS_LOW()	(GPIO_INPUT_GET(DRDY_PIN) == 0)


#define GPIO_INIT()		do{\
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);\
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);\
		PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO5_U);\
		GPIO_DIS_OUTPUT(DRDY_PIN);\
					}while(0)

#if defined(HARD_SPI)
	#define CS_0()
	#define CS_1()
#elif defined(SOFT_SPI)
	#define DI_0()		GPIO_OUTPUT_SET(DI_PIN, 0)
	#define DI_1()		GPIO_OUTPUT_SET(DI_PIN, 1)

	#define CS_0()		GPIO_OUTPUT_SET(CS_PIN, 0)
	#define CS_1()		GPIO_OUTPUT_SET(CS_PIN, 1)

	#define SCK_0()		GPIO_OUTPUT_SET(SCK_PIN, 0)
	#define SCK_1()		GPIO_OUTPUT_SET(SCK_PIN, 0)

	#define DO_IS_HIGH()	(GPIO_INPUT_GET(DO_PIN) != 0)
#endif

#if defined(HARD_SPI)

// Show the spi registers.
#define SHOWSPIREG(i) __ShowRegValue(__func__, __LINE__);

void __ShowRegValue(const char * func, uint32_t line)
{

    int i;
    uint32_t regAddr = 0x60000140; // SPI--0x60000240, HSPI--0x60000140;
    os_printf("\r\n FUNC[%s],line[%d]\r\n", func, line);
    os_printf(" SPI_ADDR      [0x%08x]\r\n", READ_PERI_REG(SPI_ADDR(SpiNum_HSPI)));
    os_printf(" SPI_CMD       [0x%08x]\r\n", READ_PERI_REG(SPI_CMD(SpiNum_HSPI)));
    os_printf(" SPI_CTRL      [0x%08x]\r\n", READ_PERI_REG(SPI_CTRL(SpiNum_HSPI)));
    os_printf(" SPI_CTRL2     [0x%08x]\r\n", READ_PERI_REG(SPI_CTRL2(SpiNum_HSPI)));
    os_printf(" SPI_CLOCK     [0x%08x]\r\n", READ_PERI_REG(SPI_CLOCK(SpiNum_HSPI)));
    os_printf(" SPI_RD_STATUS [0x%08x]\r\n", READ_PERI_REG(SPI_RD_STATUS(SpiNum_HSPI)));
    os_printf(" SPI_WR_STATUS [0x%08x]\r\n", READ_PERI_REG(SPI_WR_STATUS(SpiNum_HSPI)));
    os_printf(" SPI_USER      [0x%08x]\r\n", READ_PERI_REG(SPI_USER(SpiNum_HSPI)));
    os_printf(" SPI_USER1     [0x%08x]\r\n", READ_PERI_REG(SPI_USER1(SpiNum_HSPI)));
    os_printf(" SPI_USER2     [0x%08x]\r\n", READ_PERI_REG(SPI_USER2(SpiNum_HSPI)));
    os_printf(" SPI_PIN       [0x%08x]\r\n", READ_PERI_REG(SPI_PIN(SpiNum_HSPI)));
    os_printf(" SPI_SLAVE     [0x%08x]\r\n", READ_PERI_REG(SPI_SLAVE(SpiNum_HSPI)));
    os_printf(" SPI_SLAVE1    [0x%08x]\r\n", READ_PERI_REG(SPI_SLAVE1(SpiNum_HSPI)));
    os_printf(" SPI_SLAVE2    [0x%08x]\r\n", READ_PERI_REG(SPI_SLAVE2(SpiNum_HSPI)));

    // 主机使用W0开始的SPI缓存区
    // 从机使用W8开始的缓存区
    for (i = 0; i < 16; ++i) {
        os_printf(" ADDR[0x%08x],Value[0x%08x]\r\n", regAddr, READ_PERI_REG(regAddr));
        regAddr += 4;
    }
}

void ICACHE_FLASH_ATTR
_user_show_reg(u8 addr)
{
	uint32_t regAddr = 0x60000140; // SPI--0x60000240, HSPI--0x60000140;
	regAddr += addr*4;
	os_printf(" ADDR[0x%08x]=Value[0x%08x]\r\n", regAddr, READ_PERI_REG(regAddr));
}
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
TM7705_spi_init(void)
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
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15);

    GPIO_DIS_OUTPUT(DO_PIN);
#else
	#error #error "Please define SPI Interface mode : SOFT_SPI or HARD_SPI"
#endif

    GPIO_INIT();

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
TM7705_Delay(void)
{
	u16 i;

	for (i = 0; i < 5; i++){
		//os_delay_us(1);
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
static uint8_t ICACHE_FLASH_ATTR
TM7705_Recive8Bit(void)
{
	uint8_t i;
	uint8_t read = 0;

#if defined(HARD_SPI)
	u32 recv_data[32];
	SpiData spiData;
    spiData.cmd = MASTER_READ_DATA_FROM_SLAVE_CMD;
    spiData.cmdLen = 0;
    spiData.addr = NULL;
    spiData.addrLen = 0;
    spiData.data = recv_data;
    spiData.dataLen = 1;
    SPIMasterRecvData(SpiNum_HSPI, &spiData);
    read = (u8)recv_data[0];

#elif defined(SOFT_SPI)

	for (i = 0; i < 8; i++){
		SCK_0();
		TM7705_Delay();
		read = read<<1;
		if (DO_IS_HIGH()){
			read++;
		}
		SCK_1();
		TM7705_Delay();
	}
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
	uint8_t read;

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
	uint16_t read;
	CS_0();
#if defined(HARD_SPI)
	u32 recv_data[32];
	SpiData spiData;
    spiData.cmd = MASTER_READ_DATA_FROM_SLAVE_CMD;
    spiData.cmdLen = 0;
    spiData.addr = NULL;
    spiData.addrLen = 0;
    spiData.data = recv_data;
    spiData.dataLen = 2;
    SPIMasterRecvData(SpiNum_HSPI, &spiData);
    read = (u16)recv_data[0];

#elif defined(SOFT_SPI)
	read = TM7705_Recive8Bit();
	read <<= 8;
	read += TM7705_Recive8Bit();
#endif

	CS_1();
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
TM7705_Send8Bit(uint8_t _data)
{
	uint8_t i;

#if defined(HARD_SPI)
	u32 send_data[1] = {_data};
	SpiData spiData;
    spiData.cmd = MASTER_WRITE_DATA_TO_SLAVE_CMD;
    spiData.cmdLen = 0;
    spiData.addr = NULL;
    spiData.addrLen = 0;
    spiData.data = send_data;
    spiData.dataLen = 1;
    SPIMasterSendData(SpiNum_HSPI, &spiData);
#elif defined(SOFT_SPI)
	for(i = 0; i < 8; i++){
		if (_data & 0x80){
			DI_1();
		}else{
			DI_0();
		}
		SCK_0();
		_data <<= 1;
		TM7705_Delay();
		SCK_1();
		TM7705_Delay();
	}
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
TM7705_WriteByte(uint8_t _data)
{
	CS_0();
	TM7705_Send8Bit(_data);
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

	// 0x08
	TM7705_WriteByte(CLKDIS_0 | CLK_4_9152M | FS_50HZ);	/* 刷新速率50Hz */
	//TM7705_WriteByte(CLKDIS_0 | CLK_4_9152M | FS_500HZ);	/* 刷新速率500Hz */

}

/*********************************************************************/

static void ICACHE_FLASH_ATTR
timer_cb(void *arg)
{
	u16 adc1 = 0, adc2 = 0;
	int volt1, volt2;

#if defined(ADC1)
	adc1 = TM7705_ReadAdc(1);
	volt1 = ((s32)adc1 * 3300) / 65535;
#endif

#if defined(ADC2)
	adc2 = TM7705_ReadAdc(2);
	volt2 = ((s32)adc2 * 3300) / 65535;
#endif

	os_printf("CH1=%5ld (%5dmV)\tCH2=%5ld (%5dmV)\r\n",
			(long int)adc1, volt1, (long int)adc2, volt2);

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
    os_timer_arm(&s_timer, (3*1000), 1);

    os_printf("user_TM7705_read_timer_init\r\n");
}


void ICACHE_FLASH_ATTR
user_TM7705_test_init(void)
{
	u16 adc = 0;

	wifi_set_opmode(NULL_MODE);

	TM7705_spi_init();

	bsp_InitTM7705();

#if defined(ADC1)
	TM7705_CalibSelf(1);
	adc = TM7705_ReadAdc(1);
	os_printf("adc:0x%x\r\n", adc);
#endif

#if defined(ADC2)
	adc = 0;
	TM7705_CalibSelf(2);
	adc = TM7705_ReadAdc(2);
	os_printf("adc:0x%x\r\n", adc);
#endif

	user_TM7705_read_timer_init();
	system_soft_wdt_restart();
}
