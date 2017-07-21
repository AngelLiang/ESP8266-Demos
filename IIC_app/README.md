# IIC_app

由于ESP8266的IIC接口做得非常好，这里参考官方文档就可以用了。下面也主要根据官方文档做一些讲解。

主要文件driver\i2c_master.c和include\driver\i2c_master.h。

## IIC默认引脚

| NodeMCU引脚 | ESP8266引脚 | 功能 |
|---|---|---|
| D5 | GPIO14 | I2C_SCL |
| D4 | GPIO2 | I2C_SDA |

可以在include\driver\i2c_master.h文件中修改成其他引脚

```
// 大约第28行开始
#define I2C_MASTER_SDA_MUX 		PERIPHS_IO_MUX_GPIO2_U
#define I2C_MASTER_SCL_MUX 		PERIPHS_IO_MUX_MTMS_U
#define I2C_MASTER_SDA_GPIO 	2
#define I2C_MASTER_SCL_GPIO 	14
#define I2C_MASTER_SDA_FUNC 	FUNC_GPIO2
#define I2C_MASTER_SCL_FUNC 	FUNC_GPIO14
```

## PCF8591

PCF8591为本人测试IIC接口所用，可以对接PCF8591 ADC芯片。

此代码可以作为使用IIC的参考。