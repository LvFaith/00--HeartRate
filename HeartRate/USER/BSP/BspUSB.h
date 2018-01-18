/*
******************************************************************************
*@�ļ�����  : BspUSB.h
*@����	    : USB��صײ������������RTOS��֧��
******************************************************************************
*@����		: ��־��
*@ǿ���Թ涨: ��ֲʱֻ������ USER CODE BEGIN  USER CODE END  ֮��ɾ�Ĵ���!!!
*@�汾		: V1.00      2016/3/28
******************************************************************************
*/

#ifndef _BSP_USB_H_
#define _BSP_USB_H_


#ifdef __cplusplus
 extern "C" {
#endif 


#include "config.h"

#include "usb_device.h"
#include "usbd_customhid.h"


/* USER CODE BEGIN 0*/

extern void (*g_BspUSBReceiveProcess)(u8 const * Buf, u16 Size);
     
/* USER CODE END   0*/


/*************************���ϴ�����ֲ��������Ҫ�޸�********************************/
/*------------------------�ָ���---------------------------------------------------*/
/*************************���´�����ֲ�����в���Ҫ�޸�******************************/

typedef enum
{
    BSP_USB_ERR_SIZE_ZERO               = 0,
    BSP_USB_ERR_NO_CONFIGURED_OR_BUSY   ,
    BSP_USB_OK,
}BSP_USB_ERR_STAT_ENUM;

/*************************����ӿں���********************************************/
extern void BspUSBInit(void);
extern BSP_USB_ERR_STAT_ENUM BspUSBSendData(u8* Buf, u16 Size);
extern void BspUSBReceiveCallBack(PCD_HandleTypeDef *PcdPtr);



#ifdef __cplusplus
}
#endif


#endif

/*********END OF FILE****/


