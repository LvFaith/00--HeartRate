/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "gpio.h"  
#include "DrvMax.h"
#include "DrvLcd.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId NameLedAndTestHandle;
osThreadId NameProcessMaxHandle;
osThreadId NameProcessKeyHandle;

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void LedAndTestTask(void const * argument);
void ProcessMaxTask(void const * argument);
void ProcessKeyTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */
u8 temp_num = 0;
u8 max_fifo_data[6] = {0};
uint32_t max_red;
uint32_t max_ir;
u8 max_wr_ptr;
u8 max_rd_ptr;
extern u8 dis_hr,dis_spo2,dis_data_ready;
u8 last_dis_hr = 0, last_dis_spo2 = 0;
/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

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

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of NameLedAndTest */
  osThreadDef(NameLedAndTest, LedAndTestTask, osPriorityLow, 0, 64);
  NameLedAndTestHandle = osThreadCreate(osThread(NameLedAndTest), NULL);

  /* definition and creation of NameProcessMax */
  osThreadDef(NameProcessMax, ProcessMaxTask, osPriorityRealtime, 0, 256);
  NameProcessMaxHandle = osThreadCreate(osThread(NameProcessMax), NULL);

  /* definition and creation of NameProcessKey */
  osThreadDef(NameProcessKey, ProcessKeyTask, osPriorityBelowNormal, 0, 128);
  NameProcessKeyHandle = osThreadCreate(osThread(NameProcessKey), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN StartDefaultTask */
//  u8 temp[6];
  u8 num = 0;
  u16 hr_x = 40;
  u16 dis_spo2_x = 40;
  /* Infinite loop */
  for(;;)
  {
    if(dis_hr>50 && dis_spo2!=0 && dis_data_ready == 1)
    {
      dis_data_ready = 0;
      print_num(190,13,dis_hr);//显示心率
      print_num(318,13,dis_spo2); //显示血氧浓度
      
      if(dis_hr>50)
      Line(hr_x,178-last_dis_hr,hr_x+3,179-dis_hr,BLUE);   //显示心率曲线  
      hr_x = hr_x+2;
      
      if(hr_x>479)   //循环显示
        hr_x = 40;     
      if(dis_hr>50)
      last_dis_hr = dis_hr;
      
      
      if(dis_spo2>0)
      Line(dis_spo2_x,319-last_dis_spo2,dis_spo2_x+3,319-dis_spo2,BLUE);     //显示血氧曲线
      dis_spo2_x = dis_spo2_x+2;
      
      if(dis_spo2_x>479)
        dis_spo2_x = 40;
      
      if(dis_spo2>30)
      last_dis_spo2 = dis_spo2;
    }
    osDelay(5);
  }
  /* USER CODE END StartDefaultTask */
}

/* LedAndTestTask function */
__weak void LedAndTestTask(void const * argument)
{
  /* USER CODE BEGIN LedAndTestTask */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
    osDelay(500);
  }
  /* USER CODE END LedAndTestTask */
}

/* ProcessMaxTask function */
__weak void ProcessMaxTask(void const * argument)
{
  /* USER CODE BEGIN ProcessMaxTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END ProcessMaxTask */
}

/* ProcessKeyTask function */
void ProcessKeyTask(void const * argument)
{
  /* USER CODE BEGIN ProcessKeyTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END ProcessKeyTask */
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
