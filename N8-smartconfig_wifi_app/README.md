# smartconfig_wifi_app

上电后先使用smartconfig进行配网，一段时候后没有收到配网消息则尝试连接上次成功的Wi-Fi。

工程设置了GPIO0为Wi-Fi状态灯。进入smartconfig模式的`SC_STATUS_FIND_CHANNEL`状态后，状态灯会每隔1秒闪烁一次；当退出smartconfig，Wi-Fi连接失败则会快速闪烁，Wi-Fi连接成功则保持常亮（输出低电平）。

当然也可以启用`WIFI_SSID_ENABLE`，直接使用写死在代码的`WIFI_SSID`和`WIFI_PASS`进行连接。

# 配置

在`include/user_connfig.h`文件可以进行自定义配置：

```C
//***********************************************************************/
// 使用下面的wifi配置进行连接
// 与下面的 SMARTCONFIG_ENABLE 二选一
//#define WIFI_SSID_ENABLE

#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASS "WIFI_PASS"

//***********************************************************************/
// 上电时使用smartconfig进行wifi配置，然后过一段时间自动连接之前配置好的wifi
// 与上面的 WIFI_SSID_ENABLE 二选一
#define SMARTCONFIG_ENABLE

//***********************************************************************/

/* wifi led 引脚配置 */
#define WIFI_STATUS_LED_PIN         0

//***********************************************************************/
```

# 实例效果

烧写bin文件成功后，重启ESP8266，打开串口，如果成功则可以看到下面信息，以下为示例：

```
[INFO] SMARTCONFIG_ENABLE
SC version: V2.5.4
[INFO] smartconfig start!
scandone
scandone
SC_STATUS_FIND_CHANNEL

TYPE: ESPTOUCH
T|AP MAC: <MAC>
SC_STATUS_GETTING_SSID_PSWD
T|pswd: <WIFI_PASS>
T|ssid: <WIFI_SSID>
SC_STATUS_LINK
scandone
state: 0 -> 2 (b0)
state: 2 -> 3 (0)
state: 3 -> 5 (10)
add 0
aid 2
cnt 

connected with WIFI_SSID, channel 13
dhcp client start...
ip:192.168.10.180,mask:255.255.255.0,gw:192.168.10.1
SC_STATUS_LINK_OVER
Phone ip: 192.168.10.133
[INFO] stop smartconfig timer
[INFO] WiFi_LED_STATUS_TIMER_ENABLE
pm open,type:2 0
```

# 移植说明

只需要拷贝以下文件即可简单移植到新工程：

- wifi源码：`user/user_wifi.c`+`include/user_wifi.h`
- smartconfig源码：`user/user_smartconfig.c`+`include/user_smartconfig.h`
- wifi配置：`wifi_config.h`


之后只需在`user_main.c`中这样调用即可：

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
	print_chip_info();
	wifi_set_opmode(STATION_MODE);		// set wifi mode
	wifi_connect(wifi_connect_cb);		// 设置用户wifi回调函数
}

void ICACHE_FLASH_ATTR
user_init(void) {
    // uart_init(BIT_RATE_115200, BIT_RATE_115200);
    system_init_done_cb(init_done_cb_init);
}

```

# ESP8266 smartconfig手机App源码

- Android：https://github.com/EspressifApp/EsptouchForAndroid
- iOS：https://github.com/EspressifApp/EsptouchForIOS
