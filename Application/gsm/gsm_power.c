#include "main.h"
#include "gsm_power.h"
#include "ring_buffer.h"

extern ring_buf_t ring_buf_modem;

void gsm_poweron(void)
{
	gsm_module_status status = GSM_STATUS_OFF;
	// Wait for 100ms for VBAT to stabilize
	HAL_Delay(100);

	// 	Trigger PWRKey to active low for 2s if module is not running
	HAL_GPIO_WritePin(GPIOB, PWRKEY_Pin, SET);
	HAL_Delay(2000);
	HAL_GPIO_WritePin(GPIOB, PWRKEY_Pin, RESET);

	//	Check if STATUS PIN is high to know if Module is running
	do
	{
		status = HAL_GPIO_ReadPin(GPIOB, STATUS_Pin);
	} while (status == GSM_STATUS_OFF);
}

void gsm_poweroff(void)
{
	gsm_module_status status = GSM_STATUS_OFF;
	// Wait for 100ms for VBAT to stabilize
	HAL_Delay(100);

	//  Trigger PWRKey to active low for 1s if module is running
	HAL_GPIO_WritePin(GPIOB, PWRKEY_Pin, SET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOB, PWRKEY_Pin, RESET);

	//  Wait for 12 seconds
	HAL_Delay(12000);

	//  Check is STATUS PIN is low to know if Module is not running
	do
	{
		status = HAL_GPIO_ReadPin(GPIOB, STATUS_Pin);
	} while (status == GSM_STATUS_RUNNING);
}

void gsm_restart(void)
{
	HAL_GPIO_WritePin(GPIOB, PWRKEY_Pin, RESET);

	// Check if module is runnning or not
	gsm_module_status status = GSM_STATUS_OFF;
	status = HAL_GPIO_ReadPin(GPIOB, STATUS_Pin);

	// If not running
	if(status == GSM_STATUS_OFF)
	{
		gsm_poweron();

		// We do not know if module was correctly shutdown previously
		// so we need to power cycle again
		gsm_poweroff();
		gsm_poweron();
	}
	else
	{
		gsm_poweroff();
		gsm_poweron();
	}

	ring_buf_init(&ring_buf_modem);
}