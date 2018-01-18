/*
******************************************************************************
*@文件名字  : BspUSB.h
*@描述	    : USB相关底层驱动，裸机和RTOS都支持
******************************************************************************
*@作者		: 丁志铖
*@强制性规定: 移植时只可以在 USER CODE BEGIN  USER CODE END  之间删改代码!!!
*@版本		: V1.00      2016/3/28
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


/*************************以上代码移植过程中需要修改********************************/
/*------------------------分割线---------------------------------------------------*/
/*************************以下代码移植过程中不需要修改******************************/

typedef enum
{
    BSP_USB_ERR_SIZE_ZERO               = 0,
    BSP_USB_ERR_NO_CONFIGURED_OR_BUSY   ,
    BSP_USB_OK,
}BSP_USB_ERR_STAT_ENUM;

/*************************对外接口函数********************************************/
extern void BspUSBInit(void);
extern BSP_USB_ERR_STAT_ENUM BspUSBSendData(u8* Buf, u16 Size);
extern void BspUSBReceiveCallBack(PCD_HandleTypeDef *PcdPtr);



#ifdef __cplusplus
}
#endif


#endif

/*********END OF FILE****/


