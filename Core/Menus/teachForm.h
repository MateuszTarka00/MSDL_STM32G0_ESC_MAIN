/*
 * teachForm.h
 *
 *  Created on: 22 lut 2026
 *      Author: mateo
 */

#ifndef MENUS_TEACHFORM_H_
#define MENUS_TEACHFORM_H_

#include "main.h"

typedef enum
{
	START_STATE,
	RESTART_DRIVER,
	PREPARATION,
	WAIT_FOR_SLOW_SPEED,
	SLOW_SPEED_TIME,
	SLOW_SPEED_CONFIRMATION,
	WAIT_FOR_FAST_SPEED,
	FAST_SPEED_TIME,
	FAST_SPEED_CONFIRMATION,
	END,
	ENTER_ERROR_STATE,
	ERROR_STATE,
}TeachStateMachine;

extern bool teachingMenuActive;

void stateMachineSubTask(void);
void enterTeachingForm(void);
void stepsTeachExtiCallback(uint16_t GPIO_Pin);

#endif /* MENUS_TEACHFORM_H_ */
