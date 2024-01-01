/*
 * gsm.h
 *
 *  Created on: 17 Dec 2023
 *      Author: russel
 */
#ifndef GSM_H
#define GSM_H

#include "at_common.h"

int send_command(char * command);
int send_data_check_result_and_response(char * command, char * out_buf, resp_type_t type, char * header);
int send_command_check_result_and_response(char * command, char * out_buf, resp_type_t type);
int send_command_check_response(char * command, char * out_buf);
int send_command_check_result(char *command);
void gsm_remove_response_header(char * response, char *out);

#endif
