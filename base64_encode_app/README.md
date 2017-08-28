# base64_encode_app

在ESP8266上进行base64编码。移植于Contiki。

# 相关文件

- user/base64.c
- include/base64.h

# 主要接口

## int base64_encode(const u8 inputdata[], u8 outputdata[])

### 参数

- `inputdata`：要编码的字符串
- `outputdata`：编码后的字符串

### 返回

返回为输出字符串的长度。


# 打印示例

```
# ……
base64 encode: 123456
MTIzNDU2
# ……
```
