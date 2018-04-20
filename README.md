# ESP8266-Demos

ESP8266示例工程集合，基于ESP8266_NONOS_SDKv2.0。

| 序号 | 工程名称 | 说明 |
|---|---|---|
| 0 | empty_app | 空白工程 |
| 1_1 | gpio_app | 定时设置GPIO和GPIO的中断处理函数 |
| 1_2 | iic_app | IIC使用示例 |
| 1_3 | spi_app | SPI使用示例，支持HARD_SPI和SOFT_SPI |
| 1_4 | adc_app| 定时读取ADC数值 |
| 2_1 | hw_timer_app | ESP8266使用硬件中断定时器示例 |
| 2_2 | ir_app | IR接口示例 |
| 3_1 | CoAP_app | 搭建一个简单的CoAP Server |
| 3_3 | sntp_app | SNTP使用示例工程 |
| 3_2 | http_client_app | ESP8266作为http client示例 |
| 3_4 | esp_now_app | ESP-NOW使用示例，分为slave和controller两部分 |
| 4_1 | cJSON_app | 使用cJSON示例 |
| 4_2 | base64_app | base64编码示例 |
| 4_3 | md5_app | ESP8266使用MD5进行Hash |
| 5_2 | tcp_udp_server_app | TCP和UDP服务器示例 |
| 5_1 | tcp_ssl_client_app | TCP和SSL客户端示例 |
| 6_1 | smartconfig_app | 简单SmartConfig示例 |
| 6_2 | smartconfig_WiFi_app | 上电先SmartConfig，一段时间后WiFi连接 |
| 6_3 | sniffer_app | sniffer使用示例 |
| 6_4 | wifi_event_cb_app | 连接Wi-Fi后回调事件功能 |
| 7_1 | upgrade_app |  远程更新固件示例 |
| - | lwip_open_src_template_proj | 官网提供的lwip示例工程，添加了一些个人示例代码 |
| - | speed_test_app | ESP8266 TCP/UDP速率测试 |

部分app文件夹下有说明文件（README.md）和效果图（screenshot）。



# 作者所用的开发环境

- Windows 10 x64
- [安信可 ESP 系列一体化开发环境](http://wiki.ai-thinker.com/ai_ide_install)



# 使用步骤

1. 首先到[github](https://github.com/espressif/ESP8266_NONOS_SDK)或者[官网](http://espressif.com/zh-hans/products/hardware/esp8266ex/resources)下载SDK；
2. 然后下载并拷贝相关工程文件夹（比如cJSON_app）到主目录，用法类似于ESP8266_NONOS_SDK/examples里面的工程；
3. 最后编译下载即可（建议先clean）。

![project_sample](screenshot/project_sample.png)

# 注意事项

串口打印默认为74800bps。

