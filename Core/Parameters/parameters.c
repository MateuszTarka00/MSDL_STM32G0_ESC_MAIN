/*
 * parameters.c
 *
 *  Created on: 14 lut 2026
 *      Author: mateo
 */

#include "parameters.h"
#include "string.h"
#include "stdio.h"

Parameter parameterLooserTime = {
		1000,
		1000,
		3000,
		1000,
		1000,
		TIME_MS,
};

Parameter parameterEngineTime = {
		100,
		100,
		100000,
		100,
		100,
		TIME_MS,
};

Parameter parameterContactorTime = {
		1000,
		1000,
		3000,
		1000,
		1000,
		TIME_MS,
};

Parameter parameterFastTime = {
		10000,
		10000,
		100000,
		10000,
		10000,
		TIME_MS,
};

Parameter parameterSlowTime = {
		60000,
		60000,
		600000,
		60000,
		10000,
		TIME_MS,
};

Parameter parameterStarTriangleTime = {
		1000,
		1000,
		5000,
		1000,
		1000,
		TIME_MS,
};

Parameter parameterEngineControl =
{
		1,
		1,
		1,
		0,
		1,
		ON_OFF,
};


Parameter parameterAutoStop =
{
		1,
		1,
		1,
		0,
		1,
		ON_OFF,
};

Parameter parameterReleasing =
{
		1,
		1,
		1,
		0,
		1,
		ON_OFF,
};

Parameter parameterTrafficDirectionSignals =
{
		1,
		1,
		1,
		0,
		1,
		ON_OFF,
};

Parameter parameterLightning=
{
		1,
		1,
		1,
		0,
		1,
		ON_OFF,
};


void paramGetValueString(char *buffer, Parameter *parameter)
{
	switch(parameter->type)
	{
		case ON_OFF:
			if(parameter->value)
				sprintf(buffer, "TAK");
			else
				sprintf(buffer, "NIE");
			break;

		case TIME_MS:
			if(parameter->step > 100)
			{
				uint32_t stringValue = parameter->value/1000;

				if(stringValue >= 100)
				{
					sprintf(buffer, "%u s", stringValue);
				}
				else
				{
					sprintf(buffer, " %u s ", stringValue);
				}
			}
			else
			{
				uint32_t stringValueInteger = parameter->value/1000;
				uint32_t stringValueFraction = (parameter->value%1000)/100;

				sprintf(buffer, "%u.%u s", stringValueInteger, stringValueFraction);
			}
	}
}

void paramChangeValue(Parameter *parameter, bool increaseDecrease)
{
	if(increaseDecrease)
	{
		if(parameter->maxValue > parameter->value)
		{
			parameter->value += parameter->step;
		}
	}
	else
	{
		if(parameter->minValue < parameter->value)
		{
			parameter->value -= parameter->step;
		}
	}
}

void paramSaveValue(Parameter *parameter)
{
	parameter->lastValue = parameter->value;
}

void paramCancelValue(Parameter *parameter)
{
	parameter->value = parameter->lastValue;
}

void paramFactoryReset(void)
{

}

void flashLoadSavedParameters(void)
{

}
