#include "main.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ring_buffer.h"
#include "gsm.h"
#include "at_common.h"


#define DEBUG_P

extern UART_HandleTypeDef huart2;
extern ring_buf_t ring_buf_debug;
extern ring_buf_t ring_buf_modem;
extern uint8_t response_buffer[100];

static void gsm_get_command_header(char * command, char *out);

int send_command(char * command)
{
	HAL_StatusTypeDef res_uart;
	int i = 0;

	while(command[i] != '\0')
	{
		res_uart = HAL_UART_Transmit(&huart2, &command[i], 1, HAL_MAX_DELAY);
		++i;
	}

	return res_uart;
}


/**
 * @brief
 *     Will search until the type of response was found and
 *     until the result was found or an ERROR was received.
 *
 *
 * @note
 *
 * @param[in] command
 * 		AT+Command..\r\n. Must be terminated with null character
 *
 * @param[in] out_buf
 * 		Storage when the response type argument is not Result Code
 *
 * @param[in]
 *
 * @param[in] header
 * 		Format is +HEADER terminated with null character
 * 		Ex. +CPIN
 *
 */

int send_data_check_result_and_response(char * command, char * out_buf, resp_type_t type, char * header)
{
	resp_type_t line_type = MAX_RESP_TYPE;
	int result = 0;

	uint8_t result_buffer[20];

	HAL_Delay(500);	 // Wait to check for URC

	while(!ring_buf_is_empty(&ring_buf_modem))
	{
		//
		printf("URC\r\n");
		//printf("%s", ring_buf_modem);
		ring_buf_init_contents(&ring_buf_modem);
	}

	send_command(command);

#ifdef DEBUG_P
	// Print to Debug Port
	printf("%s", command);
#endif

	bool is_result_found = false;
	bool is_response_found = false;
	bool is_result_error = false;

	// Get each line
	while((!is_result_found || !is_response_found) && !is_result_error)
	{
		while(!ring_buf_is_empty(&ring_buf_modem))
		{
			bool is_finished_getting_line = false;

			uint8_t data;
			data = ring_buf_get(&ring_buf_modem);

			is_finished_getting_line = get_response_line(data, response_buffer);

			if(is_finished_getting_line)
			{

			  	line_type = get_resp_type(response_buffer, header);

			  	if(line_type == type)
			  	{
			  		strcpy(out_buf, response_buffer);
			  		is_response_found = true;
			  	}

			  	// Some command does not response "OK" when successful,
			  	// And if unsuccessful, they will respond error
			  	if(line_type == RESP_TYPE_RESULT_CODE)
			  	{
			  		is_result_found = true;
			  		strcpy(result_buffer, response_buffer);
			  	}

			  	if(is_result_found)
			  	{
			  		if(strstr((char*)result_buffer, "ERROR") != NULL)
			  		{
			  			is_result_error = true;
			  		}
			  	}


			  	// Print to Debug Port
#ifdef DEBUG_P
			  	printf("%s\r\n", response_buffer);
#endif
			 }
		}
	}


	if(strstr((char*)result_buffer, "OK") != NULL)
	{
		result = 1;
	}
	else
	{
		result = 0;
	}

	return result;
}

int send_command_check_result_and_response(char * command, char * out_buf, resp_type_t type)
{
	int result;

	char header[100];

	gsm_get_command_header(command, header);

	result = send_data_check_result_and_response(command, out_buf, type, header);

	return result;
}


/*
 *  Will jut get the first line response. And will
 *  detect if command produces an ERROR response
 */
int send_command_check_response(char * command, char * out_buf)
{
	bool is_finished_getting_line = false;
	bool is_result_error = false;
	bool is_command_successful = false;

	HAL_Delay(500);	 // Wait to check for URC

	while(!ring_buf_is_empty(&ring_buf_modem))
	{
			//
		printf("URC\r\n");
		//printf("%s", ring_buf_modem);
		ring_buf_init_contents(&ring_buf_modem);
	}

	send_command(command);

#ifdef DEBUG_P
	// Print to Debug Port
	printf("%s", command);
#endif


	// Get each line
	while((!is_finished_getting_line) && !is_result_error)
	{
		while(!ring_buf_is_empty(&ring_buf_modem))
		{
			uint8_t data;
			data = ring_buf_get(&ring_buf_modem);

			is_finished_getting_line = get_response_line(data, response_buffer);

			if(is_finished_getting_line)
			{
			  	(void)get_resp_type(response_buffer, NULL);
			  	strcpy(out_buf, response_buffer);

			  	// Print to Debug Port
#ifdef DEBUG_P
			  	printf("%s\r\n", response_buffer);
#endif

			  	if(strstr((char*)out_buf, "ERROR") != NULL)
			  	{
			  		is_result_error = true;
			  	}

			}
		}
	}

	if(is_result_error == true)
	{
		is_command_successful = false;
	}
	else
	{
		is_command_successful = true;
	}

	return is_command_successful;
}


int send_command_check_result(char *command)
{
	char out_buf[50];
	int result;

	result = send_command_check_response(command, out_buf);

	return result;
}


void gsm_remove_response_header(char * response, char *out)
{

	enum
	{
		START,
		CONTENT,
		END,
	};

	bool is_header_found = false;
	int state = START;
	int i = 0;

	while(is_header_found == false)
	{
		switch(state)
		{
			case START:

				if(response[i] == '+')
				{
					state = CONTENT;
				}

				break;

			case CONTENT:

				if(response[i] == ' ')
				{
					state = END;
				}
				
				break;

			case END:

				is_header_found = true;

				break;

			default:
				break;
		}

		++i;
	}

	
	strcpy(out, &response[i]);
}


static void gsm_get_command_header(char * command, char *out)
{
	// if char is +
	// 	start saving the succeeding character
	//  if char is ? or = \r
	// 		stop searching

	enum
	{
		START,
		CONTENT,
		END,
	};

	bool is_header_found = false;
	int state = START;
	int i = 0;
	int j = 0;

	while(is_header_found == false)
	{
		switch(state)
		{
			case START:

				if(command[i] == '+')
				{
					state = CONTENT;
				}

				break;

			case CONTENT:

				if((command[i] == '?') || (command[i] == '=') || (command[i] == '\r'))
				{
					state = END;
				}
				else
				{
					out[j] = command[i];
					++j;
				}

				break;

			case END:

				is_header_found = true;

				break;

			default:
				break;
		}

		++i;
	}

	out[j] = '\0';
}



