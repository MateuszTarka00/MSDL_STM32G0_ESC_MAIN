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
#define SPEED_CHANGE_TIMEOUT_MS			20000

extern volatile bool highSpeedSet;
extern volatile bool slowSpeedSet;

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

typedef struct
{
	bool engineSpeedState;
	bool handRailSpeedState;
	bool stepSpeedState;
	bool chainMotorState;
}EngineErrors;

extern RotationsPerMinute rotationsPerMinuteReal;
extern RotationsPerMinute rotationsPerMinuteGiven;
extern EngineErrors engineErrors;

extern SoftwareTimerHandler fastSpeedTimer;
extern SoftwareTimerHandler slowSpeedTimer;

void initEngineTimers(void);
void saveMeasuredRotationsValueTimerCallback(RotationsPerMinute *rotationsPerMinute);
void incrementRotationsNumber(uint16_t GPIO_Pin);
void restartRotationsTmp(void);
void enableFastSpeed(void);
void enableSlowSpeed(void);
void stopEngine(void);
bool checkSetFrequency(void);
bool checkErrorRange(uint32_t real, uint32_t given);
void rotationsLoadParameters(void);
void rotationsSaveParameters(void);
void stepsNormalExtiCallback(uint16_t GPIO_Pin);
void setEngineOnOff(bool onOff);
void checkLooserStateSubTask(void);
bool checkSpeedTeached(void);
void engineSubTask(void);
void trafficSignalsFunction(void);

#endif /* INC_ENGINEFUNCTIONS_H_ */
