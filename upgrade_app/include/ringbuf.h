#ifndef _RINGBUF_H_
#define _RINGBUF_H_

//#define _WIN32
#define _ESP8266

#define DEBUG

//***********************************************************************/

#ifdef _WIN32

#elif defined(_ESP8266)

#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "espconn.h"

#endif

#ifdef DEBUG
	#ifdef _WIN32
		#define debug(fmt, args...) printf(fmt, ##args)
		#define debugX(level, fmt, args...) if(DEBUG>=level) printf(fmt, ##args);
	#elif defined(__linux__)
		#define debug(fmt, args...) printf(fmt, ##args)
		#define debugX(level, fmt, args...) if(DEBUG>=level) printf(fmt, ##args);
	#elif defined(_ESP8266)
		#define debug(fmt, args...) os_printf(fmt, ##args)
		#define debugX(level, fmt, args...) if(DEBUG>=level) os_printf(fmt, ##args);
	#endif
#else
	#define debug(fmt, args...)
	#define debugX(level, fmt, args...)
#endif /* DEBUG */

//***********************************************************************/

typedef struct {
	uint8_t* p_o; /**< Original pointer */
	uint8_t* volatile p_r; /**< Read pointer */
	uint8_t* volatile p_w; /**< Write pointer */
	int32_t volatile fill_cnt; /**< Number of filled slots */
	int32_t size; /**< Buffer size */
} RINGBUF;

extern int16_t RINGBUF_Init(RINGBUF *r, uint8_t* buf, int32_t size);
extern int16_t RINGBUF_Put(RINGBUF *r, uint8_t c);
extern int16_t RINGBUF_Get(RINGBUF *r, uint8_t* c, int32_t length);
extern int RINGBUF_IsFull(RINGBUF *r);
extern int32_t RINGBUF_Use(RINGBUF *r);

#endif
