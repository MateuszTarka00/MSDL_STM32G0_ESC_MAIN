/*
 * teachForm.c
 *
 *  Created on: 22 lut 2026
 *      Author: mateo
 */

#ifndef MENUS_TEACHFORM_C_
#define MENUS_TEACHFORM_C_

#include "teachForm.h"
#include "st7789.h"
#include "fonts.h"
#include "buttonsFunctions.h"
#include "engineFunctions.h"
#include "sensors.h"
#include "softwareTimer_ms.h"

#define TEACH_TIME		10000
#define END_TEACH_TIME	12000

SoftwareTimerHandler teachTimer;
SoftwareTimerHandler endTeachingTimer;

void downButtonFunctionTeachMenu(void *param);
void upButtonFunctionTeachMenu(void *param);
void okButtonFunctionTeachMenu(void *param);
void escButtonFunctionTeachMenu(void *param);

TeachStateMachine teachStateMachine = START_STATE;

void endTeachingTimerCallback(void *param)
{

}

void stateMachineSubTask(void)
{
	switch(teachStateMachine)
	{
		case WAIT_FOR_SLOW_SPEED:
			if(checkTargetFrequencyReached())
			{
				teachStateMachine = SLOW_SPEED_TIME;
				enterSlowSpeedTime();
				initSoftwareTimer(&teachTimer, TEACH_TIME, saveMeasuredRotationsValueTimerCallback, FALSE, &rotationsPerMinuteGiven);
				initSoftwareTimer(&endTeachingTimer, END_TEACH_TIME, endTeachingTimerCallback, FALSE, &rotationsPerMinuteGiven);
				break;
			}

		case SLOW_SPEED_TIME:


		case WAIT_FOR_FAST_SPEED:
			if(checkTargetFrequencyReached())
			{
				teachStateMachine = FAST_SPEED_TIME;
				enterFastSpeedTime();
				break;
			}
	}
}

void enterRestartDriverState(void)
{
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Aby przejsc do trybu nauki predkosci, zrestartuj urzadzenie", Font_11x18, WHITE, BLACK);
	ST7789_WriteString(2, 150, "Aby wyjsc, wcisnij ESC", Font_11x18, WHITE, BLACK);
}

void enterPreparationState(void)
{
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Aby rozpoczac nauke powolnej predkosci, nacisnij OK", Font_11x18, WHITE, BLACK);
}

void enterWaitForSlowSpeed(void)
{
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Oczekiwanie na osiagniecie zadanej predkosci", Font_11x18, WHITE, BLACK);
}

void enterSlowSpeedTime(void)
{
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Sterownik sie uczy wolnej predkosci", Font_11x18, WHITE, BLACK);
}

void enterSlowSpeedConfirmation(void)
{
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Pomiary wolnej predkosci:", Font_11x18, WHITE, BLACK);
	ST7789_WriteString(2, 200, "ESC -> Powtorz", Font_11x18, WHITE, BLACK);
	ST7789_WriteString(2, 220, "OK -> Zapisz", Font_11x18, WHITE, BLACK);
}

void enterWaitForFastSpeed(void)
{
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Oczekiwanie na osiagniecie zadanej predkosci", Font_11x18, WHITE, BLACK);
}

void enterFastSpeedTime(void)
{
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Sterownik sie uczy wolnej predkosci", Font_11x18, WHITE, BLACK);
}

void enterFastSpeedConfirmation(void)
{
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Pomiary wolnej predkosci:", Font_11x18, WHITE, BLACK);
	ST7789_WriteString(2, 200, "ESC -> Powtorz", Font_11x18, WHITE, BLACK);
	ST7789_WriteString(2, 220, "OK -> Zapisz", Font_11x18, WHITE, BLACK);
}

void downButtonFunctionTeachMenu(void *param)
{

}

void upButtonFunctionTeachMenu(void *param)
{

}

void okButtonFunctionTeachMenu(void *param)
{

}

void escButtonFunctionTeachMenu(void *param)
{

}


#endif /* MENUS_TEACHFORM_C_ */
