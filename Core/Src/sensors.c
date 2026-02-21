/*
 * sensors.c
 *
 *  Created on: 21 lut 2026
 *      Author: mateo
 */


#include "sensors.h"

bool checkTargetFrequencyReached(void)
{
	return(HAL_GPIO_ReadPin(IN_FREQ_GPIO_Port, IN_FREQ_Pin));
}

bool checkIsHumanOnStairsUp(void)
{
	return(HAL_GPIO_ReadPin(HUMAN_UP_GPIO_Port, HUMAN_UP_Pin));
}

bool getChainMotorState(void)
{
	return(HAL_GPIO_ReadPin(CHAIN_MOT_GPIO_Port, CHAIN_MOT_Pin));
}

Direction getDirection(void)
{
	bool directionDown = HAL_GPIO_ReadPin(CHECK_DOWN_GPIO_Port, CHECK_DOWN_Pin);
	bool directionUp = HAL_GPIO_ReadPin(CHECK_UP_GPIO_Port, CHECK_UP_Pin);

	if(directionDown && directionUp)
	{
		return DIRECTION_ERROR;
	}
	else
	{
		if(directionDown)
		{
			return DOWN;
		}

		if(directionUp)
		{
			return UP;
		}

	}

	return NO_DIRECTION;
}
