/*
 * parameters.c
 *
 *  Created on: 14 lut 2026
 *      Author: mateo
 */

#include "parameters.h"
#include "string.h"
#include "stdio.h"
#include "flash.h"

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

//Fake parameter, only for format purpose
Parameter parameterFactoryReset=
{
		0,
		0,
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

	flash_parametersSave();
}

void paramCancelValue(Parameter *parameter)
{
	parameter->value = parameter->lastValue;
}

void paramLoadParameters(void)
{
	parameterLooserTime.value = flash_parametersToSave.flash_settingsValues.parameterLooserTime;
	parameterEngineTime.value = flash_parametersToSave.flash_settingsValues.parameterEngineTime;
	parameterContactorTime.value = flash_parametersToSave.flash_settingsValues.parameterContactorTime;
	parameterFastTime.value = flash_parametersToSave.flash_settingsValues.parameterFastTime;
	parameterSlowTime.value = flash_parametersToSave.flash_settingsValues.parameterSlowTime;
	parameterStarTriangleTime.value = flash_parametersToSave.flash_settingsValues.parameterStarTriangleTime;
	parameterEngineControl.value = flash_parametersToSave.flash_settingsValues.parameterEngineControl;
	parameterAutoStop.value = flash_parametersToSave.flash_settingsValues.parameterAutoStop;
	parameterReleasing.value = flash_parametersToSave.flash_settingsValues.parameterReleasing;
	parameterTrafficDirectionSignals.value = flash_parametersToSave.flash_settingsValues.parameterTrafficDirectionSignals;
	parameterLightning.value = flash_parametersToSave.flash_settingsValues.parameterLightning;

	parameterLooserTime.lastValue = flash_parametersToSave.flash_settingsValues.parameterLooserTime;
	parameterEngineTime.lastValue = flash_parametersToSave.flash_settingsValues.parameterEngineTime;
	parameterContactorTime.lastValue = flash_parametersToSave.flash_settingsValues.parameterContactorTime;
	parameterFastTime.lastValue = flash_parametersToSave.flash_settingsValues.parameterFastTime;
	parameterSlowTime.lastValue = flash_parametersToSave.flash_settingsValues.parameterSlowTime;
	parameterStarTriangleTime.lastValue = flash_parametersToSave.flash_settingsValues.parameterStarTriangleTime;
	parameterEngineControl.lastValue = flash_parametersToSave.flash_settingsValues.parameterEngineControl;
	parameterAutoStop.lastValue = flash_parametersToSave.flash_settingsValues.parameterAutoStop;
	parameterReleasing.lastValue = flash_parametersToSave.flash_settingsValues.parameterReleasing;
	parameterTrafficDirectionSignals.lastValue = flash_parametersToSave.flash_settingsValues.parameterTrafficDirectionSignals;
	parameterLightning.lastValue = flash_parametersToSave.flash_settingsValues.parameterLightning;
}
