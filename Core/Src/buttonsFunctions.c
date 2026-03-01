/*
 * buttonsFunctions.c
 *
 *  Created on: 9 lut 2026
 *      Author: mateo
 */

#include "buttonsFunctions.h"
#include "settingsForm.h"
#include "st7789.h"
#include "task.h"
#include "cmsis_os.h"
#include "engineFunctions.h"

#define BUTTON_UP		0x01
#define BUTTON_DOWN		0x02
#define BUTTON_OK		0x04
#define BUTTON_ESC		0x08

#define ROTATION_H1		0x01
#define ROTATION_S1		0x02
#define STEP_1			0x04
#define STEP_2			0x08

static volatile uint8_t buttonsPressed = 0;
static volatile uint8_t buttonsPulled = 0;

static volatile uint8_t inputPressed = 0;
static volatile uint8_t inputPulled = 0;

InputDebounce_t btnUp     = { 0, 0, 0 };
InputDebounce_t btnDown   = { 0, 0, 0 };
InputDebounce_t btnOk     = { 0, 0, 0 };
InputDebounce_t btnEsc    = { 0, 0, 0 };

InputDebounce_t rotH1     = { 0, 0, 0 };
InputDebounce_t rotS1     = { 0, 0, 0 };
InputDebounce_t st1       = { 0, 0, 0 };
InputDebounce_t st2       = { 0, 0, 0 };

void (*upButtonFunction)(void *) = 0;
void (*downButtonFunction)(void *) = 0;
void (*okButtonFunction)(void *) = 0;
void (*escButtonFunction)(void *) = 0;

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	BaseType_t hpw = pdFALSE;
	if(GPIO_Pin == BUTTON_DOWN_Pin || GPIO_Pin == BUTTON_OK_Pin || GPIO_Pin == BUTTON_ESC_Pin || GPIO_Pin == BUTTON_UP_Pin)
	{
		switch(GPIO_Pin)
		{
			case BUTTON_UP_Pin:
				buttonsPressed |= BUTTON_UP;
				break;
			case BUTTON_DOWN_Pin:
				buttonsPressed |= BUTTON_DOWN;
				break;
			case BUTTON_OK_Pin:
				buttonsPressed |= BUTTON_OK;
				break;
			case BUTTON_ESC_Pin:
				buttonsPressed |= BUTTON_ESC;
				break;
		}
		vTaskNotifyGiveFromISR(displayTaskTHandle, &hpw);
	}

	if(GPIO_Pin == ROTATION_H1_Pin || GPIO_Pin == ROTATION_S1_Pin || GPIO_Pin == MIS_ST2_Pin || GPIO_Pin == MIS_ST1_Pin) //TODO dodac porecz 2
	{

		switch(GPIO_Pin)
		{
			case ROTATION_H1_Pin:
				inputPressed |= ROTATION_H1;
				break;
			case ROTATION_S1_Pin:
				inputPressed |= ROTATION_S1;
				break;
			case MIS_ST2_Pin:
				inputPressed |= STEP_2;
				break;
			case MIS_ST1_Pin:
				inputPressed |= STEP_1;
				break;
		}
		vTaskNotifyGiveFromISR(engineControlTHandle, &hpw);
	}
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
	BaseType_t hpw = pdFALSE;
	switch(GPIO_Pin)
	{
		case BUTTON_UP_Pin:
			buttonsPulled |= BUTTON_UP;
			break;
		case BUTTON_DOWN_Pin:
			buttonsPulled |= BUTTON_DOWN;
			break;
		case BUTTON_OK_Pin:
			buttonsPulled |= BUTTON_OK;
			break;
		case BUTTON_ESC_Pin:
			buttonsPulled |= BUTTON_ESC;
			break;
	}
	vTaskNotifyGiveFromISR(displayTaskTHandle, &hpw);
}

void buttonsSubTask(void)
{
	TickType_t now = xTaskGetTickCount();

	if(buttonsPressed)
	{
		if(buttonsPressed & BUTTON_UP)
		{
			btnUp.lastTick = now;
			btnUp.pendingPress = TRUE;
			buttonsPressed &= ~BUTTON_UP;
		}

		if(buttonsPressed & BUTTON_DOWN)
		{
			btnDown.lastTick = now;
			btnDown.pendingPress = TRUE;
			buttonsPressed &= ~BUTTON_DOWN;
		}

		if(buttonsPressed & BUTTON_OK)
		{
			btnOk.lastTick = now;
			btnOk.pendingPress = TRUE;
			buttonsPressed &= ~BUTTON_OK;
		}

		if(buttonsPressed & BUTTON_ESC)
		{
			btnEsc.lastTick = now;
			btnEsc.pendingPress = TRUE;
			buttonsPressed &= ~BUTTON_ESC;
		}
	}

	if(buttonsPulled)
	{
		if(buttonsPulled & BUTTON_UP)
		{
			btnUp.lastTick = now;
			btnUp.pendingRelease = TRUE;
			buttonsPulled &= ~BUTTON_UP;
		}

		if(buttonsPulled & BUTTON_DOWN)
		{
			btnDown.lastTick = now;
			btnDown.pendingRelease = TRUE;
			buttonsPulled &= ~BUTTON_DOWN;
		}

		if(buttonsPulled & BUTTON_OK)
		{
			btnOk.lastTick = now;
			btnOk.pendingRelease = TRUE;
			buttonsPulled &= ~BUTTON_OK;
		}

		if(buttonsPulled & BUTTON_ESC)
		{
			btnEsc.lastTick = now;
			btnEsc.pendingRelease = TRUE;
			buttonsPulled &= ~BUTTON_ESC;
		}
	}

    if(btnUp.pendingRelease &&
       (now - btnUp.lastTick) >= pdMS_TO_TICKS(DEBOUNCE_TIME_MS))
    {
        if(HAL_GPIO_ReadPin(BUTTON_UP_GPIO_Port, BUTTON_UP_Pin) == GPIO_PIN_RESET)
        {
            if(upButtonFunction != 0)
            {
            	upButtonFunction(0);
            }
        }
        btnUp.pendingRelease = FALSE;
    }

    if(btnDown.pendingRelease &&
       (now - btnDown.lastTick) >= pdMS_TO_TICKS(DEBOUNCE_TIME_MS))
    {
        if(HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_Port, BUTTON_DOWN_Pin) == GPIO_PIN_RESET)
        {
        	if(downButtonFunction != 0)
        	{
        		downButtonFunction(0);
        	}
        }
        btnDown.pendingRelease = FALSE;
    }

    if(btnOk.pendingRelease &&
       (now - btnOk.lastTick) >= pdMS_TO_TICKS(DEBOUNCE_TIME_MS))
    {
        if(HAL_GPIO_ReadPin(BUTTON_OK_GPIO_Port, BUTTON_OK_Pin) == GPIO_PIN_RESET)
        {
        	if(okButtonFunction != 0)
        	{
        		okButtonFunction(0);
        	}
        }
        btnOk.pendingRelease = FALSE;
    }

    if(btnEsc.pendingRelease &&
       (now - btnEsc.lastTick) >= pdMS_TO_TICKS(DEBOUNCE_TIME_MS))
    {
        if(HAL_GPIO_ReadPin(BUTTON_ESC_GPIO_Port, BUTTON_ESC_Pin) == GPIO_PIN_RESET)
        {
        	if(escButtonFunction != 0)
        	{
        		escButtonFunction(0);
        	}
        }
        btnEsc.pendingRelease = FALSE;
    }

}

void rotationSubTask(void)
{
    TickType_t now = xTaskGetTickCount();

    /* ----------- HANDLE PRESSED FLAGS FROM ISR ----------- */

    if(inputPressed)
    {
        if(inputPressed & ROTATION_H1)
        {
            rotH1.lastTick = now;
            rotH1.pendingPress = TRUE;
            inputPressed &= ~ROTATION_H1;
        }

        if(inputPressed & ROTATION_S1)
        {
            rotS1.lastTick = now;
            rotS1.pendingPress = TRUE;
            inputPressed &= ~ROTATION_S1;
        }

        if(inputPressed & STEP_1)
        {
            st1.lastTick = now;
            st1.pendingPress = TRUE;
            inputPressed &= ~STEP_1;
        }

        if(inputPressed & STEP_2)
        {
            st2.lastTick = now;
            st2.pendingPress = TRUE;
            inputPressed &= ~STEP_2;
        }
    }

    /* ----------- DEBOUNCE PRESS ONLY ----------- */

    if(rotH1.pendingPress &&
       (now - rotH1.lastTick) >= pdMS_TO_TICKS(DEBOUNCE_TIME_ROTATION_MS))
    {
        if(HAL_GPIO_ReadPin(ROTATION_H1_GPIO_Port, ROTATION_H1_Pin) == GPIO_PIN_SET)
        {
    		incrementRotationsNumber(ROTATION_H1_Pin);
        }
        rotH1.pendingPress = FALSE;
    }

    if(rotS1.pendingPress &&
       (now - rotS1.lastTick) >= pdMS_TO_TICKS(DEBOUNCE_TIME_ROTATION_MS))
    {
        if(HAL_GPIO_ReadPin(ROTATION_S1_GPIO_Port, ROTATION_S1_Pin) == GPIO_PIN_SET)
        {
        	incrementRotationsNumber(ROTATION_S1_Pin);
        }
        rotS1.pendingPress = FALSE;
    }

    if(st2.pendingPress &&
       (now - st2.lastTick) >= pdMS_TO_TICKS(DEBOUNCE_TIME_ROTATION_MS))
    {
        if(HAL_GPIO_ReadPin(MIS_ST2_GPIO_Port, MIS_ST2_Pin) == GPIO_PIN_SET)
        {
        	stepsTeachExtiCallback(MIS_ST2_Pin);
        }
        st2.pendingPress = FALSE;
    }

    if(st1.pendingPress &&
       (now - st1.lastTick) >= pdMS_TO_TICKS(DEBOUNCE_TIME_ROTATION_MS))
    {
        if(HAL_GPIO_ReadPin(MIS_ST1_GPIO_Port, MIS_ST1_Pin) == GPIO_PIN_SET)
        {
        	stepsTeachExtiCallback(MIS_ST1_Pin);
        }
        st1.pendingPress = FALSE;
    }
}











