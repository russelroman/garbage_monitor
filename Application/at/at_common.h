#ifndef AT_COMMON_H
#define AT_COMMON_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
  RESP_TYPE_RESULT_CODE,
  RESP_TYPE_PAYLOAD_DATA,
  RESP_TYPE_HEADER,
  RESP_TYPE_ECHO,
  MAX_RESP_TYPE
} resp_type_t;

bool get_response_line(char data, char * out_buf);
resp_type_t determine_type(const char * in_buf, const char * command);
resp_type_t get_resp_type(const char * in_buf, const char * command);

#endif /* AT_COMMON_H */
