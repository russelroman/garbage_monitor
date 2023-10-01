#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>

#define BUF_SIZE 1000U

typedef struct ring_buf
{
	uint8_t buf[BUF_SIZE];
	uint32_t head;
	uint32_t tail;
	uint32_t count;
} ring_buf_t;

void ring_buf_init(ring_buf_t * p_ring_buf);
void ring_buf_put(ring_buf_t * p_ring_buf, uint8_t data);
int ring_buf_get(ring_buf_t * p_ring_buf);
bool ring_buf_is_empty(ring_buf_t * p_ring_buf);
bool ring_buf_is_full(ring_buf_t * p_ring_buf);
bool ring_buf_num_contents(ring_buf_t * p_ring_buf);

#endif /* RING_BUFFER_H */
