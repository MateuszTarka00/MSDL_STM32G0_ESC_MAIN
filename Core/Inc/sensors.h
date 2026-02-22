/*
 * sensors.h
 *
 *  Created on: 21 lut 2026
 *      Author: mateo
 */

#ifndef INC_SENSORS_H_
#define INC_SENSORS_H_

#include "main.h"

typedef enum
{
	NO_DIRECTION,
	DOWN,
	UP,
	DIRECTION_ERROR,
}Direction;

typedef enum
{
	NO_LOOSER,
	FIRST_LOOSER,
	SECOND_LOOSER,
	STOP,
}Loosers;

bool checkTargetFrequencyReached(void);
bool checkIsHumanOnStairsUp(void);
bool getChainMotorState(void);
Direction getDirection(void);
Loosers getLoosersState(void);
bool getContactorsState(void);
bool getIspectionMode(void);
bool getThermistorState(void);

#endif /* INC_SENSORS_H_ */
