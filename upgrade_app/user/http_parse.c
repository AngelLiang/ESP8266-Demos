/*
 * http_parse.c
 */

#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "espconn.h"
#include "stdlib.h"

//#define DEBUG

#ifdef DEBUG
#define debug(fmt, args...) os_printf(fmt, ##args)
#define debugX(level, fmt, args...) if(DEBUG>=level) os_printf(fmt, ##args);
#else
#define debug(fmt, args...)
#define debugX(level, fmt, args...)
#endif /* DEBUG */

/*
 * 获取http header的长度及获取http body开始位置
 * function: get_http_body
 * parameter: const char *pdata - HTTP header string.
 *            char**pOoutBody - HTTP body out pointer.
 * return: uint32_t - http header length, =0 if error.
 */
uint32_t ICACHE_FLASH_ATTR
get_http_body(const char *pdata, char**pOoutBody) {
	char *pEnd = (char *) os_strstr(pdata, "\r\n\r\n");
	if (NULL == pEnd) {
		return 0;
	}
	pEnd += 4;
	debug("%s\r\n");
	*pOoutBody = pEnd;
	return pEnd - pdata;
}

/*
 * 获取http header某个key的value
 * function: get_http_header_value
 * parameter: const char *pdata - HTTP header string.
 *            const char *pKey - the header string.
 *            char **pOutValueAddr - the addr of the value.
 * return: uint32_t - value length, =0 if error.
 */
uint32_t ICACHE_FLASH_ATTR
get_http_header_value(const char *pdata, const char *pKey, char **pOutValueAddr) {
	// 寻找key
	char *pHeader = (char *) os_strstr(pdata, pKey);

	if (NULL == pHeader) {
		return 0;
	}
	// 寻找结尾
	char *pEnd = (char *) os_strstr(pHeader, "\r\n");
	if (NULL == pEnd) {
		return 0;
	}

	debug("%d - %d = %d\r\n", pEnd, pHeader, pEnd - pHeader);

	// 寻找冒号
	char *pColon = (char *) os_strchr(pHeader, ':');
	if (NULL == pColon) {
		return 0;
	}
	pColon++; // 跳过冒号
	debug("pColon:%s\r\n", pColon);
	// 获取Value
	char *pStart = pColon;
	while (' ' == *pStart) {
		pStart++;
	}debug("%d - %d = %d\r\n", pEnd, pStart, pEnd - pStart);
	*pOutValueAddr = pStart;
	return pEnd - pStart;
}

/*
 * 获取 http header 中 Content-Length 的值
 * function: get_http_header_length
 * parameter: const char *pdata - HTTP header string.
 * return: uint32_t - http body length, =0 if error.
 */
uint32_t ICACHE_FLASH_ATTR
get_http_header_length(const char *pdata) {
	char value_arr[16] = { '\0' };
	char *pValueAddr = NULL;
	u32 ret_len = 0;
	ret_len = get_http_header_value(pdata, "Content-Length", &pValueAddr);
	debug("ret_len:%d\r\n", ret_len);debug("pValueAddr:%d\r\n", pValueAddr);
	if (ret_len > 0 && NULL != pValueAddr) {
		os_strncpy(value_arr, pValueAddr, ret_len);
		debug("value_arr:%s\r\n", value_arr);
		return atoi(value_arr);
	}
	return 0;
}
