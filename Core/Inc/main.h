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
#define CAN_FAULT_Pin GPIO_PIN_4
#define CAN_FAULT_GPIO_Port GPIOA
#define CAN_OK_Pin GPIO_PIN_5
#define CAN_OK_GPIO_Port GPIOA
#define SAFETY_END_Pin GPIO_PIN_12
#define SAFETY_END_GPIO_Port GPIOD
#define IN_FREQ_Pin GPIO_PIN_9
#define IN_FREQ_GPIO_Port GPIOC
#define IN_THERMAL_Pin GPIO_PIN_4
#define IN_THERMAL_GPIO_Port GPIOD
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

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
