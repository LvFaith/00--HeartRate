/*
******************************************************************************
*@�ļ�����  : BspSpi.h
*@����	    : Spi BSP ����룬�����RTOS��֧��
*       
******************************************************************************
*@����		: ��־��
*@ǿ���Թ涨: ��ֲʱֻ������ USER CODE BEGIN  USER CODE END  ֮��ɾ�Ĵ���!!!
*@�汾		: V1.00 2016/3/28 ��ʼ�汾��ֻ֧��DMA(STM32F405����)
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
    void (*Can1ReadData)(u8* buf, u32 size);  //�жϽ��մ�����ָ��
    void (*Can2ReadData)(u8* buf, u32 size);  //�жϽ��մ�����ָ��
    void (*Can3ReadData)(u8* buf, u32 size);  //�жϽ��մ�����ָ��
    osSemaphoreId CountSemCan1MailBoxNum;     //Can1����ո���
    osSemaphoreId CountSemCan2MailBoxNum;     //Can2����ո���
    osSemaphoreId CountSemCan3MailBoxNum;     //Can3����ո���
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


