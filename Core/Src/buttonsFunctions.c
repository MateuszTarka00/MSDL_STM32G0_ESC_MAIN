/*
 * buttonsFunctions.c
 *
 *  Created on: 9 lut 2026
 *      Author: mateo
 */

#include "buttonsFunctions.h"
#include "settingsForm.h"
#include "st7789.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#define BUTTON_UP		0x01
#define BUTTON_DOWN		0x02
#define BUTTON_OK		0x04
#define BUTTON_ESC		0x08

//static uint32_t lastTick = 0;
#define BUTTON_UP_Pin GPIO_PIN_5
#define BUTTON_UP_GPIO_Port GPIOD
#define BUTTON_UP_EXTI_IRQn EXTI4_15_IRQn
#define BUTTON_DOWN_Pin GPIO_PIN_6
#define BUTTON_DOWN_GPIO_Port GPIOD
#define BUTTON_DOWN_EXTI_IRQn EXTI4_15_IRQn
#define BUTTON_ESC_Pin GPIO_PIN_7
#define BUTTON_ESC_GPIO_Port GPIOD
#define BUTTON_ESC_EXTI_IRQn EXTI4_15_IRQn
#define BUTTON_OK_Pin GPIO_PIN_9
#define BUTTON_OK_GPIO_Port GPIOF
#define BUTTON_OK_EXTI_IRQn EXTI4_15_IRQn

static volatile uint8_t buttonsPressed = 0;
static volatile uint8_t buttonsPulled = 0;

//    uint32_t now = xTaskGetTickCountFromISR();
//
//    if (GPIO_Pin == BUTTON_Pin && (now - lastTick) > pdMS_TO_TICKS(50))
//    {
//        lastTick = now;
//
//        BaseType_t hpw = pdFALSE;
//        vTaskNotifyGiveFromISR(displayTaskTHandle, &hpw);
//        portYIELD_FROM_ISR(hpw);
//    }


void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	//TODO debouncing needs to be done
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
	if(buttonsPressed)
	{
		if(buttonsPressed & BUTTON_UP)
		{
			buttonsPressed &= ~BUTTON_UP;
		}

		if(buttonsPressed & BUTTON_DOWN)
		{
			buttonsPressed &= ~BUTTON_DOWN;
		}

		if(buttonsPressed & BUTTON_OK)
		{
			buttonsPressed &= ~BUTTON_OK;
		}

		if(buttonsPressed & BUTTON_ESC)
		{
			buttonsPressed &= ~BUTTON_ESC;
		}
	}

	if(buttonsPulled)
	{
		if(buttonsPulled & BUTTON_UP)
		{
			buttonsPulled &= ~BUTTON_UP;
			goToPreviousItem();
		}

		if(buttonsPulled & BUTTON_DOWN)
		{
			buttonsPulled &= ~BUTTON_DOWN;
			goToNextItem();
		}

		if(buttonsPulled & BUTTON_OK)
		{
			buttonsPulled &= ~BUTTON_OK;
		}

		if(buttonsPulled & BUTTON_ESC)
		{
			buttonsPulled &= ~BUTTON_ESC;
		}
	}

}








