/*
 * safetyCircuit.c
 *
 *  Created on: Jan 29, 2026
 *      Author: mtarka
 */

#include "safetyCircuit.h"
#include "mainForm.h"

SafetyCircuitPoint safetyCircuitPoint = SAFETY_CIRCUIT_UNBROKEN;
static volatile bool safetyCircuitState = FALSE;

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
	safetyCircuitState = HAL_GPIO_ReadPin(SAFETY_END_GPIO_Port, SAFETY_END_Pin);
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

void setSafetyCircuitStateOutput(void)
{
	HAL_GPIO_WritePin(K1_EN_GPIO_Port, K1_EN_Pin, safetyCircuitState);
}

void updateSafetyCircuitError(uint8_t state)
{
	if(state != SAFETY_CIRCUIT_UNBROKEN)
	{
		addRemoveError(SAFETY_CIRCUIT, TRUE);
	}
	else
	{
		addRemoveError(SAFETY_CIRCUIT, FALSE);
	}
}
