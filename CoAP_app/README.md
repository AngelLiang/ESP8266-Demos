# CoAP_app

在ESP8266上使用CoAP示例工程。

# 说明

移植于移植于https://github.com/1248/microcoap

移植代码主要部分在`udp_server.c`文件下的`udp_server_recv`函数。

CoAP主要处理流程：收到请求`buf/len`->`coap_parse`->`coap_handle_req`->`coap_build`->生成响应`buf/rsplen`

## endpoint

endpoint在`endpoints.c`文件

```
/* endpoints */
const coap_endpoint_t endpoints[] =
{
  {COAP_METHOD_GET, handle_get_well_known_core, &path_well_known_core, "ct=40"},
  {COAP_METHOD_GET, handle_get_light, &path_light, "ct=0"},
  {COAP_METHOD_PUT, handle_put_light, &path_light, NULL},
  {(coap_method_t)0, NULL, NULL, NULL}
};
```

主要有两个

1. `coap://<IP>/.well-known/core`：GET
2. `coap://<IP>/light`：GET、PUT

# 效果图
 
![CoAP](https://github.com/AngelLiang/ESP8266-Demos/blob/master/img/CoAP.jpg?raw=true)


