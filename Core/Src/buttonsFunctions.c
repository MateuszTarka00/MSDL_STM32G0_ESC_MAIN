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

#define BUTTON_UP		0x01
#define BUTTON_DOWN		0x02
#define BUTTON_OK		0x04
#define BUTTON_ESC		0x08

static volatile uint8_t buttonsPressed = 0;
static volatile uint8_t buttonsPulled = 0;

ButtonDebounce_t btnUp     = { 0, 0, 0 };
ButtonDebounce_t btnDown   = { 0, 0, 0 };
ButtonDebounce_t btnOk     = { 0, 0, 0 };
ButtonDebounce_t btnEsc    = { 0, 0, 0 };


void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	BaseType_t hpw = pdFALSE;
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
            upButtonFunction();
        }
        btnUp.pendingRelease = FALSE;
    }

    if(btnDown.pendingRelease &&
       (now - btnDown.lastTick) >= pdMS_TO_TICKS(DEBOUNCE_TIME_MS))
    {
        if(HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_Port, BUTTON_DOWN_Pin) == GPIO_PIN_RESET)
        {
        	downButtonFunction();
        }
        btnDown.pendingRelease = FALSE;
    }

    if(btnOk.pendingRelease &&
       (now - btnOk.lastTick) >= pdMS_TO_TICKS(DEBOUNCE_TIME_MS))
    {
        if(HAL_GPIO_ReadPin(BUTTON_OK_GPIO_Port, BUTTON_OK_Pin) == GPIO_PIN_RESET)
        {
        	okButtonFunction();
        }
        btnOk.pendingRelease = FALSE;
    }

    if(btnEsc.pendingRelease &&
       (now - btnEsc.lastTick) >= pdMS_TO_TICKS(DEBOUNCE_TIME_MS))
    {
        if(HAL_GPIO_ReadPin(BUTTON_ESC_GPIO_Port, BUTTON_ESC_Pin) == GPIO_PIN_RESET)
        {
        	escButtonFunction();
        }
        btnEsc.pendingRelease = FALSE;
    }

}








