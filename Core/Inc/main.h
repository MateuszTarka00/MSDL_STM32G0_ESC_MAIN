/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"
#include "cmsis_os2.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define __HAL_GPIO_EXTI_DISABLE_IT(__EXTI_LINE__) \
  (EXTI->IMR1 &= ~(__EXTI_LINE__))

#define __HAL_GPIO_EXTI_ENABLE_IT(__EXTI_LINE__) \
  (EXTI->IMR1 |= (__EXTI_LINE__))
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SAFETY_C4_Pin GPIO_PIN_9
#define SAFETY_C4_GPIO_Port GPIOB
#define SAFETY_C5_Pin GPIO_PIN_10
#define SAFETY_C5_GPIO_Port GPIOC
#define SAFETY_C6_Pin GPIO_PIN_11
#define SAFETY_C6_GPIO_Port GPIOC
#define SAFETY_C7_Pin GPIO_PIN_4
#define SAFETY_C7_GPIO_Port GPIOE
#define SAFETY_C8_Pin GPIO_PIN_5
#define SAFETY_C8_GPIO_Port GPIOE
#define TEACH_INPUT_Pin GPIO_PIN_6
#define TEACH_INPUT_GPIO_Port GPIOE
#define STOP_TIME2_CONTROL_Pin GPIO_PIN_12
#define STOP_TIME2_CONTROL_GPIO_Port GPIOC
#define STOP_TIME1_CONTROL_Pin GPIO_PIN_13
#define STOP_TIME1_CONTROL_GPIO_Port GPIOC
#define CONTACTORS_CONTROL_Pin GPIO_PIN_14
#define CONTACTORS_CONTROL_GPIO_Port GPIOC
#define AUTOSTOP_CONTROL_Pin GPIO_PIN_15
#define AUTOSTOP_CONTROL_GPIO_Port GPIOC
#define LOCKER_CONTROL_Pin GPIO_PIN_3
#define LOCKER_CONTROL_GPIO_Port GPIOF
#define RELEASE_CONTROL_Pin GPIO_PIN_4
#define RELEASE_CONTROL_GPIO_Port GPIOF
#define STAR_TRIANGLE_CONTROL_Pin GPIO_PIN_5
#define STAR_TRIANGLE_CONTROL_GPIO_Port GPIOF
#define INVERTER_CONTROL_Pin GPIO_PIN_0
#define INVERTER_CONTROL_GPIO_Port GPIOC
#define K1_EN_Pin GPIO_PIN_0
#define K1_EN_GPIO_Port GPIOA
#define STAND_CONTROL_Pin GPIO_PIN_1
#define STAND_CONTROL_GPIO_Port GPIOA
#define HANDRAIL_CONTROL_Pin GPIO_PIN_2
#define HANDRAIL_CONTROL_GPIO_Port GPIOA
#define ROTATION_CONTROL_Pin GPIO_PIN_3
#define ROTATION_CONTROL_GPIO_Port GPIOA
#define CAN_FAULT_Pin GPIO_PIN_4
#define CAN_FAULT_GPIO_Port GPIOA
#define CAN_OK_Pin GPIO_PIN_5
#define CAN_OK_GPIO_Port GPIOA
#define TEACH_OUTPUT_Pin GPIO_PIN_6
#define TEACH_OUTPUT_GPIO_Port GPIOA
#define WATCHDOG_OUT_Pin GPIO_PIN_4
#define WATCHDOG_OUT_GPIO_Port GPIOC
#define ACK_CONT_Pin GPIO_PIN_5
#define ACK_CONT_GPIO_Port GPIOC
#define CHAIN_MOT_Pin GPIO_PIN_0
#define CHAIN_MOT_GPIO_Port GPIOB
#define MIS_ST2_Pin GPIO_PIN_1
#define MIS_ST2_GPIO_Port GPIOB
#define MIS_ST2_EXTI_IRQn EXTI0_1_IRQn
#define MIS_ST1_Pin GPIO_PIN_2
#define MIS_ST1_GPIO_Port GPIOB
#define MIS_ST1_EXTI_IRQn EXTI2_3_IRQn
#define LOCKER_IN2_Pin GPIO_PIN_6
#define LOCKER_IN2_GPIO_Port GPIOF
#define LOCKER_IN1_Pin GPIO_PIN_7
#define LOCKER_IN1_GPIO_Port GPIOF
#define ROTATION_H1_Pin GPIO_PIN_8
#define ROTATION_H1_GPIO_Port GPIOE
#define ROTATION_H1_EXTI_IRQn EXTI4_15_IRQn
#define ROTATION_S1_Pin GPIO_PIN_11
#define ROTATION_S1_GPIO_Port GPIOE
#define ROTATION_S1_EXTI_IRQn EXTI4_15_IRQn
#define LCD_SCK_Pin GPIO_PIN_13
#define LCD_SCK_GPIO_Port GPIOB
#define LCD_MISO_Pin GPIO_PIN_14
#define LCD_MISO_GPIO_Port GPIOB
#define LCD_MOSI_Pin GPIO_PIN_15
#define LCD_MOSI_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_8
#define LCD_CS_GPIO_Port GPIOA
#define LCD_DC_Pin GPIO_PIN_9
#define LCD_DC_GPIO_Port GPIOA
#define SAFETY_END_Pin GPIO_PIN_12
#define SAFETY_END_GPIO_Port GPIOD
#define LCD_RST_Pin GPIO_PIN_10
#define LCD_RST_GPIO_Port GPIOA
#define LCD_BKL_Pin GPIO_PIN_8
#define LCD_BKL_GPIO_Port GPIOF
#define HUMAN_UP_Pin GPIO_PIN_8
#define HUMAN_UP_GPIO_Port GPIOC
#define IN_FREQ_Pin GPIO_PIN_9
#define IN_FREQ_GPIO_Port GPIOC
#define IN_THERMAL_Pin GPIO_PIN_4
#define IN_THERMAL_GPIO_Port GPIOD
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
#define CHECK_UP_Pin GPIO_PIN_10
#define CHECK_UP_GPIO_Port GPIOF
#define CHECK_DOWN_Pin GPIO_PIN_11
#define CHECK_DOWN_GPIO_Port GPIOF
#define INSPECTION_MODE_Pin GPIO_PIN_12
#define INSPECTION_MODE_GPIO_Port GPIOF
#define INVERTER_FWD_Pin GPIO_PIN_13
#define INVERTER_FWD_GPIO_Port GPIOF
#define INVERTER_REV_Pin GPIO_PIN_3
#define INVERTER_REV_GPIO_Port GPIOB
#define LOW_SPEED_Pin GPIO_PIN_4
#define LOW_SPEED_GPIO_Port GPIOB
#define HIGH_SPEED_Pin GPIO_PIN_5
#define HIGH_SPEED_GPIO_Port GPIOB
#define CUSTOM_OUT_Pin GPIO_PIN_0
#define CUSTOM_OUT_GPIO_Port GPIOE
#define TOEBOARD_LIGHT_Pin GPIO_PIN_1
#define TOEBOARD_LIGHT_GPIO_Port GPIOE
#define SIGNAL_RED_Pin GPIO_PIN_2
#define SIGNAL_RED_GPIO_Port GPIOE
#define SIGNAL_GREEN_Pin GPIO_PIN_3
#define SIGNAL_GREEN_GPIO_Port GPIOE
#define SAFETY_C1_Pin GPIO_PIN_6
#define SAFETY_C1_GPIO_Port GPIOB
#define SAFETY_C2_Pin GPIO_PIN_7
#define SAFETY_C2_GPIO_Port GPIOB
#define SAFETY_C3_Pin GPIO_PIN_8
#define SAFETY_C3_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
typedef enum
{
	FALSE,
	TRUE
}bool;

extern osThreadId_t displayTaskTHandle;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
