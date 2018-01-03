/*
 * tcp_server.h
 *
 *  Created on: 2017Äê7ÔÂ3ÈÕ
 *      Author: Administrator
 */

#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include "c_types.h"

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "mem.h"

// include LwIP API
#include "espconn.h"

/* TCP Server init */
extern void tcp_server_init(u32 port);

#endif /* _TCP_SERVER_H_ */

