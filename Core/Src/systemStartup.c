/*
 * systemStartup.c
 *
 *  Created on: 17 lut 2026
 *      Author: mateo
 */


#include "systemStartup.h"
#include "parameters.h"
#include "engineFunctions.h"
#include "confInputs.h"
#include "flash.h"
#include "suppCpuCommunication.h"

volatile bool teachOnStartup = FALSE;

#define BUTTON_OK_Pin GPIO_PIN_9
#define BUTTON_OK_GPIO_Port GPIOF

volatile bool serviceMode = FALSE;

void getServiceMode(void)
{
	serviceMode = HAL_GPIO_ReadPin(BUTTON_ESC_GPIO_Port, BUTTON_ESC_Pin);
}

void checkTeachOnStartup(void)
{
	if(getTeachInput())
	{
		teachOnStartup = TRUE;
		setTeachOutput(TRUE);
	}
}

void startupFunction(void)
{
	getServiceMode();
	checkTeachOnStartup();
	if(flash_loadParameters())
	{
		paramLoadParameters();
		rotationsLoadParameters();
	}
}
