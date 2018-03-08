#ifndef _RINGBUF_H_
#define _RINGBUF_H_

#include "os_type.h"

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
extern int32_t RINGBUF_DataSize(RINGBUF *r);

#endif
