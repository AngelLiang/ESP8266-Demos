# ESP8266-Demos

ESP8266示例工程集合，基于ESP8266_NONOS_SDKv2.0

| 工程名称 | 说明 |
|---|---|
| cJSON_app | 使用cJSON示例工程 |
| sniffer_app | 使用sniffer功能 |
| todo_app | 空白工程 |
| SmartConfig_app | 使用SmartConfig示例工程 |
| wifi_cb_app | 连接Wi-Fi后回调事件功能 |
| ETCP&UDP_server_app | TCP&UDP服务器示例 |
| TCP&SSL_client_app | TCP&SSL客户端示例 |
| ADC_app| 定时读取ADC数值 |
| GPIO_app| 定时设置GPIO和GPIO的中断处理函数 |

# 使用步骤

1. 首先到[github](https://github.com/espressif/ESP8266_NONOS_SDK)或者[官网](http://espressif.com/zh-hans/products/hardware/esp8266ex/resources)下载SDK；
2. 然后拷贝相关工程文件夹（比如ESP826-cJSON-demo）到主目录，类似于ESP8266_NONOS_SDK/examples里面的工程文件夹；
3. 最后编译下载即可。



# cJSON_app

在ESP8266中使用cJSON，目前cJSON最新版本是1.5.3。

由于ESP8266不支持浮点数，修改了cJSON中有关double的函数。

效果图

![ESP826-cJSON-demo](https://github.com/AngelLiang/ESP8266-Demos/blob/master/img/cJSON.jpg)

相关博客：http://blog.csdn.net/yannanxiu/article/details/52713746



# sniffer_app

ESP8266使用sniffer获取周围设备的MAC地址。

效果图

![ESP826-sniffer-demo](https://github.com/AngelLiang/ESP8266-Demos/blob/master/img/sniffer.jpg)

相关博客：http://blog.csdn.net/yannanxiu/article/details/72778688
