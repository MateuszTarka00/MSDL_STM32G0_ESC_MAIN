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
#include "systemStartup.h"
#include "suppCpuCommunication.h"

#define TEACH_TIME		10000
#define END_TEACH_TIME	12000

SoftwareTimerHandler teachTimer;
SoftwareTimerHandler endTeachingTimer;

void downButtonFunctionTeachMenu(void *param);
void upButtonFunctionTeachMenu(void *param);
void okButtonFunctionTeachMenu(void *param);
void escButtonFunctionTeachMenu(void *param);

TeachStateMachine teachStateMachine = START_STATE;

static uint32_t gapsBetweenSteps = 0;
static uint32_t stepsCounted = 0;

void enableFastSpeedTeach(void)
{
	HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, FALSE);
	HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, TRUE);
}

void enableSlowSpeedTeach(void)
{
	HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, FALSE);
	HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, TRUE);
}

void endTeachingTimerCallback(void *param)
{
	//Callback is necessery, but is not needed
}

void calculateTeachedSpeeds(bool slowFast)
{
	if(slowFast)
	{
		rotationsPerMinuteGiven.engine.slowTime = rotationsPerMinuteGiven.engine.slowTime/10;
		rotationsPerMinuteGiven.handrail.slowTime = rotationsPerMinuteGiven.handrail.slowTime/10;
		rotationsPerMinuteGiven.step.slowTime = gapsBetweenSteps/stepsCounted;
	}
	else
	{
		rotationsPerMinuteGiven.engine.fastTime = rotationsPerMinuteGiven.engine.fastTime/10;
		rotationsPerMinuteGiven.handrail.fastTime = rotationsPerMinuteGiven.handrail.fastTime/10;
		rotationsPerMinuteGiven.step.fastTime = gapsBetweenSteps/stepsCounted;
	}

	gapsBetweenSteps = 0;
	stepsCounted = 0;

}

void enterRestartDriverState(void)
{
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Aby przejsc do trybu nauki predkosci, zrestartuj urzadzenie", Font_11x18, WHITE, BLACK);
	ST7789_WriteString(2, 150, "Aby wyjsc, wylacz teachSwitch", Font_11x18, WHITE, BLACK);
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

void enterEndState(void)
{
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Zakonczono uczenie", Font_11x18, WHITE, BLACK);
	ST7789_WriteString(2, 100, "Wylacz teachSwitch\n i zrestartuj urzadzenie", Font_11x18, WHITE, BLACK);
}

void stateMachineSubTask(void)
{
	switch(teachStateMachine)
	{
		case START_STATE:
			if(teachOnStartup)
			{
				teachStateMachine = PREPARATION;
				enterPreparationState();
			}
			else
			{
				teachStateMachine = RESTART_DRIVER;
				enterRestartDriverState();
			}
			break;

		case RESTART_DRIVER:
			enterRestartDriverState();
			break;

		case PREPARATION:
			setEngineOnOff(TRUE);
			enableSlowSpeedTeach();
			setFastSpeed(FALSE);
			setSlowSpeed(TRUE);
			setTeachFast(FALSE);
			setTeachSlow(TRUE);
			teachStateMachine = WAIT_FOR_SLOW_SPEED;
			break;

		case WAIT_FOR_SLOW_SPEED:
			if(checkTargetFrequencyReached() && getSpeedReady())
			{
				teachStateMachine = SLOW_SPEED_TIME;
				enterSlowSpeedTime();
				initSoftwareTimer(&teachTimer, TEACH_TIME, saveMeasuredRotationsValueTimerCallback, FALSE, &rotationsPerMinuteGiven);
				initSoftwareTimer(&endTeachingTimer, END_TEACH_TIME, endTeachingTimerCallback, FALSE, &rotationsPerMinuteGiven);

				startSoftwareTimer(&teachTimer);
				startSoftwareTimer(&endTeachingTimer);

			}
			break;

		case SLOW_SPEED_TIME:
			if(endTeachingTimer.start == FALSE && getTeachingEnd())
			{
				teachStateMachine = SLOW_SPEED_CONFIRMATION;
				calculateTeachedSpeeds(FALSE);
				enterSlowSpeedConfirmation();
			}
			setTeachFast(FALSE);
			setTeachSlow(FALSE);
			break;

		case SLOW_SPEED_CONFIRMATION:
			//This step is handled by buttons
			break;


		case WAIT_FOR_FAST_SPEED:
			if(checkTargetFrequencyReached() && getSpeedReady())
			{
				teachStateMachine = FAST_SPEED_TIME;
				enterFastSpeedTime();

				startSoftwareTimer(&teachTimer);
				startSoftwareTimer(&endTeachingTimer);
			}
			break;

		case FAST_SPEED_TIME:
			if(endTeachingTimer.start == FALSE && getTeachingEnd())
			{
				teachStateMachine = FAST_SPEED_CONFIRMATION;
				calculateTeachedSpeeds(TRUE);
				enterSlowSpeedConfirmation();
			}
			setTeachFast(FALSE);
			setTeachSlow(FALSE);
			break;

		case FAST_SPEED_CONFIRMATION:
			//This step is handled by buttons
			break;

		case END:
			rotationsSaveParameters();
			break;

	}
}

void okButtonFunctionTeachMenu(void *param)
{
	switch(teachStateMachine)
	{
	case SLOW_SPEED_CONFIRMATION:
		setEngineOnOff(TRUE);
		enableSlowSpeedTeach();
		setSlowSpeed(FALSE);
		setTeachSlow(FALSE);
		setTeachFast(TRUE);
		setFastSpeed(TRUE);
		teachStateMachine = WAIT_FOR_FAST_SPEED;
		enterWaitForFastSpeed();
		break;

	case FAST_SPEED_CONFIRMATION:
		setEngineOnOff(FALSE);
		setSlowSpeed(FALSE);
		setTeachSlow(FALSE);
		setTeachFast(FALSE);
		setFastSpeed(FALSE);
		teachStateMachine = END;
		enterEndState();
		break;

	default:
		break;
	}

}

void escButtonFunctionTeachMenu(void *param)
{
	switch(teachStateMachine)
	{
	case SLOW_SPEED_CONFIRMATION:
		setEngineOnOff(TRUE);
		enableSlowSpeedTeach();
		setFastSpeed(FALSE);
		setSlowSpeed(TRUE);
		setTeachFast(FALSE);
		setTeachSlow(TRUE);
		teachStateMachine = WAIT_FOR_SLOW_SPEED;
		enterWaitForSlowSpeed();
		break;

	case FAST_SPEED_CONFIRMATION:
		setEngineOnOff(TRUE);
		enableSlowSpeedTeach();
		setSlowSpeed(FALSE);
		setTeachSlow(FALSE);
		setTeachFast(TRUE);
		setFastSpeed(TRUE);
		teachStateMachine = WAIT_FOR_FAST_SPEED;
		enterWaitForFastSpeed();
		break;

	default:
		break;
	}
}

void stepsTeachExtiCallback(uint16_t GPIO_Pin)
{
	static bool steps1 = FALSE;
	static bool steps2 = FALSE;
	static uint32_t gapBetweenStepsTemp = 0;

	if(teachStateMachine == SLOW_SPEED_TIME || teachStateMachine == FAST_SPEED_TIME)
	{

		if(GPIO_Pin == MIS_ST2_Pin)
		{
			if(steps1)
			{
				gapBetweenStepsTemp = HAL_GetTick() - gapBetweenStepsTemp;
				gapsBetweenSteps += gapBetweenStepsTemp;
				gapBetweenStepsTemp = 0;
			}
			else
			{
				gapBetweenStepsTemp = HAL_GetTick();
				stepsCounted++;
				steps2 = TRUE;
			}
		}
		else if(GPIO_Pin == MIS_ST1_Pin)
		{
			if(steps2)
			{
				steps2 = FALSE;
				gapBetweenStepsTemp = HAL_GetTick() - gapBetweenStepsTemp;
				gapsBetweenSteps += gapBetweenStepsTemp;
				gapBetweenStepsTemp = 0;
			}
			else
			{
				gapBetweenStepsTemp = HAL_GetTick();
				stepsCounted++;
				steps1 = TRUE;
			}
		}
	}
}


#endif /* MENUS_TEACHFORM_C_ */
