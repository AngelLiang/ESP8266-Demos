# upgrade_app

ESP8266远程更新固件。

修改好配置后，在串口中发送`upgrade`字符串即可更新固件。

# 相关配置

`user_config.h`中可能需要修改的宏定义：

```C
// user_config.h
// ...
#define WIFI_SSID 		"WIFI_SSID"
#define WIFI_PASSWORD	"WIFI_PASSWORD"

#define HOST	"192.168.10.168"
#define PORT	5000

#define FILE_URL "/admin/fileadmin/download/test.txt"
```


