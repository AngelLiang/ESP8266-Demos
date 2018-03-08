/*
 * http_parse.h
 *
 *  Created on: 2018年3月8日
 *      Author: Administrator
 */

#ifndef _HTTP_PARSE_H_
#define _HTTP_PARSE_H_

#include "os_type.h"

uint32_t get_http_body(const char *pdata, char**pOoutBody);
uint32_t get_http_header_value(const char *pdata, const char *pKey,
		char **pOutValueAddr);
uint32_t get_http_header_length(const char *pdata);

#endif /* _HTTP_PARSE_H_ */
