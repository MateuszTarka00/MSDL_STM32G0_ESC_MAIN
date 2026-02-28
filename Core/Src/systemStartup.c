/*
 * systemStartup.c
 *
 *  Created on: 17 lut 2026
 *      Author: mateo
 */


#include "systemStartup.h"
#include "parameters.h"
#include "engineFunctions.h"
#include "confInputs.h"
#include "flash.h"

volatile bool teachOnStartup = FALSE;

void checkTeachOnStartup(void)
{
	if(getTeachInput())
	{
		teachOnStartup = TRUE;
	}
}

void startupFunction(void)
{
	if(flash_loadParameters())
	{
		checkTeachOnStartup();
		paramLoadParameters();
		rotationsLoadParameters();
	}
}
