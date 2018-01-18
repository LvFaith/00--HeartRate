/*
******************************************************************************
*@文件名字  : BspSpi.h
*@描述	    : Spi BSP 层代码，裸机和RTOS都支持
*       
******************************************************************************
*@作者		: 丁志铖
*@强制性规定: 移植时只可以在 USER CODE BEGIN  USER CODE END  之间删改代码!!!
*@版本		: V1.00 2016/3/28 初始版本，只支持DMA(STM32F405测试)
******************************************************************************
*/

#ifndef _BSP_CAN_H
#define _BSP_CAN_H


#ifdef __cplusplus
 extern "C" {
#endif 


#include "config.h"

#include "can.h"


typedef struct
{
    void (*Can1ReadData)(u8* buf, u32 size);  //中断接收处理函数指针
    void (*Can2ReadData)(u8* buf, u32 size);  //中断接收处理函数指针
    void (*Can3ReadData)(u8* buf, u32 size);  //中断接收处理函数指针
    osSemaphoreId CountSemCan1MailBoxNum;     //Can1邮箱空个数
    osSemaphoreId CountSemCan2MailBoxNum;     //Can2邮箱空个数
    osSemaphoreId CountSemCan3MailBoxNum;     //Can3邮箱空个数
}BspCanProcessStruct;


extern BspCanProcessStruct g_CanProcess;

extern void BspCanInit(void);
extern u8 BspCanTxData( CAN_HandleTypeDef *hcan, u32 ide, u32 canId, u8* buf, u32 size );
extern HAL_StatusTypeDef BspCanWriteData(CAN_HandleTypeDef *hcan, u32 ide, u32 canId, u8 *buf, u8 len);
extern void BspCanReceiveIrqHandler(CAN_HandleTypeDef* hcan, uint8_t FIFONumber);
extern void BspCanTransmitIrqHandler(CAN_HandleTypeDef* hcan);
   
#ifdef __cplusplus
  }
#endif
  
  
#endif

/*********END OF FILE****/


