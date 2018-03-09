// ringbuf.c

#include "ringbuf.h"

/*
 * \brief init a RINGBUF object
 * \param r pointer to a RINGBUF object
 * \param buf pointer to a byte array
 * \param size size of buf
 * \return 1 if successfull, otherwise failed
 */
#if defined(_ESP8266)
ICACHE_FLASH_ATTR
#endif
int16_t RINGBUF_Init(RINGBUF *r, uint8_t* buf, int32_t size) {
	if (r == NULL || buf == NULL || size < 2) {
		return 0;                   // 如果r和buf传入的参数为空、size数组大小小于2，则初始化失败
	}
	r->p_o = r->p_r = r->p_w = buf; // 初始化p_o，p_r，p_w
	r->fill_cnt = 0;                // 初始化fill_cnt
	r->size = size;                 // 初始化size
	return 1;
}
/**
 * \brief put a character into ring buffer
 * \param r pointer to a ringbuf object
 * \param c character to be put
 * \return 1 if successfull, otherwise failed
 */
#if defined(_ESP8266)
ICACHE_FLASH_ATTR
#endif
int16_t RINGBUF_Put(RINGBUF *r, uint8_t c) {
	if (r->fill_cnt >= r->size) {
		os_printf("RINGBUF FULL\n");
		return 0;                  // 如果缓冲区满了，则返回0错误
	}

	r->fill_cnt++;                  // 每写入一个字节，则加1计数
	*r->p_w++ = c;                  // 把数据放入缓冲区，并指向下一个地址
	if (r->p_w >= r->p_o + r->size) // 如果写入指针超过了缓冲区末尾，则
		r->p_w = r->p_o;            // 把指针指向原点，这就是RING的含义
	return 1;
}
/**
 * \brief get a character from ring buffer
 * \param r pointer to a ringbuf object
 * \param c read character
 * \return 1 if successfull, otherwise failed
 */
#if defined(_ESP8266)
ICACHE_FLASH_ATTR
#endif
int16_t RINGBUF_Get(RINGBUF *r, uint8_t* c, int32_t length) {
	int32_t cnt = 0;
	int i;

	if (r->fill_cnt <= 0) {
		return 0;             // 如果缓冲区为空，则返回0
	}
	if (length > r->fill_cnt)
		length = r->fill_cnt;  // 最大只能读取缓冲区拥有数据的长度

	cnt = r->fill_cnt;
	for (i = 0; i < length; i++) {
		r->fill_cnt--;                  // 每读取一个字节，计数减1
		*c = *r->p_r++;                 // 返回数据给*c
		*c++;
		if (r->p_r >= r->p_o + r->size) { // 如果读取指针超过了缓冲区末尾，则
			r->p_r = r->p_o;            // 把指针指向原点
		}
	}
	return 1;
}

/*
 * function: RINGBUF_IsFull
 */
#if defined(_ESP8266)
ICACHE_FLASH_ATTR
#endif
int RINGBUF_IsFull(RINGBUF *r) {
	if (r->fill_cnt >= r->size) {
		return 1;
	}
	return 0;
}

/*
 * function: RINGBUF_Use
 */
#if defined(_ESP8266)
ICACHE_FLASH_ATTR
#endif
int32_t RINGBUF_Use(RINGBUF *r) {
	return r->fill_cnt;
}

