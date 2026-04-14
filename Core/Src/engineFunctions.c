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
#include "parameters.h"
#include "mainForm.h"
#include "systemStartup.h"
#include "suppCpuCommunication.h"
#include "canCommunication.h"

#define SAVE_SPEED_TIME_MS			1000
#define VOLTAGE_REDUCTION_TIME_MS	3000

volatile uint32_t engineRotationTemporary = 0;
volatile uint32_t handrailRotationTemporary = 0;

volatile bool highSpeedSet = FALSE;
volatile bool slowSpeedSet = FALSE;

SoftwareTimerHandler fastSpeedTimer;
SoftwareTimerHandler slowSpeedTimer;
static SoftwareTimerHandler speedChangeTimer;
static SoftwareTimerHandler chainMotorErrorTimer;
static SoftwareTimerHandler stepsErrorTimer;
static SoftwareTimerHandler speedSave;
static SoftwareTimerHandler voltageReductionTimer;

static WorkModeType engineWorkMode = CONSTANS;
static bool speedReached = FALSE;
static uint8_t encoderErrorCounter = 0;

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

void startSpeedCheckTimer(void)
{
	startSoftwareTimer(&speedSave);
}

static void setVoltageReduction(void)
{
	HAL_GPIO_WritePin(VOLTAGE_REDUCTION_GPIO_Port, VOLTAGE_REDUCTION_Pin, TRUE);
}

static void resetVoltageReduction(void)
{
	HAL_GPIO_WritePin(VOLTAGE_REDUCTION_GPIO_Port, VOLTAGE_REDUCTION_Pin, FALSE);
}

static void stepsErrorTimerCallback(void *Param)
{
	if((!getStandState() || getStandControl()) && !getIspectionMode())
	{
		engineErrors.stepSpeedState = FALSE;
		addRemoveError(STEP_FALLING_OFF, TRUE);
	}
}

static void fastSpeedTimerCallback(void * Param)
{
	HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, FALSE);
	HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, TRUE);
}

static void slowSpeedTimerCallback(void * Param)
{
	HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, FALSE);
	HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, TRUE);
}

static void speedChangeTimeoutCallback(void)
{
	if(!errorStateActive)
	{
		if(!checkTargetFrequencyReached())
		{
			highSpeedSet = FALSE;
			slowSpeedSet = FALSE;
			HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, FALSE);
			HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, FALSE);
			addRemoveError(SPEED_CHANGE_ERROR, TRUE);
		}
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
		addRemoveError(CHAIN_FALLING_OFF, TRUE);
	}
}

static void looserErrorCallback(void)
{
	if(!getLoosersState())
	{
		highSpeedSet = FALSE;
		slowSpeedSet = FALSE;
		HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, FALSE);
		HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, FALSE);
		addRemoveError(LOOSER_ERROR, TRUE);
	}
}

void setEngineOnOff(bool onOff)
{
	if(getDirection() == UP)
	{
		HAL_GPIO_WritePin(INVERTER_FWD_GPIO_Port, INVERTER_FWD_Pin, onOff);
		startSoftwareTimer(&voltageReductionTimer);
	}
	else if(getDirection() == DOWN)
	{
		HAL_GPIO_WritePin(INVERTER_REV_GPIO_Port, INVERTER_REV_Pin, onOff);
		startSoftwareTimer(&voltageReductionTimer);
	}

	if(!onOff)
	{
		HAL_GPIO_WritePin(INVERTER_REV_GPIO_Port, INVERTER_REV_Pin, onOff);
		HAL_GPIO_WritePin(INVERTER_FWD_GPIO_Port, INVERTER_FWD_Pin, onOff);
	}
}

void softStopEngine(void)
{
	setEngineOnOff(FALSE);
	highSpeedSet = FALSE;
	slowSpeedSet = FALSE;
	HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, FALSE);
	HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, FALSE);
	setFastSpeed(FALSE);
	setSlowSpeed(FALSE);
	speedReached = FALSE;
	resetVoltageReduction();

	stopSoftwareTimer(&fastSpeedTimer);
	stopSoftwareTimer(&slowSpeedTimer);

}

void enableFastSpeed(void)
{
	speedReached = FALSE;
	highSpeedSet = TRUE;
	slowSpeedSet = FALSE;
	setEngineOnOff(TRUE);
	HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, FALSE);
	HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, TRUE);
	setSlowSpeed(FALSE);
	setFastSpeed(TRUE);

	deInitSoftwareTimer(&stepsErrorTimer);
	initSoftwareTimer(&stepsErrorTimer, rotationsPerMinuteGiven.step.fastTime, stepsErrorTimerCallback, FALSE, 0);
	startSoftwareTimer(&speedChangeTimer);

	deInitSoftwareTimer(&fastSpeedTimer);

	if(workWithoutBottom.value == FALSE)
	{
		if(engineWorkMode == FAST_SLOW_STOP)
		{
			initSoftwareTimer(&fastSpeedTimer, parameterFastTime.value, enableSlowSpeed, FALSE, 0);
			startSoftwareTimer(&fastSpeedTimer);
		}
		else if(engineWorkMode == FAST_STOP)
		{
			initSoftwareTimer(&fastSpeedTimer, parameterFastTime.value, softStopEngine, FALSE, 0);
			startSoftwareTimer(&fastSpeedTimer);
		}
	}
}

void enableSlowSpeed(void)
{
	speedReached = FALSE;
	highSpeedSet = FALSE;
	slowSpeedSet = TRUE;
	setEngineOnOff(TRUE);
	HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, FALSE);
	HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, TRUE);
	setFastSpeed(FALSE);
	setSlowSpeed(TRUE);

	deInitSoftwareTimer(&stepsErrorTimer);
	initSoftwareTimer(&stepsErrorTimer, rotationsPerMinuteGiven.step.slowTime, stepsErrorTimerCallback, FALSE, 0);
	startSoftwareTimer(&speedChangeTimer);

	if(workWithoutBottom.value == FALSE)
	{
		if(engineWorkMode == FAST_SLOW_STOP)
		{
			startSoftwareTimer(&slowSpeedTimer);
		}
	}
}

void initEngineTimers(void)
{
	deInitSoftwareTimer(&fastSpeedTimer);
	deInitSoftwareTimer(&slowSpeedTimer);
	deInitSoftwareTimer(&speedChangeTimer);
	deInitSoftwareTimer(&chainMotorErrorTimer);
	deInitSoftwareTimer(&speedSave);
	deInitSoftwareTimer(&voltageReductionTimer);

	initSoftwareTimer(&slowSpeedTimer, parameterSlowTime.value, softStopEngine, FALSE, 0);
	initSoftwareTimer(&fastSpeedTimer, parameterFastTime.value, enableSlowSpeed, FALSE, 0);
	initSoftwareTimer(&speedChangeTimer, SPEED_CHANGE_TIMEOUT_MS, speedChangeTimeoutCallback, FALSE, 0);
	initSoftwareTimer(&chainMotorErrorTimer, parameterEngineTime.value, chainMotorErrorCallback, FALSE, 0);
	initSoftwareTimer(&speedSave, SAVE_SPEED_TIME_MS, saveMeasuredRotationsValueTimerCallback, TRUE, &rotationsPerMinuteReal);
	initSoftwareTimer(&voltageReductionTimer, VOLTAGE_REDUCTION_TIME_MS, setVoltageReduction, FALSE, 0);
}

void applyTimerValues(void)
{
	deInitSoftwareTimer(&fastSpeedTimer);
	deInitSoftwareTimer(&slowSpeedTimer);
	deInitSoftwareTimer(&chainMotorErrorTimer);

	initSoftwareTimer(&slowSpeedTimer, parameterSlowTime.value, softStopEngine, FALSE, 0);
	initSoftwareTimer(&fastSpeedTimer, parameterFastTime.value, enableSlowSpeed, FALSE, 0);
	initSoftwareTimer(&chainMotorErrorTimer, parameterEngineTime.value, chainMotorErrorCallback, FALSE, 0);
}

void incrementRotationsNumber(uint16_t GPIO_Pin)
{
	if(checkTargetFrequencyReached())
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
	else
	{
		engineRotationTemporary = 0;
		handrailRotationTemporary = 0;

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

			if(!rotationsPerMinute->engine.fastTime || !rotationsPerMinute->handrail.fastTime)
			{
				encoderErrorCounter++;
			}
			else
			{
				encoderErrorCounter = 0;
			}
		}
		else if(slowSpeedSet)
		{
			rotationsPerMinute->engine.slowTime = engineRotationTemporary;
			rotationsPerMinute->handrail.slowTime = handrailRotationTemporary;

			if(!rotationsPerMinute->engine.slowTime || !rotationsPerMinute->handrail.slowTime)
			{
				encoderErrorCounter++;
			}
			else
			{
				encoderErrorCounter = 0;
			}
		}

	}

	engineRotationTemporary = 0;
	handrailRotationTemporary = 0;

	// Enable
	__HAL_GPIO_EXTI_ENABLE_IT(ROTATION_H1_Pin);
	__HAL_GPIO_EXTI_ENABLE_IT(MIS_ST1_Pin);
}

void restartRotationsTmp(void)
{
	engineRotationTemporary = 0;
	handrailRotationTemporary = 0;
}

bool checkSetFrequency(void)
{
	if(checkTargetFrequencyReached())
	{
		if(highSpeedSet)
		{
			engineErrors.engineSpeedState = checkErrorRange(rotationsPerMinuteReal.engine.fastTime,  rotationsPerMinuteGiven.engine.fastTime);
			engineErrors.handRailSpeedState = checkErrorRange(rotationsPerMinuteReal.handrail.fastTime, rotationsPerMinuteGiven.handrail.fastTime) | !parameterHandrailControl.value;
		}
		else if(slowSpeedSet)
		{
			engineErrors.engineSpeedState = checkErrorRange(rotationsPerMinuteReal.engine.slowTime,  rotationsPerMinuteGiven.engine.slowTime);
			engineErrors.handRailSpeedState = checkErrorRange(rotationsPerMinuteReal.handrail.slowTime, rotationsPerMinuteGiven.handrail.slowTime) | !parameterHandrailControl.value;
		}
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
	uint32_t errorRange = given/FREQUENCY_ERROR_RANGE;

	if(errorRange == 0)
	{
		errorRange = 1;
	}

	if((real <= (given + errorRange)) && (real >= (given - errorRange)))
	{
		return TRUE;
	}
	return FALSE;
}

void stopEngine(void)
{
	highSpeedSet = FALSE;
	slowSpeedSet = FALSE;
	speedReached = FALSE;
	HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, FALSE);
	HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, FALSE);
	setFastSpeed(FALSE);
	setSlowSpeed(FALSE);
	speedReached = FALSE;
	resetVoltageReduction();
	stopSoftwareTimer(&fastSpeedTimer);
	stopSoftwareTimer(&slowSpeedTimer);

}

void checkChainMotorOK(void)
{
	if(parameterEngineControl.value)
	{
		if(!getChainMotorState())
		{
			if(chainMotorErrorTimer.start == FALSE)
			{
				startSoftwareTimer(&chainMotorErrorTimer);
			}
		}
		else
		{
			stopSoftwareTimer(&chainMotorErrorTimer);
			addRemoveError(CHAIN_FALLING_OFF, FALSE);
		}
	}
}

void rotationsLoadParameters(void)
{
	rotationsPerMinuteGiven.engine.fastTime   = flash_parametersToSave.flash_RotationsPerMinuteFast.engine;
	rotationsPerMinuteGiven.handrail.fastTime = flash_parametersToSave.flash_RotationsPerMinuteFast.handrail;
	rotationsPerMinuteGiven.step.fastTime     = flash_parametersToSave.flash_RotationsPerMinuteFast.step;

	rotationsPerMinuteGiven.engine.slowTime   = flash_parametersToSave.flash_RotationsPerMinuteSlow.engine;
	rotationsPerMinuteGiven.handrail.slowTime = flash_parametersToSave.flash_RotationsPerMinuteSlow.handrail;
	rotationsPerMinuteGiven.step.slowTime     = flash_parametersToSave.flash_RotationsPerMinuteSlow.step;
}

void rotationsSaveParameters(void)
{
	flash_parametersToSave.flash_RotationsPerMinuteFast.engine = rotationsPerMinuteGiven.engine.fastTime;
	flash_parametersToSave.flash_RotationsPerMinuteFast.handrail = rotationsPerMinuteGiven.handrail.fastTime;
	flash_parametersToSave.flash_RotationsPerMinuteFast.step = rotationsPerMinuteGiven.step.fastTime;

	flash_parametersToSave.flash_RotationsPerMinuteSlow.engine = rotationsPerMinuteGiven.engine.slowTime;
	flash_parametersToSave.flash_RotationsPerMinuteSlow.handrail = rotationsPerMinuteGiven.handrail.slowTime;
	flash_parametersToSave.flash_RotationsPerMinuteSlow.step = rotationsPerMinuteGiven.step.slowTime;

	flash_parametersToSave.flash_settingsValues.parameterLooserTime = parameterLooserTime.value;
	flash_parametersToSave.flash_settingsValues.parameterEngineTime = parameterEngineTime.value;
	flash_parametersToSave.flash_settingsValues.parameterContactorTime = parameterContactorTime.value;
	flash_parametersToSave.flash_settingsValues.parameterFastTime = parameterFastTime.value;
	flash_parametersToSave.flash_settingsValues.parameterSlowTime = parameterSlowTime.value;
	flash_parametersToSave.flash_settingsValues.parameterStarTriangleTime = parameterStarTriangleTime.value;
	flash_parametersToSave.flash_settingsValues.parameterEngineControl = parameterEngineControl.value;
	flash_parametersToSave.flash_settingsValues.parameterAutoStop = parameterAutoStop.value;
	flash_parametersToSave.flash_settingsValues.parameterReleasing = parameterReleasing.value;
	flash_parametersToSave.flash_settingsValues.parameterTrafficDirectionSignals = parameterTrafficDirectionSignals.value;
	flash_parametersToSave.flash_settingsValues.parameterLightning = parameterLightning.value;
	flash_parametersToSave.flash_settingsValues.parameterHandrailControl = parameterHandrailControl.value;
	flash_parametersToSave.flash_settingsValues.parameterStepControl = parameterStepControl.value;
	flash_parametersToSave.flash_settingsValues.parameterHardFault = parameterHardFault.value;

	flash_parametersSave();
}

void stepsNormalExtiCallback(uint16_t GPIO_Pin)
{
	static bool steps1 = FALSE;
	static bool steps2 = FALSE;

	if(checkTargetFrequencyReached() && !parameterStepControl.value)
	{
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
}

void trafficSignalsFunction(void)
{
	if(parameterTrafficDirectionSignals.value)
	{
		if(getDirection() == DOWN)
		{
			HAL_GPIO_WritePin(SIGNAL_GREEN_GPIO_Port, SIGNAL_GREEN_Pin, TRUE);
			HAL_GPIO_WritePin(SIGNAL_RED_GPIO_Port, SIGNAL_RED_Pin, FALSE);
		}
		else
		{
			HAL_GPIO_WritePin(SIGNAL_GREEN_GPIO_Port, SIGNAL_GREEN_Pin, FALSE);
			HAL_GPIO_WritePin(SIGNAL_RED_GPIO_Port, SIGNAL_RED_Pin, TRUE);
		}
	}
	else
	{
		HAL_GPIO_WritePin(SIGNAL_GREEN_GPIO_Port, SIGNAL_GREEN_Pin, FALSE);
		HAL_GPIO_WritePin(SIGNAL_RED_GPIO_Port, SIGNAL_RED_Pin, FALSE);
	}
}

bool checkSpeedTeached(void)
{
	if(((rotationsPerMinuteGiven.engine.fastTime == 0 || rotationsPerMinuteGiven.engine.slowTime == 0)) ||
		((rotationsPerMinuteGiven.handrail.fastTime == 0 || rotationsPerMinuteGiven.handrail.slowTime == 0) && parameterHandrailControl.value) ||
		((rotationsPerMinuteGiven.step.fastTime == 0 || rotationsPerMinuteGiven.step.slowTime == 0) && parameterStepControl.value) )
	{
		return FALSE;
	}

	return TRUE;
}

void engineSubTask(void)
{
	static Direction previousDirection = NO_DIRECTION;

	if(parameterAutoStop.value)
	{
		if(parameterReleasing.value)
		{
			engineWorkMode = FAST_SLOW_STOP;
		}
		else
		{
			engineWorkMode = FAST_STOP;
		}
	}
	else
	{
		if(parameterReleasing.value)
		{
			engineWorkMode = FAST_SLOW;
		}
		else
		{
			engineWorkMode = CONSTANS;
		}
	}

	if(activeMenu != TEACHING_MENU)
	{
		checkChainMotorOK();
	}

	if(!getIspectionMode() && !serviceMode && checkTargetFrequencyReached())
	{
		if(!checkSetFrequency() || !getRotationState())
		{
			addRemoveError(SPEED_ERROR, TRUE);
		}
	}

	if(workWithoutBottom.value)
	{
		engineWorkMode = CONSTANS;
	}

	if(checkSpeedTeached() || getIspectionMode() || serviceMode)
	{

		if(encoderErrorCounter == 3)
		{
			addRemoveError(ENCODER_ERROR, TRUE);
		}

		if(!errorStateActive && (getDirection() == DOWN || getDirection() == UP))
		{
			switch(engineWorkMode)
			{
			case CONSTANS:
				if(!highSpeedSet)
				{
					enableFastSpeed();
				}
				break;
			case FAST_STOP:
				if((getDirection() == DOWN && checkIsHumanOnStairsUp()) ||
				   (getDirection() == UP && getHumanDown()))
				{
					enableFastSpeed();
				}

				break;
			case FAST_SLOW:
				if((getDirection() == DOWN && checkIsHumanOnStairsUp()) ||
				   (getDirection() == UP && getHumanDown()))
				{
					enableFastSpeed();
				}
				break;

			case FAST_SLOW_STOP:
				if((getDirection() == DOWN && checkIsHumanOnStairsUp()) ||
				   (getDirection() == UP && getHumanDown()))
				{
					enableFastSpeed();
				}
				break;
			}

		}
		else
		{
			softStopEngine();
		}

		if((previousDirection == NO_DIRECTION ) && (getDirection() == DOWN || getDirection() == UP))
		{
			enableFastSpeed();
		}

		if(previousDirection != NO_DIRECTION && (checkIsHumanOnStairsUp() || getHumanDown()) && !highSpeedSet && !slowSpeedSet)
		{
			enableFastSpeed();
		}
	}

	previousDirection = getDirection();
}




