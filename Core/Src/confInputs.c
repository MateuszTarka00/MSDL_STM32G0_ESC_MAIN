/*
 * confInputs.c
 *
 *  Created on: 5 lut 2026
 *      Author: mateo
 */

#include "confInputs.h"

bool getTeachInput(void)
{
	bool teachInput = !HAL_GPIO_ReadPin(TEACH_INPUT_GPIO_Port, TEACH_INPUT_Pin);
	HAL_GPIO_WritePin(TEACH_OUTPUT_GPIO_Port, TEACH_OUTPUT_Pin, teachInput);

	return teachInput;
}

bool getStandControl(void)
{
	return !HAL_GPIO_ReadPin(STAND_CONTROL_GPIO_Port, STAND_CONTROL_Pin);
}

bool getHandrailControl(void)
{
	return !HAL_GPIO_ReadPin(HANDRAIL_CONTROL_GPIO_Port, HANDRAIL_CONTROL_Pin);
}

bool getRotationControl(void)
{
	return !HAL_GPIO_ReadPin(ROTATION_CONTROL_GPIO_Port, ROTATION_CONTROL_Pin);
}

bool getContractorsControl(void)
{
	return !HAL_GPIO_ReadPin(CONTACTORS_CONTROL_GPIO_Port, CONTACTORS_CONTROL_Pin);
}

bool getStopTimeControl(bool number)
{
	if(number)
	{
		return !HAL_GPIO_ReadPin(STOP_TIME2_CONTROL_GPIO_Port, STOP_TIME2_CONTROL_Pin);
	}
	else
	{
		return !HAL_GPIO_ReadPin(STOP_TIME1_CONTROL_GPIO_Port, STOP_TIME1_CONTROL_Pin);
	}
}

bool getAutoStopControl(void)
{
	return !HAL_GPIO_ReadPin(AUTOSTOP_CONTROL_GPIO_Port, AUTOSTOP_CONTROL_Pin);
}

bool getInverterControl(void)
{
	return !HAL_GPIO_ReadPin(INVERTER_CONTROL_GPIO_Port, INVERTER_CONTROL_Pin);
}

bool getReleaseControl(void)
{
	return !HAL_GPIO_ReadPin(RELEASE_CONTROL_GPIO_Port, RELEASE_CONTROL_Pin);
}

bool getStarTriangleControl(void)
{
	return !HAL_GPIO_ReadPin(STAR_TRIANGLE_CONTROL_GPIO_Port, STAR_TRIANGLE_CONTROL_Pin);
}

bool getLockerControl(void)
{
	return !HAL_GPIO_ReadPin(LOCKER_CONTROL_GPIO_Port, LOCKER_CONTROL_Pin);
}

