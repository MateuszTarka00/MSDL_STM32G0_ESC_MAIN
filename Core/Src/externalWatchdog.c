/*
 * externalWatchdog.c
 *
 *  Created on: 6 lut 2026
 *      Author: mateo
 */


#include "externalWatchdog.h"
#include "softwareTimer_ms.h"

#define WHATCHDOG_TIME_MS	500

static SoftwareTimerHandler externalWatchdogTimer;

//Toggle watchdog pin to let watchog mcu is alive
void whatchdogTimerHandler(void *param)
{
	HAL_GPIO_TogglePin(WATCHDOG_OUT_GPIO_Port, WATCHDOG_OUT_Pin);
}

void initWatchdogTimerInit(void)
{
	initSoftwareTimer(&externalWatchdogTimer, WHATCHDOG_TIME_MS, whatchdogTimerHandler, TRUE, 0);
	startSoftwareTimer(&externalWatchdogTimer);
}

