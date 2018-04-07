# cJSON_app

移植cJSON到ESP8266，目前使用的cJSON版本是1.6.0。

由于ESP8266不支持浮点数，在ESP8266中如果有浮点数则进行截断处理，总之不要在ESP8266中使用浮点数。

效果图

![ESP826-cJSON-demo](screenshot/cJSON.jpg)

相关博客：http://blog.csdn.net/yannanxiu/article/details/52713746


# 字符串解析为cJSON示例

```
// ...
u8* data = "{\"string\":\"hello world\"}";
cJSON * root = cJSON_Parse(data);
if(NULL!=root){
    if(cJSON_HasObjectItem(root, "string")){		// 是否有对应的item
        cJSON *string = cJSON_GetObjectItem(root, "string");	// 获取item
        if (cJSON_IsString(string)){			// 判断item的value是否是string类型
            char *s = cJSON_Print(string);		// 渲染item的value为字符串
            os_printf("string: %s\r\n", s);
            cJSON_free((void *)s);				// 记得要释放
        }
    }
    cJSON_Delete(root);
}else{
    os_printf("\r\nparse error!\r\n");
}
```

生成JSON示例请参考`cJSON_test.c`测试代码。
