# ESP8266-Demos

ESP8266示例工程集合，基于ESP8266_NONOS_SDKv2.0。

| 工程名称 | 说明 |
|---|---|
| ADC_app| 定时读取ADC数值 |
| CoAP_app | 搭建一个简单的CoAP Server |
| cJSON_app | 使用cJSON示例 |
| esp-now_app | ESP-NOW使用示例，分为slave和controller两部分 |
| GPIO_app | 定时设置GPIO和GPIO的中断处理函数 |
| IIC_app | IIC使用示例 |
| ir_app | IR接口示例 |
| SmartConfig_app | 简单SmartConfig示例 |
| SmartConfig+WiFi_app | 上电先SmartConfig，一段时间后WiFi连接 |
| sniffer_app | sniffer使用示例 |
| sntp_app | SNTP使用示例工程 |
| SPI_app | SPI使用示例，支持HARD_SPI和SOFT_SPI |
| todo_app | 空白工程 |
| TCP-UDP_server_app | TCP和UDP服务器示例 |
| base64_encode_app | base64编码示例 |
| TCP-SSL_client_app | TCP和SSL客户端示例 |
| wifi_cb_app | 连接Wi-Fi后回调事件功能 |

部分app文件夹下有说明文件（README.md）和效果图（screenshot）。

# 使用步骤

1. 首先到[github](https://github.com/espressif/ESP8266_NONOS_SDK)或者[官网](http://espressif.com/zh-hans/products/hardware/esp8266ex/resources)下载SDK；
2. 然后下载并拷贝相关工程文件夹（比如cJSON_app）到主目录，用法类似于ESP8266_NONOS_SDK/examples里面的工程；
3. 最后编译下载即可。

# 注意事项

串口打印默认为76800bps。




