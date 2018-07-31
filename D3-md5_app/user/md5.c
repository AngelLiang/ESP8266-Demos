/*
 * filename: md5.c
 */

#include "ets_sys.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"

#define MEMCPY	os_memcpy
#define STRLEN	os_strlen
#define STRNCMP os_strncmp
#define MEMCMP os_memcmp

#define MEMSET	os_memset
#define STRNCPY	os_strncpy
#define MALLOC	os_malloc
#define FREE	os_free

#include "md5.h"

#define MD5_TEST

/****************************************************************/
#define DEBUG

#ifdef DEBUG
#define debug(fmt, args...) os_printf(fmt, ##args)
#define debugX(level, fmt, args...) if(DEBUG>=level) os_printf(fmt, ##args);
#else
#define debug(fmt, args...)
#define debugX(level, fmt, args...)
#endif /* DEBUG */

/****************************************************************/

unsigned char PADDING[] = { 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void ICACHE_FLASH_ATTR
MD5Init(MD5_CTX *context) {
	context->count[0] = 0;
	context->count[1] = 0;
	context->state[0] = 0x67452301;
	context->state[1] = 0xEFCDAB89;
	context->state[2] = 0x98BADCFE;
	context->state[3] = 0x10325476;
}

void ICACHE_FLASH_ATTR
MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputlen) {
	unsigned int i = 0, index = 0, partlen = 0;
	index = (context->count[0] >> 3) & 0x3F;
	partlen = 64 - index;
	context->count[0] += inputlen << 3;
	if (context->count[0] < (inputlen << 3))
		context->count[1]++;
	context->count[1] += inputlen >> 29;

	if (inputlen >= partlen) {
		MEMCPY(&context->buffer[index], input, partlen);
		MD5Transform(context->state, context->buffer);
		for (i = partlen; i + 64 <= inputlen; i += 64)
			MD5Transform(context->state, &input[i]);
		index = 0;
	} else {
		i = 0;
	}
	MEMCPY(&context->buffer[index], &input[i], inputlen - i);
}

void ICACHE_FLASH_ATTR
MD5Final(MD5_CTX *context, unsigned char digest[16]) {
	unsigned int index = 0, padlen = 0;
	unsigned char bits[8];
	index = (context->count[0] >> 3) & 0x3F;
	padlen = (index < 56) ? (56 - index) : (120 - index);
	MD5Encode(bits, context->count, 8);
	MD5Update(context, PADDING, padlen);
	MD5Update(context, bits, 8);
	MD5Encode(digest, context->state, 16);
}
void ICACHE_FLASH_ATTR
MD5Encode(unsigned char *output, unsigned int *input, unsigned int len) {
	unsigned int i = 0, j = 0;
	while (j < len) {
		output[j] = input[i] & 0xFF;
		output[j + 1] = (input[i] >> 8) & 0xFF;
		output[j + 2] = (input[i] >> 16) & 0xFF;
		output[j + 3] = (input[i] >> 24) & 0xFF;
		i++;
		j += 4;
	}
}
void ICACHE_FLASH_ATTR
MD5Decode(unsigned int *output, unsigned char *input, unsigned int len) {
	unsigned int i = 0, j = 0;
	while (j < len) {
		output[i] = (input[j]) | (input[j + 1] << 8) | (input[j + 2] << 16)
				| (input[j + 3] << 24);
		i++;
		j += 4;
	}
}

void ICACHE_FLASH_ATTR
MD5Transform(unsigned int state[4], unsigned char block[64]) {
	unsigned int a = state[0];
	unsigned int b = state[1];
	unsigned int c = state[2];
	unsigned int d = state[3];
	unsigned int x[64];
	MD5Decode(x, block, 64);
	FF(a, b, c, d, x[0], 7, 0xd76aa478);
	FF(d, a, b, c, x[1], 12, 0xe8c7b756);
	FF(c, d, a, b, x[2], 17, 0x242070db);
	FF(b, c, d, a, x[3], 22, 0xc1bdceee);
	FF(a, b, c, d, x[4], 7, 0xf57c0faf);
	FF(d, a, b, c, x[5], 12, 0x4787c62a);
	FF(c, d, a, b, x[6], 17, 0xa8304613);
	FF(b, c, d, a, x[7], 22, 0xfd469501);
	FF(a, b, c, d, x[8], 7, 0x698098d8);
	FF(d, a, b, c, x[9], 12, 0x8b44f7af);
	FF(c, d, a, b, x[10], 17, 0xffff5bb1);
	FF(b, c, d, a, x[11], 22, 0x895cd7be);
	FF(a, b, c, d, x[12], 7, 0x6b901122);
	FF(d, a, b, c, x[13], 12, 0xfd987193);
	FF(c, d, a, b, x[14], 17, 0xa679438e);
	FF(b, c, d, a, x[15], 22, 0x49b40821);

	GG(a, b, c, d, x[1], 5, 0xf61e2562);
	GG(d, a, b, c, x[6], 9, 0xc040b340);
	GG(c, d, a, b, x[11], 14, 0x265e5a51);
	GG(b, c, d, a, x[0], 20, 0xe9b6c7aa);
	GG(a, b, c, d, x[5], 5, 0xd62f105d);
	GG(d, a, b, c, x[10], 9, 0x2441453);
	GG(c, d, a, b, x[15], 14, 0xd8a1e681);
	GG(b, c, d, a, x[4], 20, 0xe7d3fbc8);
	GG(a, b, c, d, x[9], 5, 0x21e1cde6);
	GG(d, a, b, c, x[14], 9, 0xc33707d6);
	GG(c, d, a, b, x[3], 14, 0xf4d50d87);
	GG(b, c, d, a, x[8], 20, 0x455a14ed);
	GG(a, b, c, d, x[13], 5, 0xa9e3e905);
	GG(d, a, b, c, x[2], 9, 0xfcefa3f8);
	GG(c, d, a, b, x[7], 14, 0x676f02d9);
	GG(b, c, d, a, x[12], 20, 0x8d2a4c8a);

	HH(a, b, c, d, x[5], 4, 0xfffa3942);
	HH(d, a, b, c, x[8], 11, 0x8771f681);
	HH(c, d, a, b, x[11], 16, 0x6d9d6122);
	HH(b, c, d, a, x[14], 23, 0xfde5380c);
	HH(a, b, c, d, x[1], 4, 0xa4beea44);
	HH(d, a, b, c, x[4], 11, 0x4bdecfa9);
	HH(c, d, a, b, x[7], 16, 0xf6bb4b60);
	HH(b, c, d, a, x[10], 23, 0xbebfbc70);
	HH(a, b, c, d, x[13], 4, 0x289b7ec6);
	HH(d, a, b, c, x[0], 11, 0xeaa127fa);
	HH(c, d, a, b, x[3], 16, 0xd4ef3085);
	HH(b, c, d, a, x[6], 23, 0x4881d05);
	HH(a, b, c, d, x[9], 4, 0xd9d4d039);
	HH(d, a, b, c, x[12], 11, 0xe6db99e5);
	HH(c, d, a, b, x[15], 16, 0x1fa27cf8);
	HH(b, c, d, a, x[2], 23, 0xc4ac5665);

	II(a, b, c, d, x[0], 6, 0xf4292244);
	II(d, a, b, c, x[7], 10, 0x432aff97);
	II(c, d, a, b, x[14], 15, 0xab9423a7);
	II(b, c, d, a, x[5], 21, 0xfc93a039);
	II(a, b, c, d, x[12], 6, 0x655b59c3);
	II(d, a, b, c, x[3], 10, 0x8f0ccc92);
	II(c, d, a, b, x[10], 15, 0xffeff47d);
	II(b, c, d, a, x[1], 21, 0x85845dd1);
	II(a, b, c, d, x[8], 6, 0x6fa87e4f);
	II(d, a, b, c, x[15], 10, 0xfe2ce6e0);
	II(c, d, a, b, x[6], 15, 0xa3014314);
	II(b, c, d, a, x[13], 21, 0x4e0811a1);
	II(a, b, c, d, x[4], 6, 0xf7537e82);
	II(d, a, b, c, x[11], 10, 0xbd3af235);
	II(c, d, a, b, x[2], 15, 0x2ad7d2bb);
	II(b, c, d, a, x[9], 21, 0xeb86d391);
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
}

/*
 * function: MD5Digest
 * parameter: void const *strContent - 需要MD5的字符串
 *            u16 iLength - 字符串长度
 *            u8 output[16] - 输出
 * description: 生成MD5摘要
 */
void ICACHE_FLASH_ATTR
MD5Digest(void const *strContent, u16 iLength, u8 output[16]) {
	u8 *q = (u8*) strContent;

	MD5_CTX md5;
	MD5Init(&md5);
	MD5Update(&md5, q, iLength);
	MD5Final(&md5, output);
}

/*
 * function: HMAC_MD5
 * parameter: u8 *inBuffer - 需要加密的字符串
 *            u16 len - 字符串长度
 *            u8 *ky - 初始密钥
 *            u8 output[16] - 输出
 */
void ICACHE_FLASH_ATTR
HMAC_MD5(u8 *inBuffer, u16 len, u8 *ky, u8 output[16]) {
	int i, j;

	u8 *tempBuffer = (u8 *) MALLOC(len + 64); //第一次HASH的参数
	u8 Buffer2[80]; //第二次HASH

	u8 key[64];
	u8 ipad[64], opad[64];

	MEMSET(key, 0, 64);

	if (os_strlen(ky) > 64) {
		MD5Digest(ky, STRLEN(ky), key);
	} else {
		STRNCPY(key, ky, 64);
	}

	for (i = 0; i < 64; i++) {
		ipad[i] = 0x36;
		opad[i] = 0x5c;
	}

	for (i = 0; i < 64; i++) {
		ipad[i] = key[i] ^ ipad[i];   //K ⊕ ipad
		opad[i] = key[i] ^ opad[i];   //K ⊕ opad
	}

	for (i = 0; i < 64; i++) {
		tempBuffer[i] = ipad[i];
	}

	for (i = 64; i < len + 64; i++) {
		tempBuffer[i] = inBuffer[i - 64];
	}

	MD5Digest(tempBuffer, len + 64, output);

	for (j = 0; j < 64; j++) {
		Buffer2[j] = opad[j];
	}

	for (i = 64; i < 80; i++) {
		Buffer2[i] = output[i - 64];
	}

	MD5Digest(Buffer2, 80, output);

	FREE(tempBuffer);
}

#ifdef MD5_TEST

void ICACHE_FLASH_ATTR
md5_test(void) {
	int i;
	u8 encrypt[] = "admin";
	u8 decrypt[16];

	MD5Digest(encrypt, STRLEN((char *) encrypt), decrypt);

	debug("md5 test\r\n");
	debug("*********************************\r\n");
	debug("before encrypt:%s\r\nafter encrypt 16bit:", encrypt);
	for (i = 4; i < 12; i++) {
		debug("%02x", decrypt[i]);	// 7a57a5a743894a0e
	}

	debug("\r\nbefore encrypt:%s\r\nafter encrypt 32bit:", encrypt);
	for (i = 0; i < 16; i++) {
		debug("%02x", decrypt[i]);	// 21232f297a57a5a743894a0e4a801fc3
	}
	debug("\r\n");
	debug("*********************************\r\n");

	debug("hmacmd5 test\r\n");
	u8 output[16];
	HMAC_MD5("", 0, "", output);	// 74e6f7298a9c2d168935f58c001bad88
	debug("Digest: ");
	for (i = 0; i < 16; i++) {
		if (output[i] < 0x10) {
			debug("%d", 0);
		}
		debug("%x", output[i]);
	}
	debug("\n");

	// b9092bfe47f21e2930a864b457f7c26d
	u8 *key = "key";
	HMAC_MD5(encrypt, STRLEN((char *) encrypt), key, output);
	debug("Digest: ");
	for (i = 0; i < 16; i++) {
		if (output[i] < 0x10) {
			debug("%d", 0);
		}
		debug("%x", output[i]);
	}
	debug("\n");

	// 超过16个字节的key
	// e5d88b25c472411491dced7d1f0e42d3
	u8 *key16 = "12345678901234567";
	HMAC_MD5(encrypt, STRLEN((char *) encrypt), key16, output);
	debug("Digest 16: ");
	for (i = 0; i < 16; i++) {
		if (output[i] < 0x10) {
			debug("%d", 0);
		}
		debug("%x", output[i]);
	}
	debug("\n");

	// 超过64个字节的key
	// 4a646bce9962a8953c4a88e9af844907
	u8 *key64 = "12345678901234567890123456789012345678901234567890123456789012345";
	HMAC_MD5(encrypt, STRLEN((char *) encrypt), key64, output);
	debug("Digest 64: ");
	for (i = 0; i < 16; i++) {
		if (output[i] < 0x10) {
			debug("%d", 0);
		}
		debug("%x", output[i]);
	}
	debug("\n");

}

#endif
