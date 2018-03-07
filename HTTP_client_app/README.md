# HTTP_client_app

使用ESP8266的ESP_TCP接口作为HTTP Client。

修改好配置后，在串口中发送`request`字符串即可连接HTTP Server。

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
#define _HTTP_HEADER_HOST	"Host: 192.168.10.168:5000\r\n"
#define HTTP_REQUEST "GET "FILE_URL" HTTP/1.1\r\n"_HTTP_HEADER_HOST"\r\n"
```


