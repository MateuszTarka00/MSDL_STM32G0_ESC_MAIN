/*
 * engineFunctions.c
 *
 *  Created on: 29 sty 2026
 *      Author: mateo
 */


#include "engineFunctions.h"

volatile uint32_t engineRotationTemporary = 0;
volatile uint32_t handrailRotationTemporary = 0;
volatile uint32_t stepRotationTemporary = 0;

void incrementRotationsNumber(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
	case ROTATION_S1_Pin:
		engineRotationTemporary++;
		break;

	case ROTATION_H1_Pin:
		handrailRotationTemporary++;
		break;

	case MIS_ST1_Pin:
		stepRotationTemporary++;
		break;
	}

}
