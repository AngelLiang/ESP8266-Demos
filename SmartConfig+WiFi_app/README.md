# SmartConfig+WiFi_app

上电后先使用smartconfig进行配网，一段时候后没有收到配网消息则尝试连接上次成功的Wi-Fi。

工程设置了GPIO0为Wi-Fi状态灯。进入smartconfig模式的`SC_STATUS_FIND_CHANNEL`状态后，状态灯会每隔1秒闪烁一次；当退出smartconfig，Wi-Fi连接失败则会快速闪烁，Wi-Fi连接成功则保持常亮（低电平）。

