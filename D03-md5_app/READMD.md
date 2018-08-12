# md5_app

ESP8266使用MD5和HmacMD5方式对数据进行hash。

相关文件：`user/md5.c`和`include/md5.h`

# 调试信息

上电后会打印如下信息：

```
md5 test
*********************************
before encrypt:admin
after encrypt 16bit:7a57a5a743894a0e
before encrypt:admin
after encrypt 32bit:21232f297a57a5a743894a0e4a801fc3
*********************************
hmacmd5 test
Digest: 74e6f7298a9c2d168935f58c001bad88
Digest: b9092bfe47f21e2930a864b457f7c26d
```

# 相关接口

```C
/*
 * function: MD5Digest
 * parameter: void const *strContent - 需要MD5的字符串
 *            u16 iLength - 字符串长度
 *            u8 output[16] - 输出
 * description: 生成MD5摘要
 */
void MD5Digest(void const *strContent, u16 iLength, u8 output[16]);

/*
 * function: HMAC_MD5
 * parameter: u8 *inBuffer - 需要加密的字符串
 *            u16 len - 字符串长度
 *            u8 *ky - 初始密钥
 *            u8 output[16] - 输出
 */
void HMAC_MD5(u8 *inBuffer, u16 len, u8 *ky, u8 output[16]);
```
