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

Loosers getLoosersState(void)
{
	bool loserOne = HAL_GPIO_ReadPin(LOCKER_IN1_GPIO_Port, LOCKER_IN1_Pin);
	bool looserTwo = HAL_GPIO_ReadPin(LOCKER_IN2_GPIO_Port, LOCKER_IN2_Pin);

	if(loserOne && looserTwo)
	{
		return STOP;
	}
	else
	{
		if(loserOne)
		{
			return FIRST_LOOSER;
		}

		if(looserTwo)
		{
			return SECOND_LOOSER;
		}
	}

	return NO_LOOSER;

}

bool getContactorsState(void)
{
	return(HAL_GPIO_ReadPin(ACK_CONT_GPIO_Port, ACK_CONT_Pin));
}

