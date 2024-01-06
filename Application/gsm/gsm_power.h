typedef enum
{
	GSM_STATUS_OFF = 0,
	GSM_STATUS_RUNNING = 1
} gsm_module_status;

void gsm_poweron(void);
void gsm_poweroff(void);
void gsm_restart(void);