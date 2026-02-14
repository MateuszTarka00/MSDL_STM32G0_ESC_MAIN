/*
 * parameters.h
 *
 *  Created on: 14 lut 2026
 *      Author: mateo
 */

#ifndef PARAMETERS_PARAMETERS_H_
#define PARAMETERS_PARAMETERS_H_

#include "main.h"

typedef enum
{
	ON_OFF,
	TIME_MS,
}ParamyterType;

typedef struct
{
	uint32_t value;
	uint32_t lastValue;
	uint32_t maxValue;
	uint32_t minValue;
	uint32_t step;
	ParamyterType type;
}Parameter;

void paramGetValueString(char *buffer, Parameter parameter);
void paramChangeValue(Parameter *parameter, bool increaseDecrease);
void paramSaveValue(Parameter *parameter);
void paramCancelValue(Parameter *parameter);
void paramFactoryReset(void);

#endif /* PARAMETERS_PARAMETERS_H_ */
