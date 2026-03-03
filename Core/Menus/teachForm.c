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
#include "mainForm.h"
#include "confInputs.h"

#define WAIT_FOR_SPEED_TIME		60000
#define TEACH_TIME				10000
#define END_TEACH_TIME			12000

SoftwareTimerHandler teachTimer;
SoftwareTimerHandler endTeachingTimer;

SoftwareTimerHandler waitForSpeedTimer;

void enableFastSpeed(void);
void enableSlowSpeed(void);

void downButtonFunctionTeachMenu(void *param);
void upButtonFunctionTeachMenu(void *param);
void okButtonFunctionTeachMenu(void *param);
void escButtonFunctionTeachMenu(void *param);

TeachStateMachine teachStateMachine = START_STATE;
TeachStateMachine previousStateMachine;

static uint32_t gapsBetweenSteps = 0;
static uint32_t stepsCounted = 0;

static bool hasErrorOccured = FALSE;

bool teachingMenuActive = FALSE;

void enterSlowSpeedConfirmation(void)
{
	char buffer[30];
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Pomiary wolnej\npredkosci:", Font_11x18, BLACK, WHITE);
	sprintf(buffer, "Silnik: %u obr/s", rotationsPerMinuteGiven.engine.slowTime);
	ST7789_WriteString(2, 50, buffer, Font_11x18, BLACK, WHITE);
	sprintf(buffer, "Porecz: %u obr/s", rotationsPerMinuteGiven.handrail.slowTime);
	ST7789_WriteString(2, 73, buffer, Font_11x18, BLACK, WHITE);
	sprintf(buffer, "Stopnie: %u ms", rotationsPerMinuteGiven.step.slowTime);
	ST7789_WriteString(2, 96, buffer, Font_11x18, BLACK, WHITE);
	ST7789_WriteString(2, 265, "ESC -> Powtorz", Font_11x18, BLACK, WHITE);
	ST7789_WriteString(2, 285, "OK -> Zapisz", Font_11x18, BLACK, WHITE);
}

void waitForSpeedTimerCallback(void *param)
{
	previousStateMachine = teachStateMachine;
	teachStateMachine = ENTER_ERROR_STATE;
}

void enterEndState(void)
{
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Zakonczono uczenie", Font_11x18, BLACK, WHITE);
	ST7789_WriteString(2, 220, "Wylacz teachSwitch i\nzrestartuj urzadzenie", Font_11x18, BLACK, WHITE);
}

void enableSlowSpeedTeach(void)
{
	highSpeedSet = FALSE;
	slowSpeedSet = TRUE;
	HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, FALSE);
	HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, TRUE);
}

void enableFastSpeedTeach(void)
{
	slowSpeedSet = FALSE;
	highSpeedSet = TRUE;
	HAL_GPIO_WritePin(LOW_SPEED_GPIO_Port, LOW_SPEED_Pin, FALSE);
	HAL_GPIO_WritePin(HIGH_SPEED_GPIO_Port, HIGH_SPEED_Pin, TRUE);
}

void enterWaitForSlowSpeed(void)
{
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Oczekiwanie na\nosiagniecie zadanej\npredkosci", Font_11x18, BLACK, WHITE);
}

void enterWaitForLoosers(void)
{
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Oczekiwanie na zanik\nsygnalu luzownikow", Font_11x18, BLACK, WHITE);
}

void enterWaitForFastSpeed(void)
{
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Oczekiwanie na\nosiagniecie zadanej\npredkosci", Font_11x18, BLACK, WHITE);
}

void okButtonFunctionTeachMenu(void *param)
{
	switch(teachStateMachine)
	{
	case PREPARATION:
		teachStateMachine = WAIT_FOR_LOOSERS;
		enterWaitForLoosers();
		break;

	case SLOW_SPEED_CONFIRMATION:
		setEngineOnOff(TRUE);
		enableFastSpeedTeach();
		setTeachSlow(FALSE);
		setTeachFast(TRUE);
		teachStateMachine = WAIT_FOR_FAST_SPEED;
		startSoftwareTimer(&waitForSpeedTimer);
		enterWaitForFastSpeed();
		break;

	case FAST_SPEED_CONFIRMATION:
		setEngineOnOff(FALSE);
		setTeachSlow(FALSE);
		setTeachFast(FALSE);
		stopEngine();
		teachStateMachine = END;
		enterEndState();
		break;

	case ERROR_STATE:
		if(errorStateActive)
		{
			activeMenu = MAIN_MENU;
			teachStateMachine = START_STATE;
			initMainForm();
		}
		else if(previousStateMachine < SLOW_SPEED_CONFIRMATION)
		{
			teachStateMachine = START_STATE;
		}
		else
		{
			teachStateMachine = SLOW_SPEED_CONFIRMATION;
			setTeachFast(FALSE);
			setTeachSlow(FALSE);
			enterSlowSpeedConfirmation();
		}
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
		teachStateMachine = WAIT_FOR_LOOSERS;
		enterWaitForLoosers();
		break;

	case FAST_SPEED_CONFIRMATION:
		setEngineOnOff(TRUE);
		enableFastSpeedTeach();
		setTeachSlow(FALSE);
		setTeachFast(TRUE);
		teachStateMachine = WAIT_FOR_FAST_SPEED;
		restartRotationsTmp();
		startSoftwareTimer(&waitForSpeedTimer);
		enterWaitForFastSpeed();
		break;

	default:
		break;
	}
}

void upButtonFunctionTeachMenu(void *param)
{


}

void downButtonFunctionTeachMenu(void *param)
{

}

void endTeachingTimerCallback(void *param)
{
	//Callback is necessery, but is not needed
}

void calculateTeachedSpeeds(bool slowFast)
{
	if(slowFast)
	{
		rotationsPerMinuteGiven.engine.fastTime = rotationsPerMinuteGiven.engine.fastTime/10;
		rotationsPerMinuteGiven.handrail.fastTime = rotationsPerMinuteGiven.handrail.fastTime/10;
		rotationsPerMinuteGiven.step.fastTime = gapsBetweenSteps/stepsCounted;
	}
	else
	{
		rotationsPerMinuteGiven.engine.slowTime = rotationsPerMinuteGiven.engine.slowTime/10;
		rotationsPerMinuteGiven.handrail.slowTime = rotationsPerMinuteGiven.handrail.slowTime/10;
		rotationsPerMinuteGiven.step.slowTime = gapsBetweenSteps/stepsCounted;
	}

	gapsBetweenSteps = 0;
	stepsCounted = 0;

}

void enterRestartDriverState(void)
{
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Aby przejsc do trybu nauki predkosci,\nzrestartuj urzadzenie", Font_11x18, BLACK, WHITE);
	ST7789_WriteString(2, 265, "Aby wyjsc, wylacz\nteachSwitch", Font_11x18, BLACK, WHITE);
}

void enterPreparationState(void)
{
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Aby rozpoczac nauke\npowolnej predkosci,\nnacisnij OK", Font_11x18, BLACK, WHITE);
}

void enterSlowSpeedTime(void)
{
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Sterownik uczy sie\nwolnej predkosci", Font_11x18, BLACK, WHITE);
}

void enterFastSpeedTime(void)
{
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Sterownik uczy sie\nszybkiej predkosci", Font_11x18, BLACK, WHITE);
}

void enterErrorState(void)
{
	teachStateMachine = ERROR_STATE;
	setEngineOnOff(FALSE);
	setTeachSlow(FALSE);
	setTeachFast(FALSE);
	stopEngine();
	ST7789_Fill_Color(WHITE);

	if(errorStateActive)
	{
		hasErrorOccured = TRUE;
		ST7789_WriteString(2, 5, "Pojawil sie blad.\nNapraw blad aby\nkontynuowac.", Font_11x18, BLACK, WHITE);
		ST7789_WriteString(2, 245, "Nacisnij ok, aby\nprzejsc do\nglownego menu.", Font_11x18, BLACK, WHITE);
	}
	else
	{
		ST7789_WriteString(2, 5, "Czas oczekiwania\nna zadana predkosc\nprzekroczyl limit.", Font_11x18, BLACK, WHITE);
		ST7789_WriteString(2, 265, "Nacisnij ok, aby\npowtorzyc", Font_11x18, BLACK, WHITE);
	}
}

void errorStateFunction(void)
{
	if(errorStateActive && !hasErrorOccured)
	{
		ST7789_Fill_Color(WHITE);
		hasErrorOccured = TRUE;
		ST7789_WriteString(2, 5, "Pojawil sie blad.\nNapraw blad aby\nkontynuowac.", Font_11x18, BLACK, WHITE);
		ST7789_WriteString(2, 245, "Nacisnij ok, aby\nprzejsc do\nglownego menu.", Font_11x18, BLACK, WHITE);
	}
	else if(hasErrorOccured && !errorStateActive)
	{
		ST7789_Fill_Color(WHITE);
		ST7789_WriteString(2, 5, "Blad zostal\nnaprawiony.", Font_11x18, BLACK, WHITE);
		ST7789_WriteString(2, 265, "Nacisnij ok, aby\npowtorzyc", Font_11x18, BLACK, WHITE);
		hasErrorOccured = FALSE;
	}
}

void enterFastSpeedConfirmation(void)
{
	char buffer[30];
	ST7789_Fill_Color(WHITE);
	ST7789_WriteString(2, 5, "Pomiary szybkiej\npredkosci:", Font_11x18, BLACK, WHITE);
	sprintf(buffer, "Silnik: %u obr/s", rotationsPerMinuteGiven.engine.fastTime);
	ST7789_WriteString(2, 50, buffer, Font_11x18, BLACK, WHITE);
	sprintf(buffer, "Porecz: %u obr/s", rotationsPerMinuteGiven.handrail.fastTime);
	ST7789_WriteString(2, 73, buffer, Font_11x18, BLACK, WHITE);
	sprintf(buffer, "Stopnie: %u ms", rotationsPerMinuteGiven.step.fastTime);
	ST7789_WriteString(2, 96, buffer, Font_11x18, BLACK, WHITE);
	ST7789_WriteString(2, 265, "ESC -> Powtorz", Font_11x18, BLACK, WHITE);
	ST7789_WriteString(2, 285, "OK -> Zapisz", Font_11x18, BLACK, WHITE);
}

void stateMachineSubTask(void)
{
	if(errorStateActive && teachStateMachine != ERROR_STATE && teachStateMachine != ENTER_ERROR_STATE)
	{
		previousStateMachine = teachStateMachine;
		teachStateMachine = ENTER_ERROR_STATE;
	}

	switch(teachStateMachine)
	{
		case START_STATE:

			upButtonFunction = upButtonFunctionTeachMenu;
			downButtonFunction = downButtonFunctionTeachMenu;
			okButtonFunction = okButtonFunctionTeachMenu;
			escButtonFunction = escButtonFunctionTeachMenu;

			if(teachOnStartup)
			{
				teachStateMachine = PREPARATION;
				initSoftwareTimer(&waitForSpeedTimer, WAIT_FOR_SPEED_TIME, waitForSpeedTimerCallback, FALSE, 0);
				enterPreparationState();
			}
			else
			{
				teachStateMachine = RESTART_DRIVER;
				enterRestartDriverState();
			}
			break;

		case RESTART_DRIVER:
			break;

		case PREPARATION:
			break;

		case WAIT_FOR_LOOSERS:
			if(!getLoosersState())
			{
				setEngineOnOff(TRUE);
				enableSlowSpeedTeach();
				setTeachFast(FALSE);
				setTeachSlow(TRUE);
				teachStateMachine = WAIT_FOR_SLOW_SPEED;
				startSoftwareTimer(&waitForSpeedTimer);
				enterWaitForSlowSpeed();
			}
			break;

		case WAIT_FOR_SLOW_SPEED:
			if(checkTargetFrequencyReached())// && getSpeedReady())
			{
				teachStateMachine = SLOW_SPEED_TIME;
				enterSlowSpeedTime();
				initSoftwareTimer(&teachTimer, TEACH_TIME, saveMeasuredRotationsValueTimerCallback, FALSE, &rotationsPerMinuteGiven);
				initSoftwareTimer(&endTeachingTimer, END_TEACH_TIME, endTeachingTimerCallback, FALSE, &rotationsPerMinuteGiven);

				startSoftwareTimer(&teachTimer);
				startSoftwareTimer(&endTeachingTimer);
				stopSoftwareTimer(&waitForSpeedTimer);

			}
			break;

		case SLOW_SPEED_TIME:
			if(endTeachingTimer.start == FALSE)// && getTeachingEnd())
			{
				teachStateMachine = SLOW_SPEED_CONFIRMATION;
				calculateTeachedSpeeds(FALSE);
				enterSlowSpeedConfirmation();

				setTeachFast(FALSE);
				setTeachSlow(FALSE);
			}
			break;

		case SLOW_SPEED_CONFIRMATION:
			//This step is handled by buttons
			break;


		case WAIT_FOR_FAST_SPEED:
			if(checkTargetFrequencyReached())// && getSpeedReady())
			{
				teachStateMachine = FAST_SPEED_TIME;
				enterFastSpeedTime();

				startSoftwareTimer(&teachTimer);
				startSoftwareTimer(&endTeachingTimer);
				stopSoftwareTimer(&waitForSpeedTimer);
			}
			break;

		case FAST_SPEED_TIME:
			if(endTeachingTimer.start == FALSE)// && getTeachingEnd())
			{
				teachStateMachine = FAST_SPEED_CONFIRMATION;
				calculateTeachedSpeeds(TRUE);
				enterFastSpeedConfirmation();
				setTeachFast(FALSE);
				setTeachSlow(FALSE);
			}
			break;

		case FAST_SPEED_CONFIRMATION:
			//This step is handled by buttons
			break;

		case END:
			rotationsSaveParameters();
			break;

		case ENTER_ERROR_STATE:
			enterErrorState();
			break;

		case ERROR_STATE:
			errorStateFunction();
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
				gapBetweenStepsTemp = xTaskGetTickCount() - gapBetweenStepsTemp;
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
				gapBetweenStepsTemp = xTaskGetTickCount() - gapBetweenStepsTemp;
				gapsBetweenSteps += gapBetweenStepsTemp;
				gapBetweenStepsTemp = 0;
			}
			else
			{
				gapBetweenStepsTemp = xTaskGetTickCount();
				stepsCounted++;
				steps1 = TRUE;
			}
		}
	}
}

void enterTeachingForm(void)
{
	if(getTeachInput())
	{
		if(!errorStateActive)
		{
			activeMenu = TEACHING_MENU;
		}
	}
	else if(activeMenu == TEACHING_MENU)
	{
		activeMenu = MAIN_MENU;
		teachStateMachine = START_STATE;
		initMainForm();
	}
}


#endif /* MENUS_TEACHFORM_C_ */
