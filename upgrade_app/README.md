# upgrade_app

ESP8266远程更新固件示例。

# 操作步骤

## 代码配置

`user_config.h`中可能需要修改的宏定义：

```C
// user_config.h
// ...
#define WIFI_SSID 		"WIFI_SSID"
#define WIFI_PASSWORD	"WIFI_PASSWORD"

#define HOST	"192.168.0.50"
#define PORT	5000
```



## 生成user1.bin和user2.bin

可以使用`gen_misc.bat/sh`脚本生成，或者修改Makefile脚本后直接编译。

在Makefile大约23行开始：

```
BOOT?=none
APP?=0
SPI_SPEED?=40
SPI_MODE?=QIO
SPI_SIZE_MAP?=4
```

修改配置如下：

```
BOOT?=new
APP?=1
```

编译生成user1.bin，打印如下：

```
...
Supportboot_v1.1 and +
Generate user1.4096.new.4.bin successully in folder bin/upgrade.
boot.bin------------>0x00000
user1.4096.new.4.bin--->0x01000
```

修改为 APP?=2 ，生成user2.bin：
```
...
Support boot_v1.2 and +
Generate user2.4096.new.4.bin successully in folder bin/upgrade.
boot.bin------------>0x00000
user2.4096.new.4.bin--->0x81000
```

## 烧写

本人使用的是4MB（32Mbit） Flash，所以烧写配置如下：

- `blank.bin`：0x7FB000和0x3FE000
- `esp_init_data_default.bin`：0x3FC000
- `boot1_v1.6.bin`：0x0
- `user1.bin`：0x01000
- `user2.bin`：0x81000

`user1.bin`和`user2.bin`可二选一。

## 启动文件服务器

之后把`user1.4096.new.4.bin`和`user2.4096.new.4.bin`放在`python_file_server/file/`文件夹下：

Python3需要安装flask和flask_admin

```
pip3 install flask
pip3 install flask_admin
```

然后运行`file_server.py`：

```
python3 file_server.py
```

启动后访问`http://127.0.0.1:5000/admin/fileadmin/`即可看到`python_file_server/file/`下的文件

![file_server](screenshot/file_server.jpg)

## 升级操作

打开串口连接ESP8266，波特率74880bps或76800bps。

当连接上Wi-Fi获取到IP地址后，发送`request`即请求文件服务器，进行固件升级。

当串口停止打印并输出下面信息之后：

```
...
http_body_handle finish!
tcp_client_discon_cb
```

发送`finish`使用新的固件进行重启：

可以找到下面信息证明使用了新的固件：

`use1.bin`@0x01000：

```
reboot to use1
...
*********************************
user bin:0
SDK version:2.0.0(656edbf)
...
*********************************
```

`use2.bin`@0x81000：

```
reboot to use2
...

*********************************
user bin:1
SDK version:2.0.0(656edbf)
...
*********************************
```

