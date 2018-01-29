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
  osThreadDef(NameProcessMax, ProcessMaxTask, osPriorityRealtime, 0, 128);
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
  /* Infinite loop */
  for(;;)
  {
    
//      Test Read Fifo code£º         
//            while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)==1)
//            {
//                osDelay(1);
//            } 
//            DrvBaroMaxReadData(BSP_I2C_COM_MAX, REG_FIFO_WR_PTR, &max_wr_ptr,1);
//            DrvBaroMaxReadData(BSP_I2C_COM_MAX, REG_FIFO_RD_PTR, &max_rd_ptr,1);
//            DrvMaxFifoReadBytes(temp);
//            max_red =  (uint32_t)((uint32_t)((uint32_t)temp[0]&0x03)<<16) | (uint32_t)temp[1]<<8 | (uint32_t)temp[2];    // Combine values to get the actual number
//            max_ir = (uint32_t)((uint32_t)((uint32_t)temp[3] & 0x03)<<16) |(uint32_t)temp[4]<<8 | (uint32_t)temp[5];   // Combine values to get the actual number
//            
//            SEGGER_RTT_printf(0,"%d        %d        wr=%d    rd=%d\n",max_red,max_ir,max_wr_ptr,max_rd_ptr);
//            osDelay(1);
    
    osDelay(500);
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
    osDelay(500);
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    osDelay(500);
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

 //   HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
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
