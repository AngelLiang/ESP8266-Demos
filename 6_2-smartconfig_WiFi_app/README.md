# SmartConfig-WiFi_app

上电后先使用smartconfig进行配网，一段时候后没有收到配网消息则尝试连接上次成功的Wi-Fi。

工程设置了GPIO0为Wi-Fi状态灯。进入smartconfig模式的`SC_STATUS_FIND_CHANNEL`状态后，状态灯会每隔1秒闪烁一次；当退出smartconfig，Wi-Fi连接失败则会快速闪烁，Wi-Fi连接成功则保持常亮（输出低电平）。

# 配置

在`include/user_connfig.h`文件可以进行自定义配置：

```
// user_connfig.h
// ...
// 上电使用 smartconfig 进行 wifi 配置
#define SMARTCONFIG_ENABLE

// wifi检查连接方式
// 0 - DISABLE：不检查wifi连接
// 1 - timer mode：定时检查wifi连接
// 2 - event mode：注册事件的方式检查wifi连接
#define WIFI_CHECK_MODE		1

// 是否使用定时器控制wifi状态led灯
// 0 - DISABLE：不使用wifi led状态灯
// 1 - timer mode：使用定时器设置wifi led状态灯
// 2 - esp mode ：使用ESP自带的接口配置wifi led状态的
#define WIFI_LED_MODE	1
```

# 示例

![image](screenshot/SmartConfig-WiFi.jpg)
