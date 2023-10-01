#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "stm32f1xx_hal.h"

#include "ring_buffer.h"


static void increment_count(ring_buf_t * p_ring_buf);
static void decrement_count(ring_buf_t * p_ring_buf);
static void wrap_around_index_check(ring_buf_t * p_ring_buf);


void ring_buf_init(ring_buf_t * p_ring_buf)
{
	p_ring_buf->head = 0U;
	p_ring_buf->tail = 0U;
	p_ring_buf->count = 0U;

	memset(p_ring_buf->buf, 0U, sizeof(p_ring_buf->buf));
}


void ring_buf_put(ring_buf_t * p_ring_buf, uint8_t data)
{
	// Overwrite the existing data with new data
	// when buffer is full.

	if(p_ring_buf->count < BUF_SIZE)
	{
		increment_count(p_ring_buf);
	}
	else
	{
		p_ring_buf->count = 0U;
	}

	p_ring_buf->buf[p_ring_buf->head] = data;
	++(p_ring_buf->head);

	wrap_around_index_check(p_ring_buf);
}


int ring_buf_get(ring_buf_t * p_ring_buf)
{
	uint8_t data;

	if(p_ring_buf->count > 0U)
	{
		data = p_ring_buf->buf[p_ring_buf->tail];
		++(p_ring_buf->tail);

		decrement_count(p_ring_buf);
		wrap_around_index_check(p_ring_buf);
	}
	else
	{

	}

	return data;
}


bool ring_buf_is_empty(ring_buf_t * p_ring_buf)
{
	bool is_empty = false;

	if(p_ring_buf->count == 0)
	{
		is_empty = true;
	}
	else
	{
		is_empty = false;
	}

	return is_empty;
}


bool ring_buf_is_full(ring_buf_t * p_ring_buf)
{
	bool is_full = false;

	if(p_ring_buf->count == BUF_SIZE)
	{
		is_full = true;
	}
	else
	{
		is_full = false;
	}

	return is_full;
}

bool ring_buf_num_contents(ring_buf_t * p_ring_buf)
{
	return p_ring_buf->count;
}

static void wrap_around_index_check(ring_buf_t * p_ring_buf)
{
	if(p_ring_buf->tail > (BUF_SIZE - 1U))
	{
		p_ring_buf->tail = 0U;
	}
}


static void increment_count(ring_buf_t * p_ring_buf)
{
	uint32_t masking_tate;
	masking_tate = __get_PRIMASK();
	__disable_irq();

	++(p_ring_buf->count);

	__set_PRIMASK(masking_tate);
}


static void decrement_count(ring_buf_t * p_ring_buf)
{
	uint32_t masking_tate;
	masking_tate = __get_PRIMASK();
	__disable_irq();

	--(p_ring_buf->count);

	__set_PRIMASK(masking_tate);
}












