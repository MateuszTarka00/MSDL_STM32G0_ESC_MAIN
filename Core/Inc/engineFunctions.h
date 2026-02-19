/*
 * engineFunctions.h
 *
 *  Created on: 29 sty 2026
 *      Author: mateo
 */

#ifndef INC_ENGINEFUNCTIONS_H_
#define INC_ENGINEFUNCTIONS_H_

#include "main.h"
#include "softwareTimer_ms.h"

#define FREQUENCY_ERROR_RANGE 			10
#define FAST_SPEED_TIME_MS				60000
#define SPEED_CHANGE_TIMEOUT_MS			10000
#define CHAIN_MOTOR_ERROR_TIMEOUT_MS 	1000

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
void saveMeasuredRotationsValueTimerCallback(RotationsPerMinute *rotationsPerMinute);
void incrementRotationsNumber(uint16_t GPIO_Pin);
void enableFastSpeed(void);
void enableSlowSpeed(void);
bool checkTargetFrequencyReached(void);
bool checkSetFrequency(void);
bool checkErrorRange(uint32_t real, uint32_t given);
bool checkIsHumanOnStairsUp(void);
void rotationsLoadParameters(void);
void rotationsSaveParameters(void);

#endif /* INC_ENGINEFUNCTIONS_H_ */
