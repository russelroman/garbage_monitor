#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "stddef.h"

#include "at_common.h"


typedef enum
{
  GET_RESPONSE_LINE_IDLE,
  GET_RESPONSE_LINE_START,
  GET_RESPONSE_LINE_PAYLOAD,
  GET_RESPONSE_LINE_END
} get_line_state_t;


/**
 *  @brief
 *  	Get a response line and remove carriage return
 *  	and newline characters.
 *
 *	@details
 *
 *
 *	@param[in] data
 *		ASCII Character.
 *
 *	@param[out] out_buf
 *		Buffer on which the characters of the line response
 *		will be stored.
 *
 *	@return  is_finished_getting_line
 *
 */
bool get_response_line(char data, char * out_buf)
{
  static uint8_t state = GET_RESPONSE_LINE_IDLE;
  static uint32_t pointer = 0;
  bool is_finished_getting_line = false;

  switch(state)
  {
    case GET_RESPONSE_LINE_IDLE:

      if(data == '\r')
      {
        state = GET_RESPONSE_LINE_START;
      }

      break;

    case GET_RESPONSE_LINE_START:

      if(data == '\n')
      {
        state = GET_RESPONSE_LINE_PAYLOAD;
      }

      break;

    case GET_RESPONSE_LINE_PAYLOAD:

      if(data != '\r')
      {
        out_buf[pointer] = data;
        ++pointer;
      }
      else
      {
        state = GET_RESPONSE_LINE_END;
      }

      break;

    case GET_RESPONSE_LINE_END:

      if(data == '\n')
      {
        state = GET_RESPONSE_LINE_IDLE;
        out_buf[pointer] = '\0';
        pointer = 0;

        is_finished_getting_line = true;
      }

      break;

    default:

      state = GET_RESPONSE_LINE_IDLE;
      pointer = 0;

      break;
  }

  return is_finished_getting_line;
}


/**
 *  @brief
 *  	Get response type of request line.
 *
 *	@details
 *
 *	@param[in] in_buf
 *		Buffer for request line
 *
 *	@param[in] command
 *		Needed to know if the response type is HEADER or ECHO.
 *		If not looking for HEADER or ECHO, pass NULL on this
 *		parameter
 *
 *	@return  line_type
 *
 */
resp_type_t get_resp_type(const char * in_buf, const char * command)
{
  int line_type;
  char temp_buf[100] = {'A', 'T', '\0'};

  strcat(temp_buf, command);

  if((command != NULL) && (strstr(in_buf, temp_buf) != NULL))
  {
      line_type = RESP_TYPE_ECHO;
  }
  else if((command != NULL) && (strstr(in_buf, command) != NULL))
  {
    line_type = RESP_TYPE_HEADER;
  }
  else if((strstr(in_buf, "OK") != NULL) || (strstr(in_buf, "ERROR") != NULL))
  {
    line_type = RESP_TYPE_RESULT_CODE;
  }
  else
  {
    line_type = RESP_TYPE_PAYLOAD_DATA;
  }

  return line_type;
}
