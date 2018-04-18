# tcp_ssl_client_app

ESP8266作为TCP/SSL client示例工程。

# 作为ssl client操作说明

工程默认是TCP client，如果要测试SSL，可以按照下面步骤进行：

1. 启动SSL服务器：在电脑上使用Python3运行`python_server\ssl_server.py`；
2. 修改宏定义：修改`user_config.h`文件下的相关宏定义；
3. 烧写bin：ESP8266烧写bin文件，并且需要烧写`server\TLS_BiDirectVerif_Demo\client-cert`文件夹下的`esp_cert_private_key.bin`（烧写地址：0x9A000）；
4. ESP8266重启上电，自动连接Wi-Fi后就会连接电脑上的TCP server。

# 相关配置

`user_config.h`中可能需要修改的宏定义：

```C
// user_config.h
// ...
#define WIFI_SSID     "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define HOST	"192.168.10.70"
#define PORT	2000

// 是否使用SSL
#define SSL_CLIENT_ENABLE		0
```


