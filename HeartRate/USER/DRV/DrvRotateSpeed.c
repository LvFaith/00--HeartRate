/*
******************************************************************************
*@文件名字  : DrvRotateSpeed.c
*@描述	    : 转速驱动
*       
******************************************************************************
*@作者		: 张文君
*@强制性规定: 
*@版本		: 
*             

******************************************************************************
*/
#include "BspTim.h"
#include "DrvRotateSpeed.h"
uint32_t  RotateSpeed[2];
//uint32_t  RotateSpeed;

static void SetSpeedZero(void)
{
   RotateSpeed[0] = 0;
   //RotateSpeed[1] = 0;
}

void RotateSpeedInit(void)
{
  g_BspTimCallBackFunc.Tim4 = &SetSpeedZero;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//4 div
{
   if ( TIM4 == htim->Instance )
  {
    __HAL_TIM_SET_COUNTER(htim, 0);
  
    static u32 temp1;
//   static u32 temp2;
  
    temp1 = htim->Instance->CCR1;//HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
    //temp2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
    //SEGGER_RTT_printf(0,"%d ",temp1);
    
		RotateSpeed[0] = (6000000ul * 4 /temp1);
   // RotateSpeed[1] = (60000000ul/temp2);
  } 
}



