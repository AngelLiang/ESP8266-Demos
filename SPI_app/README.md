# SPI_app

SPI示例工程，需要两个ESP8266，一个烧写Master代码，另一个烧写Slave代码。

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

