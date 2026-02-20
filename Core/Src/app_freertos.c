/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "safetyCircuit.h"
#include "buttonsFunctions.h"
#include "settingsForm.h"
#include "mainForm.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for safetyCheckT */
osThreadId_t safetyCheckTHandle;
const osThreadAttr_t safetyCheckT_attributes = {
  .name = "safetyCheckT",
  .priority = (osPriority_t) osPriorityNormal2,
  .stack_size = 512 * 4
};
/* Definitions for canMenagerT */
osThreadId_t canMenagerTHandle;
const osThreadAttr_t canMenagerT_attributes = {
  .name = "canMenagerT",
  .priority = (osPriority_t) osPriorityNormal3,
  .stack_size = 1024 * 4
};
/* Definitions for displayTaskT */
osThreadId_t displayTaskTHandle;
const osThreadAttr_t displayTaskT_attributes = {
  .name = "displayTaskT",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};
/* Definitions for engineControlT */
osThreadId_t engineControlTHandle;
const osThreadAttr_t engineControlT_attributes = {
  .name = "engineControlT",
  .priority = (osPriority_t) osPriorityNormal1,
  .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void safetyCheck(void *argument);
void canMenager(void *argument);
void displayTask(void *argument);
void engineControl(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationIdleHook(void);
void vApplicationTickHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);
void vApplicationDaemonTaskStartupHook(void);

/* USER CODE BEGIN 2 */
void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/* USER CODE BEGIN 3 */
void vApplicationTickHook( void )
{
   /* This function will be called by each tick interrupt if
   configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
   added here, but the tick hook is called from an interrupt context, so
   code must not attempt to block, and only the interrupt safe FreeRTOS API
   functions can be used (those that end in FromISR()). */
}
/* USER CODE END 3 */

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void)
{
   /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
}
/* USER CODE END 5 */

/* USER CODE BEGIN DAEMON_TASK_STARTUP_HOOK */
void vApplicationDaemonTaskStartupHook(void)
{
}
/* USER CODE END DAEMON_TASK_STARTUP_HOOK */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of safetyCheckT */
  safetyCheckTHandle = osThreadNew(safetyCheck, NULL, &safetyCheckT_attributes);

  /* creation of canMenagerT */
  canMenagerTHandle = osThreadNew(canMenager, NULL, &canMenagerT_attributes);

  /* creation of displayTaskT */
  displayTaskTHandle = osThreadNew(displayTask, NULL, &displayTaskT_attributes);

  /* creation of engineControlT */
  engineControlTHandle = osThreadNew(engineControl, NULL, &engineControlT_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_safetyCheck */
/**
  * @brief  Function implementing the safetyCheckT thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_safetyCheck */
void safetyCheck(void *argument)
{
  /* USER CODE BEGIN safetyCheck */
  /* Infinite loop */
  for(;;)
  {
	  if(!checkSafetyCircuitState())
	  {
		  safetyCircuitPoint = checkBrokenSafetyCircuitPoint();
	  }
	  else
	  {
		  safetyCircuitPoint = SAFETY_CIRCUIT_UNBROKEN;
	  }

	  setSafetyCircuitStateOutput();
	  osDelay(1);
  }
  /* USER CODE END safetyCheck */
}

/* USER CODE BEGIN Header_canMenager */
/**
* @brief Function implementing the canMenagerT thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_canMenager */
void canMenager(void *argument)
{
  /* USER CODE BEGIN canMenager */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END canMenager */
}

/* USER CODE BEGIN Header_displayTask */
/**
* @brief Function implementing the displayTaskT thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_displayTask */
void displayTask(void *argument)
{
  /* USER CODE BEGIN displayTask */
  /* Infinite loop */
//	backToParentMenu();
	initMainForm();
  for(;;)
  {
	mainMenuSubTask();
	buttonsSubTask();
    osDelay(1);
  }
  /* USER CODE END displayTask */
}

/* USER CODE BEGIN Header_engineControl */
/**
* @brief Function implementing the engineControlT thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_engineControl */
void engineControl(void *argument)
{
  /* USER CODE BEGIN engineControl */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END engineControl */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
