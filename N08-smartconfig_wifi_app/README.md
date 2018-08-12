# smartconfig_wifi_app

上电后先使用smartconfig进行配网，一段时候后没有收到配网消息则尝试连接上次成功的Wi-Fi。

工程设置了GPIO0为Wi-Fi状态灯。进入smartconfig模式的`SC_STATUS_FIND_CHANNEL`状态后，状态灯会每隔1秒闪烁一次；当退出smartconfig，Wi-Fi连接失败则会快速闪烁，Wi-Fi连接成功则保持常亮（输出低电平）。

当然也可以启用`WIFI_SSID_ENABLE`，直接使用写死在代码的`WIFI_SSID`和`WIFI_PASS`进行连接。



## 配置

在`include/user_connfig.h`文件可以进行自定义配置：

```C
//***********************************************************************/
// 使用下面的wifi配置进行连接
// 调用 wifi_connect() 即可

#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASS "123456789"

//***********************************************************************/

/* wifi led 引脚配置 */
#define WIFI_STATUS_LED_PIN         0

//***********************************************************************/
```



## 实例效果

烧写bin文件成功后，重启ESP8266，打开串口，如果成功则可以看到下面信息，以下为示例：

```
SDK ver: 2.0.0(656edbf) compiled @ Jul 19 2016 17:58:40
phy ver: 1055, pp ver: 10.2

SC version: V2.5.4
[INFO] smartconfig start!
mode : sta(5c:cf:7f:f7:99:6f)
add if0
wifi_smartconfig_timer_cb
[INFO] smartconfig stop!
wifi ssid:WIFI_SSID 
wifi pass:123456789 
[INFO] WiFi_LED_STATUS_TIMER_ENABLE
wifi connect fail!
scandone
state: 0 -> 2 (b0)
state: 2 -> 3 (0)
state: 3 -> 5 (10)
add 0
aid 1
cnt 
wifi connect fail!

connected with WIFI_SSID, channel 11
dhcp client start...
wifi connect fail!
ip:192.168.191.2,mask:255.255.255.0,gw:192.168.191.1
wifi connect success!
pm open,type:2 0
```



## 相关接口

- `void wifi_connect(WifiCallback cb);`：直接使用`WIFI_SSID`和`WIFI_PASS`进行连接
- `void smartconfig_connect(WifiCallback cb);`：先进行smartconfig，如果没有配网信息则自动连接上次的wifi



## 移植说明

只需要拷贝以下文件即可简单移植到新工程：

- wifi源码：`user/user_wifi.c`+`include/user_wifi.h`
- smartconfig源码：`user/user_smartconfig.c`+`include/user_smartconfig.h`
- wifi配置：`wifi_config.h`

之后参考`user_main.c`的代码调用即可：

```C
#include "user_wifi.h"

// ...

void ICACHE_FLASH_ATTR
wifi_connect_cb(u8 status) {
	if (status == STATION_GOT_IP) {
		os_printf("wifi connect success!\r\n");
	} else {

	}
}

void ICACHE_FLASH_ATTR
init_done_cb_init(void) {
	//print_chip_info();

	/*
	 * smartconfig_connect 只能在 init_done_cb_init 调用才正常
	 * 先进行smartconfig，没有配网信息则自动连接上次的wifi
	 */
	smartconfig_connect(wifi_connect_cb);
	/* OR */
	//wifi_connect(wifi_connect_cb);
}

void ICACHE_FLASH_ATTR
user_init(void) {
	//uart_init(BIT_RATE_115200, BIT_RATE_115200);

	wifi_set_opmode(STATION_MODE);		// set wifi mode

	/* 与 smartconfig_connect() 二选一
	 * wifi_connect() 在这里调用正常
	 */
	//wifi_connect(wifi_connect_cb);
	/* 在这里调用不正常，无法用手机配网  */
	//smartconfig_connect(wifi_connect_cb);

	system_init_done_cb(init_done_cb_init);
}

```



## ESP8266 smartconfig手机配网App源码

- Android：https://github.com/EspressifApp/EsptouchForAndroid
- iOS：https://github.com/EspressifApp/EsptouchForIOS
