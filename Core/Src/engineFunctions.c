/*
 * engineFunctions.c
 *
 *  Created on: 29 sty 2026
 *      Author: mateo
 */


#include "engineFunctions.h"
#include "confInputs.h"

volatile uint32_t engineRotationTemporary = 0;
volatile uint32_t handrailRotationTemporary = 0;
volatile uint32_t stepRotationTemporary = 0;

static volatile bool highSpeedSet = FALSE;

static SoftwareTimerHandler fastSpeedTimer;
static SoftwareTimerHandler speedChangeTimer;
static SoftwareTimerHandler chainMotorErrorTimer;

RotationsPerMinute rotationsPerMinuteReal =
{
		{0, 0},
		{0, 0},
		{0, 0}
};

RotationsPerMinute rotationsPerMinuteGiven =
{
		{0, 0},
		{0, 0},
		{0, 0}
};

static bool getChainMotorState(void);

static void fastSpeedTimerCallback(void * Param)
{
	HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, FALSE);
	HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, TRUE);
}

static void speedChangeTimeoutCallback(void)
{
	if(!checkTargetFrequencyReached())
	{
		highSpeedSet = FALSE;
		HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, FALSE);
		HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, FALSE);
	}
}

static void chainMotorErrorCallback(void)
{
	if(!getChainMotorState())
	{
		highSpeedSet = FALSE;
		HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, FALSE);
		HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, FALSE);
	}
}

void initEngineTimers(void)
{
	deInitSoftwareTimer(&fastSpeedTimer);
	deInitSoftwareTimer(&speedChangeTimer);
	deInitSoftwareTimer(&chainMotorErrorTimer);

	initSoftwareTimer(&fastSpeedTimer, FAST_SPEED_TIME_MS, fastSpeedTimerCallback, FALSE, 0);
	initSoftwareTimer(&speedChangeTimer, SPEED_CHANGE_TIMEOUT_MS, speedChangeTimeoutCallback, FALSE, 0);
	initSoftwareTimer(&chainMotorErrorTimer, CHAIN_MOTOR_ERROR_TIMEOUT_MS, chainMotorErrorCallback, FALSE, 0);
}

void incrementRotationsNumber(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
	case ROTATION_S1_Pin:
		engineRotationTemporary++;
		break;

	case ROTATION_H1_Pin:
		handrailRotationTemporary++;
		break;

	case MIS_ST1_Pin:
		stepRotationTemporary++;
		break;
	}
}

void saveMeasuredRotationsValueTimerCallback(RotationsPerMinute *rotationsPerMinute)
{
	// Disable
	__HAL_GPIO_EXTI_DISABLE_IT(ROTATION_H1_Pin);
	__HAL_GPIO_EXTI_DISABLE_IT(ROTATION_S1_Pin);
	__HAL_GPIO_EXTI_DISABLE_IT(MIS_ST1_Pin);

	// Clear pending
	EXTI->RPR1 = ROTATION_H1_Pin | ROTATION_S1_Pin | MIS_ST1_Pin;
	EXTI->FPR1 = ROTATION_H1_Pin | ROTATION_S1_Pin | MIS_ST1_Pin;

	if(checkTargetFrequencyReached())
	{
		if(highSpeedSet)
		{
			rotationsPerMinute->engine.fastTime = engineRotationTemporary;
			rotationsPerMinute->handrail.fastTime = handrailRotationTemporary;
			rotationsPerMinute->step.fastTime = stepRotationTemporary;
		}
		else
		{
			rotationsPerMinute->engine.slowTime = engineRotationTemporary;
			rotationsPerMinute->handrail.slowTime = handrailRotationTemporary;
			rotationsPerMinute->step.slowTime = stepRotationTemporary;
		}
	}

	engineRotationTemporary = 0;
	handrailRotationTemporary = 0;
	stepRotationTemporary = 0;

	// Enable
	__HAL_GPIO_EXTI_ENABLE_IT(ROTATION_H1_Pin);
	__HAL_GPIO_EXTI_ENABLE_IT(ROTATION_S1_Pin);
	__HAL_GPIO_EXTI_ENABLE_IT(MIS_ST1_Pin);
}

bool checkTargetFrequencyReached(void)
{
	return(HAL_GPIO_ReadPin(IN_FREQ_GPIO_Port, IN_FREQ_Pin));
}

bool checkSetFrequency(void)
{
	if(highSpeedSet)
	{
		if(	(checkErrorRange(rotationsPerMinuteReal.engine.fastTime,  rotationsPerMinuteGiven.engine.fastTime) || getRotationControl()) &&
			(checkErrorRange(rotationsPerMinuteReal.handrail.fastTime, rotationsPerMinuteGiven.handrail.fastTime) || getHandrailControl()) &&
			(checkErrorRange(rotationsPerMinuteReal.step.fastTime, rotationsPerMinuteGiven.step.fastTime) || getStandControl()))
		{
			return TRUE;
		}
	}
	else
	{
		if(	checkErrorRange(rotationsPerMinuteReal.engine.slowTime,  rotationsPerMinuteGiven.engine.slowTime) &&
			checkErrorRange(rotationsPerMinuteReal.handrail.slowTime, rotationsPerMinuteGiven.handrail.slowTime) &&
			checkErrorRange(rotationsPerMinuteReal.step.slowTime, rotationsPerMinuteGiven.step.slowTime))
		{
			return TRUE;
		}
	}

	return FALSE;
}

bool checkErrorRange(uint32_t real, uint32_t given)
{
	if((real <= (given + FREQUENCY_ERROR_RANGE)) || (real >= (given - FREQUENCY_ERROR_RANGE)))
	{
		return TRUE;
	}
	return FALSE;
}

bool checkIsHumanOnStairs(void)
{
	return(HAL_GPIO_ReadPin(HUMAN_UP_GPIO_Port, HUMAN_UP_Pin));
}

void enableFastSpeed(void)
{
	highSpeedSet = TRUE;
	HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, FALSE);
	HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, TRUE);

	startSoftwareTimer(&fastSpeedTimer);
	startSoftwareTimer(&speedChangeTimer);
}

void enableSlowSpeed(void)
{
	highSpeedSet = TRUE;
	HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, FALSE);
	HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, TRUE);

	startSoftwareTimer(&speedChangeTimer);
}

void checkChainMotorOK(void)
{
	if(!getChainMotorState())
	{
		startSoftwareTimer(&chainMotorErrorTimer);
	}
	else
	{
		stopSoftwareTimer(&chainMotorErrorTimer);
	}
}

static bool getChainMotorState(void)
{
	return(HAL_GPIO_ReadPin(CHAIN_MOT_GPIO_Port, CHAIN_MOT_Pin));
}


