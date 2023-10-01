#include <stdint.h>
#include <stdbool.h>

#include "stm32f1xx_hal.h"

#include "ring_buffer.h"


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
	// when buffer is full
	uint32_t masking_tate;

	if(p_ring_buf->count == BUF_SIZE)
	{
		p_ring_buf->count = 0U;
	}
	else
	{
		masking_tate = __get_PRIMASK();
		__disable_irq();

		++(p_ring_buf->count);

		__set_PRIMASK(masking_tate);
	}

	p_ring_buf->buf[p_ring_buf->head] = data;
	++(p_ring_buf->head);

	if(p_ring_buf->head > (BUF_SIZE - 1U))
	{
		p_ring_buf->head = 0U;
	}



}

int ring_buf_get(ring_buf_t * p_ring_buf)
{
	uint8_t data;
	uint32_t masking_tate;

	if(p_ring_buf->count == 0U)
	{

	}
	else
	{
		data = p_ring_buf->buf[p_ring_buf->tail];
		++(p_ring_buf->tail);

		masking_tate = __get_PRIMASK();
		__disable_irq();

		--(p_ring_buf->count);

		__set_PRIMASK(masking_tate);


		if(p_ring_buf->tail > (BUF_SIZE - 1U))
		{
			p_ring_buf->tail = 0U;
		}
	}

	return data;
}
