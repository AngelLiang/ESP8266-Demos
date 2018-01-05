# lwip_open_src_template_proj

SDK中自带的lwip示例工程

user文件夹下源代码说明

- `tcp_server.c`：基于SDK提供的API编写的TCP Server示例；
- `udp_server.c`：基于SDK提供的API编写的UDP Server示例；
- `lwip_demo.c`：基于lwip的raw/callback API的示例代码，包括TCP/UDP Server；
- `telnet_server.c`：基于lwip的raw/callback API编写的Tenlet Server示例；

![Tenlet Server](screenshot/tenlet_server.png)

# lwip输出调试打印步骤

1. 在`include/arch/cc.h`中76行解除`#define LWIP_DEBUG`的注释；
2. 在`include/lwipopts.h`中设置`LWIP_DBG_TYPES_ON`为`LWIP_DBG_ON`；
3. 之后在`include/lwipopts.h`中1808行开始使能需要调试的模块；

	比如开启TCP调试打印：1945行把`TCP_DEBUG`设置为`LWIP_DBG_ON`

另，开启调试后重新编译要等比较久！！！


