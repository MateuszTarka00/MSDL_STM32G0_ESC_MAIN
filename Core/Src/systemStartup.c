/*
 * systemStartup.c
 *
 *  Created on: 17 lut 2026
 *      Author: mateo
 */


#include "systemStartup.h"
#include "parameters.h"
#include "engineFunctions.h"
#include "flash.h"

void startupFunction(void)
{
	if(flash_loadParameters())
	{
		paramLoadParameters();
		rotationsLoadParameters();
	}
}
