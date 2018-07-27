/*
 * telnet_server.h
 *
 *  Created on: 2018Äê1ÔÂ5ÈÕ
 *      Author: Administrator
 */

#ifndef _TELNET_SERVER_H_
#define _TELNET_SERVER_H_

#include "c_types.h"

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "mem.h"

// include LwIP API
#include "espconn.h"

extern void telnet_init(void);

#endif /* _TELNET_SERVER_H_ */
