/*
 * UART_callback.c
 *
 *  Created on: 17 Dec 2023
 *      Author: russel
 */
#include "main.h"
#include "ring_buffer.h"


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

extern ring_buf_t ring_buf_debug;
extern ring_buf_t ring_buf_modem;

extern volatile uint8_t rx_data_debug;
extern volatile uint8_t rx_data_modem;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
	{
		ring_buf_put(&ring_buf_debug, rx_data_debug);
		HAL_UART_Receive_IT(&huart1, &rx_data_debug, 1);
	}
	else if(huart == &huart2)
	{
		ring_buf_put(&ring_buf_modem, rx_data_modem);
		HAL_UART_Receive_IT(&huart2, &rx_data_modem, 1);
	}
}

