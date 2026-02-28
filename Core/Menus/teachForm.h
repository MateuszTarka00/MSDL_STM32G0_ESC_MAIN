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
}TeachStateMachine;

void stateMachineSubTask(void);
//void enterRestartDriverState(void);
//void enterPreparationState(void);
//void enterWaitForSlowSpeed(void);
//void enterSlowSpeedTime(void);
//void enterSlowSpeedConfirmation(void);
//void enterWaitForFastSpeed(void);
//void enterFastSpeedTime(void);
//void enterFastSpeedConfirmation(void);



#endif /* MENUS_TEACHFORM_H_ */
