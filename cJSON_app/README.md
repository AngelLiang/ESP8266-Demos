# cJSON_app

在ESP8266中使用cJSON，目前cJSON最新版本是1.5.3。

由于ESP8266不支持浮点数，修改了cJSON中有关double的函数。

效果图

![ESP826-cJSON-demo](https://github.com/AngelLiang/ESP8266-Demos/blob/master/img/cJSON.jpg)

相关博客：http://blog.csdn.net/yannanxiu/article/details/52713746

字符串解析成JSON示例

```
u8* pdata = "{\"hello\":\"world\"}";
cJSON *root = cJSON_Parse(pdata);
print_preallocated(root);
cJSON *hello = cJSON_GetObjectItemCaseSensitive(root, "hello");
if (cJSON_IsString(hello)){
    os_printf("hello %s!\r\n", hello->valuestring); // 打印「hello world!」
}
cJSON_Delete(root);
```

生成JSON示例请参考cJSON_text.c文件里的代码。
