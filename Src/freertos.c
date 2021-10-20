/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticEventGroup_t osStaticEventGroupDef_t;
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
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
uint32_t defaultTaskBuffer[ 4096 ];
osStaticThreadDef_t defaultTaskControlBlock;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .cb_mem = &defaultTaskControlBlock,
  .cb_size = sizeof(defaultTaskControlBlock),
  .stack_mem = &defaultTaskBuffer[0],
  .stack_size = sizeof(defaultTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Audio_Task */
osThreadId_t Audio_TaskHandle;
uint32_t Audio_TaskBuffer[ 4096 ];
osStaticThreadDef_t Audio_TaskControlBlock;
const osThreadAttr_t Audio_Task_attributes = {
  .name = "Audio_Task",
  .cb_mem = &Audio_TaskControlBlock,
  .cb_size = sizeof(Audio_TaskControlBlock),
  .stack_mem = &Audio_TaskBuffer[0],
  .stack_size = sizeof(Audio_TaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Audio_Rec_Event */
osEventFlagsId_t Audio_Rec_EventHandle;
osStaticEventGroupDef_t Audio_Rec_EventControlBlock;
const osEventFlagsAttr_t Audio_Rec_Event_attributes = {
  .name = "Audio_Rec_Event",
  .cb_mem = &Audio_Rec_EventControlBlock,
  .cb_size = sizeof(Audio_Rec_EventControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartAudio_Task(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

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
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of Audio_Task */
  Audio_TaskHandle = osThreadNew(StartAudio_Task, NULL, &Audio_Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of Audio_Rec_Event */
  Audio_Rec_EventHandle = osEventFlagsNew(&Audio_Rec_Event_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
  User_Main_Init();
  /* Infinite loop */
  for(;;)
  {
    User_Main_Loop();
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartAudio_Task */
/**
* @brief Function implementing the Audio_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartAudio_Task */
void StartAudio_Task(void *argument)
{
  /* USER CODE BEGIN StartAudio_Task */
  /*定时自动时序条件*/
  static uint32_t Time_ms = 0;  
  /* Infinite loop */
  for(;;)
  {
    uint32_t Current_Time_ms = Timer_Port_Get_Current_Time(TIMER_MS);
    if((Current_Time_ms - Time_ms) >= (uint32_t)ONE_FRAME_TIME_MS)
    {
      osEventFlagsSet(Audio_Rec_EventHandle, AUDIO_SEND_OK_EVENT);
      Time_ms = Current_Time_ms;
    }   
    Audio_Debug_Start();
    osDelay(1);
  }
  /* USER CODE END StartAudio_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
