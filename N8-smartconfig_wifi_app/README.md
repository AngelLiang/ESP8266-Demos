# smartconfig_wifi_app

上电后先使用smartconfig进行配网，一段时候后没有收到配网消息则尝试连接上次成功的Wi-Fi。

工程设置了GPIO0为Wi-Fi状态灯。进入smartconfig模式的`SC_STATUS_FIND_CHANNEL`状态后，状态灯会每隔1秒闪烁一次；当退出smartconfig，Wi-Fi连接失败则会快速闪烁，Wi-Fi连接成功则保持常亮（输出低电平）。

当然也可以启用`WIFI_SSID_ENABLE`，直接使用写死在代码的`WIFI_SSID`和`WIFI_PASS`进行连接。

# 配置

在`include/user_connfig.h`文件可以进行自定义配置：

```C
//*********************************************************/
// 使用下面的wifi配置进行连接
// 与下面的 SMARTCONFIG_ENABLE 二选一
//#define WIFI_SSID_ENABLE

#define WIFI_SSID "wifi_ssid"
#define WIFI_PASS "wifi_pass"


//*********************************************************/
// 上电时使用smartconfig进行wifi配置
// 与上面的 WIFI_SSID_ENABLE 二选一
#define SMARTCONFIG_ENABLE


#define DISABLE     0
#define TIMER_MODE  1
#define EVENT_MODE  2

/*
 * wifi检查连接方式
 * 0 - disable：不检查wifi连接
 * 1 - timer mode：定时检查wifi连接
 * 2 - event mode：注册事件的方式检查wifi连接
 */
#define WIFI_CHECK_MODE     TIMER_MODE



//#define DISABLE       0
//#define TIMER_MODE    1
#define ESP_MODE        2

/*
 * 是否使用定时器控制wifi状态led灯
 * 0 - disable：不使用wifi led状态灯
 * 1 - timer mode：使用定时器设置wifi led状态灯
 * 2 - esp mode ：使用ESP自带的接口配置wifi led状态的
 */
#define WIFI_LED_MODE   TIMER_MODE

/* wifi led 引脚配置 */
#if TIMER_MODE==WIFI_LED_MODE
    #define WIFI_STATUS_LED_PIN         0
#elif ESP_MODE==WIFI_LED_MODE
    #define HUMITURE_WIFI_LED_IO_MUX    PERIPHS_IO_MUX_GPIO0_U
    #define HUMITURE_WIFI_LED_IO_NUM    0
    #define HUMITURE_WIFI_LED_IO_FUNC   FUNC_GPIO0
#else
#endif

//*********************************************************/
```

# 效果图

![image](screenshot/SmartConfig-WiFi.jpg)

# 移植说明

只需要拷贝以下文件即可简单移植到新工程：

- `user/wifi.c`+`include/user_wifi.h`
- `user/user_smartconfig.c`+`include/user_smartconfig.h`
- `user_config.h`中有关的配置


之后只需在`user_main.c`中这样调用即可：

```C
#include "user_wifi.h"

// ...

void ICACHE_FLASH_ATTR
init_done_cb_init(void) {
    // ...
	wifi_connect();
}

void ICACHE_FLASH_ATTR
user_init(void) {
	// uart_init(BIT_RATE_115200, BIT_RATE_115200);
	system_init_done_cb(init_done_cb_init);
}

```
