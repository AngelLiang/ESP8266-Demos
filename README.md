# ESP8266-Demos

ESP8266示例工程集合，基于ESP8266_NONOS_SDKv2.0

| 工程名称 | 说明 |
|---|---|
| ESP8266-cJSON-demo | 在ESP8266中使用cJSON示例工程 |
| ESP8266-Sniffer-demo | 使用ESP8266的sniffer功能 |
| ESP8266-TODO-demo | ESP8266空白工程 |
| ESP8266-SmartConfig-demo | ESP8266使用SmartConfig示例 |
| ESP8266-wifi_cb-demo | ESP8266连接Wi-Fi后回调事件 |
| ESP8266-TCP&UDP_server-demo | ESP8266搭建TCP&UDP服务器示例 |
| ESP8266-TCP&SSL_client-demo | ESP8266TCP&SSL连接服务器示例 |

# 使用步骤

1. 首先到[github](https://github.com/espressif/ESP8266_NONOS_SDK)或者[官网](http://espressif.com/zh-hans/products/hardware/esp8266ex/resources)下载SDK；
2. 然后拷贝相关工程文件夹（比如ESP826-cJSON-demo）到主目录，类似于ESP8266_NONOS_SDK/examples里面的工程文件夹；
3. 最后编译下载即可。



# ESP8266-cJSON-demo

在ESP8266中使用cJSON，目前cJSON最新版本是1.5.3。

由于ESP8266不支持浮点数，修改了cJSON中有关double的函数。

效果图

![ESP826-cJSON-demo](https://github.com/AngelLiang/ESP8266-Demos/blob/master/img/ESP8266-cJSON-demo.jpg)

相关博客：http://blog.csdn.net/yannanxiu/article/details/52713746



# ESP8266-sniffer-demo

ESP8266使用sniffer获取周围设备的MAC地址。

效果图

![ESP826-sniffer-demo](https://github.com/AngelLiang/ESP8266-Demos/blob/master/img/ESP8266-sniffer-demo.jpg)

相关博客：http://blog.csdn.net/yannanxiu/article/details/72778688
