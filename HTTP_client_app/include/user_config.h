/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define AP_CACHE           0

#if AP_CACHE
#define AP_CACHE_NUMBER    5
#endif

#define WIFI_SSID 		"WIFI_SSID"
#define WIFI_PASSWORD	"WIFI_PASSWORD"

// baidu
// baidu
#define HOST	"119.75.217.109"
#define PORT	80

#define FILE_URL "/"
#define _HTTP_HEADER_HOST	"Host: 119.75.217.109:80\r\n"
#define HTTP_REQUEST "GET "FILE_URL" HTTP/1.0\r\n"_HTTP_HEADER_HOST"\r\n"

// 是否使用SSL
#define SSL_CLIENT_ENABLE		0

#if SSL_CLIENT_ENABLE
#define SSL_CLIENT_KEY_ADDR		0x9A
#define SSL_CA_ADDR				0x9B
#endif

#endif

