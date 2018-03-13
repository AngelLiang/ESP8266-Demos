# speed_test_app

ESP8266 TCP/UDP 速率测试工程。

# 配置

在`include/user_config.h`文件中需要修改的配置：

```
// ...
#define WIFI_SSID		"WIFI_SSID"
#define WIFI_PASSWORD	"WIFI_PASSWORD"

// 是否打印TCP速率
#define TCP_SEPPD_TEST
// 是否打印UDP速率
#define UDP_SEPPD_TEST
```

# 特点

- 使用硬件中断定时器进行每秒的接收数据统计，确保定时时间准确
- 由于ESP8266是接收数据回调（lwip raw callback）模式，回调函数处理时仅仅是累加接收到的数据包大小，如果增加处理代码（比如内存拷贝，写入Flash等），速率估计会下降一点

# 测试数据

网络拓扑结构为：PC和ESP8266连接无线路由器，PC端对ESP8266发起测试。

## TCP速率测试

在局域网中，使用PCATTCP进行速率测试，测试结果如下：

```
> .\PCATTCP.exe -t -p18266 192.168.10.153
PCAUSA Test TCP Utility V2.01.01.06
TCP Transmit Test
  Transmit    : TCP -> 192.168.10.153:18266
  Buffer Size : 8192; Alignment: 16384/0
  TCP_NODELAY : DISABLED (0)
  Connect     : Connected to 192.168.10.153:18266
  Send Mode   : Send Pattern; Number of Buffers: 2048
  Statistics  : TCP -> 192.168.10.153:18266
16777216 bytes in 12.84 real seconds = 1275.62 KB/sec +++
numCalls: 2048; msec/call: 6.42; calls/sec: 159.45
```

结论：PCATTCP在12.84s内发送了16777216 bytes数据，相当于1275.62 KB/sec。

经过反复测试，根据网络情况，ESP8266接收速率会在1200KB/s~1600KB/s之间浮动。

## UDP速率测试

待测

