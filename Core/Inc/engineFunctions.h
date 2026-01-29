/*
 * engineFunctions.h
 *
 *  Created on: 29 sty 2026
 *      Author: mateo
 */

#ifndef INC_ENGINEFUNCTIONS_H_
#define INC_ENGINEFUNCTIONS_H_

#include "main.h"

extern volatile uint32_t engineRotationTemporary;
extern volatile uint32_t handrailRotationTemporary;
extern volatile uint32_t stepRotationTemporary;

typedef struct
{
	uint32_t slowTime;
	uint32_t fastTime;
}SpeedTimes;

typedef struct
{
	SpeedTimes engine;
	SpeedTimes handrail;
	SpeedTimes step;
}RotationsPerMinute;

extern RotationsPerMinute rotationsPerMinuteReal;
extern RotationsPerMinute rotationsPerMinuteGiven;

void initEngineTimers(void);
void saveMeasuredRotationsValueTimerCallback(void);
void incrementRotationsNumber(uint16_t GPIO_Pin);

#endif /* INC_ENGINEFUNCTIONS_H_ */
