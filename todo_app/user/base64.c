/*
 * base64.c
 *
 *  Created on: 2017年8月28日
 *      Author: Administrator
 */

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

#include "base64.h"

/*---------------------------------------------------------------------------*/
static u8 ICACHE_FLASH_ATTR
base64_encode_6bits(u8 c)
{
  if(c <= 25) {
    return c + 'A';
  } else if(c <= 51) {
    return c - 26 + 'a';
  } else if(c <= 61) {
    return c - 52 + '0';
  } else if(c == 62) {
    return '+';
  } else if(c == 63) {
    return '/';
  }
  /* This shouldn't happen because only 6 bits of data should be
     passed to this function. */
  return '=';
}

/*---------------------------------------------------------------------------*/
static void ICACHE_FLASH_ATTR
base64_encode_24bits(const u8 inputdata[], u8 outputdata[], int len)
{
  switch(len) {
  case 0:
    outputdata[0] = outputdata[1] = outputdata[2] = outputdata[3] = '=';
    break;
  case 1:
    outputdata[0] = base64_encode_6bits((inputdata[0] >> 2) & 0x3f);
    outputdata[1] = base64_encode_6bits((((inputdata[0] << 4) & 0x30)));
    outputdata[2] = outputdata[3] = '=';
    break;
  case 2:
    outputdata[0] = base64_encode_6bits((inputdata[0] >> 2) & 0x3f);
    outputdata[1] = base64_encode_6bits((((inputdata[0] << 4) & 0x30) |
					 (inputdata[1] >> 4)) & 0x3f);
    outputdata[2] = base64_encode_6bits((((inputdata[1] << 2) & 0x3f)));
    outputdata[3] = '=';
    break;
  case 3:
  default:
    outputdata[0] = base64_encode_6bits((inputdata[0] >> 2) & 0x3f);
    outputdata[1] = base64_encode_6bits((((inputdata[0] << 4) & 0x30) |
					 (inputdata[1] >> 4)) & 0x3f);
    outputdata[2] = base64_encode_6bits((((inputdata[1] << 2) & 0x3c) |
					 (inputdata[2] >> 6)) & 0x3f);
    outputdata[3] = base64_encode_6bits((inputdata[2]) & 0x3f);
    break;
  }
}

/*
 * function: base64_encode
 * parameter: u8 inputdata[]
 * 			  u8 outputdata[]
 * return: int - 输出的outputdata长度
 */
int ICACHE_FLASH_ATTR
base64_encode(const u8 inputdata[], u8 outputdata[])
{
	int len;
	int i, j;

	len = os_strlen(inputdata);
	j = 0;
	for(i = 0; i < len; i += 3) {
		base64_encode_24bits(&inputdata[i], &outputdata[j], len - i);
		j += 4;
	}
	return len;
}
