/*
 * safetyCircuit.h
 *
 *  Created on: Jan 29, 2026
 *      Author: mtarka
 */

#ifndef INC_SAFETYCIRCUIT_H_
#define INC_SAFETYCIRCUIT_H_

#include "main.h"

#define NUMBER_OF_CIRCUITS_POINTS	8
#define BROKEN_CIRCUIT_POINT		0
#define WORKING_CIRCUIT_POIT		1

typedef struct
{
	GPIO_TypeDef *port;
	uint16_t pin;
}SafetyCircuitPoints;

typedef enum
{
	STOP_ENGINE_ROOM,
	STOP_INTAKE,
	RIGHT_COMB_PLATE,
	LEFT_COMB_PLATE,
	RIGHT_HANDRAIL_INLET,
	LEFT_HANDRAIL_INLET,
	STEP_DECLINE,
	RESERVE,
	SAFETY_CIRCUIT_ERROR,
	SAFETY_CIRCUIT_UNBROKEN
}SafetyCircuitPoint;

extern const SafetyCircuitPoints safetyCircuitPoints[NUMBER_OF_CIRCUITS_POINTS];
extern SafetyCircuitPoint safetyCircuitPoint;

bool checkSafetyCircuitState(void);
SafetyCircuitPoint checkBrokenSafetyCircuitPoint(void);
void setSafetyCircuitStateOutput(void);

void updateSafetyCircuitError(uint8_t state);

#endif /* INC_SAFETYCIRCUIT_H_ */
