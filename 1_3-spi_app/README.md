# SPI_app

本工程有以下内容，可以在`user_config.h`根据宏定义来编译

| 宏定义 | 说明 |
|---|---|
| SPI_TEST | 官方SPI测试用例 |
| OLED | 基于SOFT_SPI控制OLED示例 |
| TM7705 | 基于SOFT_SPI读取TM7705 ADC模块示例 |

```C
#define SPI_TEST
//#define OLED
//#define TM7705
```

# 如何使用本工程

关于如何使用本工程的软件模拟SPI，核心代码请看`app/user/user_spi.c`和`app/include/user_spi.h`。

如果需要移植，拷贝上面两份文件到自己的工程然后调用头文件提供的接口即可。具体示例可以参考后面 OLED 调用 user spi 接口代码。

引脚宏定义

```C
// app/include/user_spi.h

/********************************************************************/
// 可能需要修改下面的宏定义

//#define HARD_SPI
#define SOFT_SPI

#define MISO_PIN 12
#define MOSI_PIN 13
#define SCK_PIN  14
#define CS_PIN	   15

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

/********************************************************************/
// 接口

/* spi引脚初始化 */
extern void user_spi_pin_init(void);
/* spi读取1个字节 */
extern u8 user_spi_read_byte(void);
/* spi读取2个字节 */
extern u16 user_spi_read_2byte(void);
/* spi读取4个字节 */
extern u32 user_spi_read_4byte(void);
/* spi写1个字节 */
extern void user_spi_write_byte(u8);
```

## spi_test

官方SPI测试用例。需要两个ESP8266，一个烧写Master代码，另一个烧写Slave代码。

硬件连线

Master | Slave
---|---
HSPI_CLK | HSPI_CLK
HSPI_MISO | HSPI_MISO
HSPI_MOSI | HSPI_MOSI
HSPI_CS | HSPI_CS

如果两个ESP8266是分别独立供电，还需要连接GND。

NodeMCU与ESP8266引脚对应表

NodeMCU引脚 | ESP8266引脚
---|---
D5 | HSPI_CLK
D6 | HSPI_MISO
D7 | HSPI_MOSI
D8 | HSPI_CS


## OLED

基于软件模拟SPI（SOFT_SPI）的OLED示例。

引脚连接

 NodeMCU | ESP8266 | Function | OLED
 ---|---|---|---
 D5 | GPIO14 | HSCLK | CLK
 D6 | GPIO12 | HMISO | NA
 D7 | GPIO13 | HMOSI | MOSI
 D8 | GPIO15 | HCS | CS1
 D1 | GPIO5 | GPIO | DC
 D2 | GPIO4 | GPIO | FSO
 D4 | GPIO2 | GPIO | CS2
 
OLED 封装 user api 接口相关代码： 
 
 ```C
 // app/user/oled.c
 
 /**********************************************************************/
// OLED 底层接口，封装 user spi接口

/*
 * OLED 引脚初始化，把 user spi 接口封装一下。
 * @function: oled_pin_init
 */
static void ICACHE_FLASH_ATTR
oled_pin_init(void)
{
	// spi 初始化
	user_spi_pin_init();

	// OLED 相关引脚的初始化
	ROM_CS_PIN_INIT();
	DC_PIN_INIT();
	FSO_PIN_INIT();
}


/*
 * 对 OLED 写一个字节，本质是封装了 user spi接口。
 * @function: oled_write_byte
 */
static void ICACHE_FLASH_ATTR
oled_write_byte(u8 data)
{
	// 调用 user spi 接口
	user_spi_write_byte(data);
}

/**********************************************************************/
 ```

## TM7705

基于SOFT_SPI读取TM7705 ADC模块示例。
