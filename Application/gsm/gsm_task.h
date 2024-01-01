/*
 * gsm_task.h
 *
 *  Created on: 18 Dec 2023
 *      Author: russel
 */

#ifndef GSM_TASK_H_
#define GSM_TASK_H_

typedef struct
{
    char iccid[25];
    char imsi[20];
    char imei[20];
} sim_info_t;

typedef struct 
{
    char rssi[10];
    char telecom[30];
} network_t;


int gsm_check_modem(void);
int gsm_check_sim(sim_info_t * sim_info);
int gsm_check_network();
int gsm_connect_gprs();
int gsm_setup_tls();
int gsm_mqtt_setup();



#endif /* GSM_TASK_H_ */
