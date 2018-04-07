# SPI_app

本工程有以下内容，可以在user_config.h根据宏定义来编译

| 宏定义 | 说明 |
|---|---|
| SPI_TEST | 官方SPI测试用例 |
| OLED | 基于SOFT_SPI的OLED示例 |
| TM7705 | 基于SOFT_SPI TM7705 ADC模块的读取 |

```
#define SPI_TEST
//#define OLED
//#define TM7705
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

## TM7705

