# TCP-SSL_client_app

SSL客户端连接本地服务器测试步骤

1. 在电脑上使用Python3运行server\ssl_server.py；
2. 修改user_wifi.c文件下的相关宏定义；
3. ESP8266烧写bin文件，并且需要烧写server\TLS_BiDirectVerif_Demo\client-cert文件夹下的esp_cert_private_key.bin（烧写地址：0x9A000）；
4. ESP8266重启上电，自动连接Wi-Fi后就会连接电脑上的TCP server。

user_wifi.c中可能需要修改的宏定义

```
#define WIFI_SSID 		"your ssid"
#define WIFI_PASSWORD	"your password"

#define HOST	"192.168.10.70"
#define PORT	2000
```
