/*
 * http_client.h
 */

#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include "ets_sys.h"
#include "os_type.h"

extern void http_client_init(u8* ip, u16 port);
extern void http_client_connect(void);

#endif /* _HTTP_CLIENT_H_ */
