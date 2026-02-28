/*
 * engineFunctions.c
 *
 *  Created on: 29 sty 2026
 *      Author: mateo
 */


#include "engineFunctions.h"
#include "confInputs.h"
#include "sensors.h"
#include "flash.h"

volatile uint32_t engineRotationTemporary = 0;
volatile uint32_t handrailRotationTemporary = 0;

volatile bool highSpeedSet = FALSE;
volatile bool slowSpeedSet = FALSE;

static SoftwareTimerHandler fastSpeedTimer;
static SoftwareTimerHandler speedChangeTimer;
static SoftwareTimerHandler chainMotorErrorTimer;
static SoftwareTimerHandler stepsErrorTimer;

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

EngineErrors engineErrors = {
		TRUE,
		TRUE,
		TRUE,
		TRUE,
};

static void stepsErrorTimerCallback(void *Param)
{
	engineErrors.stepSpeedState = FALSE;
}

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
		slowSpeedSet = FALSE;
		HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, FALSE);
		HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, FALSE);
		engineErrors.chainMotorState = FALSE;
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
	}
}

void saveMeasuredRotationsValueTimerCallback(RotationsPerMinute *rotationsPerMinute)
{
	// Disable
	__HAL_GPIO_EXTI_DISABLE_IT(ROTATION_H1_Pin);
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
		}
		else if(slowSpeedSet)
		{
			rotationsPerMinute->engine.slowTime = engineRotationTemporary;
			rotationsPerMinute->handrail.slowTime = handrailRotationTemporary;
		}
	}

	engineRotationTemporary = 0;
	stepRotationTemporary = 0;

	// Enable
	__HAL_GPIO_EXTI_ENABLE_IT(ROTATION_H1_Pin);
	__HAL_GPIO_EXTI_ENABLE_IT(MIS_ST1_Pin);
}

bool checkSetFrequency(void)
{
	if(checkTargetFrequencyReached())
	{
		engineErrors.engineSpeedState = TRUE;
		engineErrors.handRailSpeedState = TRUE;
	}
	else if(highSpeedSet)
	{
		engineErrors.engineSpeedState = checkErrorRange(rotationsPerMinuteReal.engine.fastTime,  rotationsPerMinuteGiven.engine.fastTime) || getRotationControl();
		engineErrors.handRailSpeedState = checkErrorRange(rotationsPerMinuteReal.handrail.fastTime, rotationsPerMinuteGiven.handrail.fastTime) || getHandrailControl();
	}
	else if(slowSpeedSet)
	{
		engineErrors.engineSpeedState = checkErrorRange(rotationsPerMinuteReal.engine.slowTime,  rotationsPerMinuteGiven.engine.slowTime) || getRotationControl();
		engineErrors.handRailSpeedState = checkErrorRange(rotationsPerMinuteReal.handrail.slowTime, rotationsPerMinuteGiven.handrail.slowTime) || getHandrailControl();
	}
	else
	{
		engineErrors.engineSpeedState = TRUE;
		engineErrors.handRailSpeedState = TRUE;
	}

	return engineErrors.engineSpeedState & engineErrors.handRailSpeedState;

}

bool checkErrorRange(uint32_t real, uint32_t given)
{
	if((real <= (given + FREQUENCY_ERROR_RANGE)) || (real >= (given - FREQUENCY_ERROR_RANGE)))
	{
		return TRUE;
	}
	return FALSE;
}

void enableFastSpeed(void)
{
	highSpeedSet = TRUE;
	slowSpeedSet = FALSE;
	HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, FALSE);
	HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, TRUE);

	startSoftwareTimer(&fastSpeedTimer);

	deInitSoftwareTimer(&stepsErrorTimer);
	initSoftwareTimer(&stepsErrorTimer, rotationsPerMinuteGiven.handrail.fastTime, stepsErrorTimerCallback, FALSE, 0);
	//	startSoftwareTimer(&speedChangeTimer);
}

void enableSlowSpeed(void)
{
	highSpeedSet = FALSE;
	slowSpeedSet = TRUE;
	HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, FALSE);
	HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, TRUE);

	deInitSoftwareTimer(&stepsErrorTimer);
	initSoftwareTimer(&stepsErrorTimer, rotationsPerMinuteGiven.handrail.slowTime, stepsErrorTimerCallback, FALSE, 0);

//	startSoftwareTimer(&speedChangeTimer);
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

void rotationsLoadParameters(void)
{
	flash_parametersToSave.flash_RotationsPerMinuteFast.engine = rotationsPerMinuteReal.engine.fastTime;
	flash_parametersToSave.flash_RotationsPerMinuteFast.handrail = rotationsPerMinuteReal.handrail.fastTime;
	flash_parametersToSave.flash_RotationsPerMinuteFast.step = rotationsPerMinuteReal.step.fastTime;

	flash_parametersToSave.flash_RotationsPerMinuteSlow.engine = rotationsPerMinuteReal.engine.slowTime;
	flash_parametersToSave.flash_RotationsPerMinuteSlow.handrail = rotationsPerMinuteReal.handrail.slowTime;
	flash_parametersToSave.flash_RotationsPerMinuteSlow.step = rotationsPerMinuteReal.step.slowTime;
}

void rotationsSaveParameters(void)
{
	rotationsPerMinuteReal.engine.fastTime   = flash_parametersToSave.flash_RotationsPerMinuteFast.engine;
	rotationsPerMinuteReal.handrail.fastTime = flash_parametersToSave.flash_RotationsPerMinuteFast.handrail;
	rotationsPerMinuteReal.step.fastTime     = flash_parametersToSave.flash_RotationsPerMinuteFast.step;

	rotationsPerMinuteReal.engine.slowTime   = flash_parametersToSave.flash_RotationsPerMinuteSlow.engine;
	rotationsPerMinuteReal.handrail.slowTime = flash_parametersToSave.flash_RotationsPerMinuteSlow.handrail;
	rotationsPerMinuteReal.step.slowTime     = flash_parametersToSave.flash_RotationsPerMinuteSlow.step;

	flash_parametersSave();
}

void stepsNormalExtiCallback(uint16_t GPIO_Pin)
{
	static bool steps1 = FALSE;
	static bool steps2 = FALSE;

	if(GPIO_Pin == MIS_ST2_Pin)
	{
		if(steps1)
		{
			steps1 = FALSE;
			stopSoftwareTimer(&stepsErrorTimer);
		}
		else
		{
			steps2 = TRUE;
			startSoftwareTimer(&stepsErrorTimer);
		}
	}
	else if(GPIO_Pin == MIS_ST1_Pin)
	{
		if(steps2)
		{
			steps2 = FALSE;
			stopSoftwareTimer(&stepsErrorTimer);
		}
		else
		{
			steps1 = TRUE;
			startSoftwareTimer(&stepsErrorTimer);
		}
	}
}

void setEngineOnOff(bool onOff)
{
	if(getDirection() == UP)
	{
		HAL_GPIO_WritePin(INVERTER_FWD_GPIO_Port, INVERTER_FWD_Pin, onOff);
	}
	else if(getDirection() == DOWN)
	{
		HAL_GPIO_WritePin(INVERTER_REV_GPIO_Port, INVERTER_REV_Pin, onOff);
	}
}
