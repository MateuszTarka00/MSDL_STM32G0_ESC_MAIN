/*
 * suppCpuCommunication.c
 *
 *  Created on: 27 lut 2026
 *      Author: mateo
 */


#include "suppCpuCommunication.h"

void setFactoryReset(void)
{
	HAL_GPIO_WritePin(FACTORY_RESET_GPIO_Port, FACTORY_RESET_Pin, TRUE);
}

void setTeachFast(bool onOff)
{
	HAL_GPIO_WritePin(TEACH_FAST_GPIO_Port, TEACH_FAST_Pin, onOff);
}

void setTeachSlow(bool onOff)
{
	HAL_GPIO_WritePin(TEACH_SLOW_GPIO_Port, TEACH_SLOW_Pin, onOff);
}


bool getSpeedReady(void)
{
	return HAL_GPIO_ReadPin(SPEED_READY_GPIO_Port, SPEED_READY_Pin);
}

bool getRotationState(void)
{
	return HAL_GPIO_ReadPin(ROTATION_OK_GPIO_Port, ROTATION_OK_Pin);
}

bool getStandState(void)
{
	return HAL_GPIO_ReadPin(STAND_OK_GPIO_Port, STAND_OK_Pin);
}

bool getSafetyState(void)
{
	return HAL_GPIO_ReadPin(SAFETY_OK_GPIO_Port, SAFETY_OK_Pin);
}

void setSlowSpeed(bool onOff)
{
	HAL_GPIO_WritePin(SLOW_SPEED_GPIO_Port, SLOW_SPEED_Pin, onOff);
}

void setFastSpeed(bool onOff)
{
	HAL_GPIO_WritePin(FAST_SPEED_GPIO_Port, FAST_SPEED_Pin, onOff);
}

void setTeachOutput(bool onOff)
{
	HAL_GPIO_WritePin(TEACH_OUTPUT_GPIO_Port, TEACH_OUTPUT_Pin, onOff);
}

bool getTeachingEnd(void)
{
	return HAL_GPIO_ReadPin(END_TEACHING_GPIO_Port, END_TEACHING_Pin);
}
