/*
 * mainForm.h
 *
 *  Created on: 18 lut 2026
 *      Author: mateo
 */

#ifndef MENUS_MAINFORM_H_
#define MENUS_MAINFORM_H_

#include "main.h"

typedef enum
{
	SPEED_STOP,
	SLOW,
	FAST,
}SpeedType;

typedef enum
{
	CONSTANS,
	TIMED,
	AUTO_STOP,
	INSPECTION,
}WorkModeType;

typedef enum
{
	NO_ERROR,
	THERMISTOR,
	CHAIN_FALLING_OFF,
	STEP_FALLING_OFF,
	STAR_TRIANGLE,
	CONTACTORS_FALLING_OFF,
	SAFETY_CIRCUIT,
}ErrorsType;

extern bool mainMenuActive;

void updateSafetyCircuitState(void);
void updateSensorUp(void);
void mainMenuSubTask(void);
void initMainForm(void);
void addRemoveError(ErrorsType error, bool removeAdd);

#endif /* MENUS_MAINFORM_H_ */
