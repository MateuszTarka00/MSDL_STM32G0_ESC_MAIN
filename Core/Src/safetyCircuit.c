/*
 * safetyCircuit.c
 *
 *  Created on: Jan 29, 2026
 *      Author: mtarka
 */

#include "safetyCircuit.h"
#include "mainForm.h"
#include "softwareTimer_ms.h"
#include "suppCpuCommunication.h"
#include "sensors.h"
#include "parameters.h"
#include "engineFunctions.h"

SoftwareTimerHandler contactorTimer;
SoftwareTimerHandler looserTimer;
SafetyCircuitPoint safetyCircuitPoint = SAFETY_CIRCUIT_UNBROKEN;

static volatile bool safetyCircuitState = FALSE;
static volatile bool contactor1errorSet = FALSE;
static volatile bool contactor2errorSet = FALSE;
static volatile bool contactor3errorSet = FALSE;

const SafetyCircuitPoints safetyCircuitPoints[NUMBER_OF_CIRCUITS_POINTS] =
{
	{SAFETY_C1_GPIO_Port, SAFETY_C1_Pin},
	{SAFETY_C2_GPIO_Port, SAFETY_C2_Pin},
	{SAFETY_C3_GPIO_Port, SAFETY_C3_Pin},
	{SAFETY_C4_GPIO_Port, SAFETY_C4_Pin},
	{SAFETY_C5_GPIO_Port, SAFETY_C5_Pin},
	{SAFETY_C6_GPIO_Port, SAFETY_C6_Pin},
	{SAFETY_C7_GPIO_Port, SAFETY_C7_Pin},
	{SAFETY_C8_GPIO_Port, SAFETY_C8_Pin},
};

#define SAFETY_END_Pin GPIO_PIN_12
#define SAFETY_END_GPIO_Port GPIOD

bool checkSafetyCircuitState(void)
{
	safetyCircuitState = HAL_GPIO_ReadPin(SAFETY_END_GPIO_Port, SAFETY_END_Pin) && getSafetyStateSupp();
	return safetyCircuitState;
}

SafetyCircuitPoint checkBrokenSafetyCircuitPoint(void)
{
	for(uint8_t i = 0; i < NUMBER_OF_CIRCUITS_POINTS; ++i)
	{
		if(HAL_GPIO_ReadPin(safetyCircuitPoints[i].port, safetyCircuitPoints[i].pin) == BROKEN_CIRCUIT_POINT)
		{
			return i;
		}
	}
	return SAFETY_CIRCUIT_ERROR;
}

void setContactorK1State(void)
{
	static uint8_t restartClicks = 6;
	static uint32_t ticksTemp;

	if(safetyCircuitState && !noContactorErrors)
	{
		if(restartClicks)
		{
			if(xTaskGetTickCount() - ticksTemp >= CONTACTOR_CLICK_TIME)
			{
				HAL_GPIO_TogglePin(K1_EN_GPIO_Port, K1_EN_Pin);
				ticksTemp = xTaskGetTickCount();
				restartClicks--;
			}
		}
		else
		{
			HAL_GPIO_WritePin(K1_EN_GPIO_Port, K1_EN_Pin, safetyCircuitState);
			setHardStop(!safetyCircuitState);
		}
	}
	else
	{
		setHardStop(TRUE);
		HAL_GPIO_WritePin(K1_EN_GPIO_Port, K1_EN_Pin, FALSE);
		restartClicks = 6;
	}
}

void updateSafetyCircuitError(uint8_t state)
{
	if(state != SAFETY_CIRCUIT_UNBROKEN || !getSafetyStateSupp())
	{
		addRemoveError(SAFETY_CIRCUIT, TRUE);
	}
	else
	{
		addRemoveError(SAFETY_CIRCUIT, FALSE);
	}
}

void updateContactorsStates(void)
{

	if(getIntContactorState())
	{
		contactor1errorSet = FALSE;
		addRemoveError(CONTACTOR_1_FALLING_OFF, FALSE);
	}
	else if(!contactor1errorSet)
	{
		if(contactorTimer.start == FALSE)
		{
			startSoftwareTimer(&contactorTimer);
		}
	}

	if(getAckK2())
	{
		contactor1errorSet = FALSE;
		addRemoveError(CONTACTOR_2_FALLING_OFF, FALSE);
	}
	else if(!contactor2errorSet)
	{
		if(contactorTimer.start == FALSE)
		{
			startSoftwareTimer(&contactorTimer);
		}
	}


	if((getDirection() == UP || getDirection() == DOWN) && getAckK2() && getIntContactorState())
	{
		if(getExtContactorState())
		{
			contactor3errorSet = FALSE;
			addRemoveError(CONTACTOR_3_FALLING_OFF, FALSE);
		}
		else
		{
			if(contactorTimer.start == FALSE)
			{
				startSoftwareTimer(&contactorTimer);
			}
		}
	}
	else
	{
		if(!getExtContactorState())
		{
			contactor3errorSet = FALSE;
			addRemoveError(CONTACTOR_3_FALLING_OFF, FALSE);
		}
		else
		{
			if(contactorTimer.start == FALSE)
			{
				startSoftwareTimer(&contactorTimer);
			}
		}
	}

}

static void contactorTimerCallback(void *param)
{
	if(!getIntContactorState())
	{
		contactor1errorSet = TRUE;
		addRemoveError(CONTACTOR_1_FALLING_OFF, TRUE);
	}


	if(!getAckK2())
	{
		contactor2errorSet = TRUE;
		addRemoveError(CONTACTOR_2_FALLING_OFF, TRUE);
	}

	if((getDirection() == UP || getDirection() == DOWN) && getAckK2() && getIntContactorState())
	{
		if(!getExtContactorState())
		{
			contactor3errorSet = TRUE;
			addRemoveError(CONTACTOR_3_FALLING_OFF, TRUE);
		}

	}
	else
	{
		if(getExtContactorState())
		{
			contactor3errorSet = TRUE;
			addRemoveError(CONTACTOR_3_FALLING_OFF, TRUE);
		}
	}

}

static void looserTimerCallback(void *param)
{
	addRemoveError(LOOSER_ERROR, TRUE);
}

void initSafetyTimers(void)
{
	deInitSoftwareTimer(&contactorTimer);
	deInitSoftwareTimer(&looserTimer);

	initSoftwareTimer(&contactorTimer, parameterContactorTime.value, contactorTimerCallback, FALSE, 0);
	initSoftwareTimer(&looserTimer, parameterLooserTime.value, looserTimerCallback, FALSE, 0);
}

void updateLoosersStates(void)
{
	if(getLoosersState())
	{
		if(highSpeedSet || slowSpeedSet)
		{
			if(looserTimer.start == FALSE)
			{
				startSoftwareTimer(&looserTimer);
			}
		}
	}
	else
	{
		addRemoveError(LOOSER_ERROR, FALSE);
		stopSoftwareTimer(&looserTimer);
	}
}


