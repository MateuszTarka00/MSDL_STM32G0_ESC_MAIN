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
	LANGUAGE,
}ParamyterType;

typedef enum
{
	POLISH
}LanguageType;

typedef struct
{
	uint32_t value;
	uint32_t lastValue;
	uint32_t maxValue;
	uint32_t minValue;
	uint32_t step;
	ParamyterType type;
}Parameter;

extern Parameter parameterLooserTime;
extern Parameter parameterEngineTime;
extern Parameter parameterContactorTime;
extern Parameter parameterFastTime;
extern Parameter parameterSlowTime;
extern Parameter parameterStarTriangleTime;
extern Parameter parameterEngineControl;
extern Parameter parameterHandrailControl;
extern Parameter parameterStepControl;
extern Parameter parameterAutoStop;
extern Parameter parameterReleasing;
extern Parameter parameterTrafficDirectionSignals;
extern Parameter parameterLightning;
extern Parameter parameterLanguage;

extern Parameter parameterFactoryReset;
extern Parameter parameteClearLogs;

void paramGetValueString(char *buffer, Parameter *parameter);
void paramChangeValue(Parameter *parameter, bool increaseDecrease);
void paramSaveValue(Parameter *parameter);
void paramCancelValue(Parameter *parameter);
void paramFactoryReset(void);
void paramLoadParameters(void);
void applyParameters(void);
void toeBoardLightFunction(void);
void trafficSignalsFunction(void);

#endif /* PARAMETERS_PARAMETERS_H_ */
