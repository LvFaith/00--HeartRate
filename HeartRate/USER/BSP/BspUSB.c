/*
******************************************************************************
*@文件名字  : BspUSB.c
*@描述	    : USB相关底层驱动，裸机和RTOS都支持
******************************************************************************
*@作者		: 丁志铖
*@强制性规定: 移植时只可以在 USER CODE BEGIN  USER CODE END  之间删改代码!!!
*@版本		: V1.00      2016/4/14
******************************************************************************
*/


#include "BspUSB.h"
#include "CMSIS_OS.h"

/* USER CODE BEGIN 0*/ 

#define BSP_TIME_OUT    (3)

/* USER CODE END   0*/

/*************************以上代码移植过程中需要修改********************************/
/*------------------------分割线---------------------------------------------------*/
/*************************以下代码移植过程中不需要修改******************************/


/******************************************************************************
* @函 数 名： BspUSBReceiveProcess
* @函数描述： USB接收到数据的处理函数指针，在Drv层注册这个处理函数 
******************************************************************************/
void (*g_BspUSBReceiveProcess)(u8 const * Buf, u16 Size);



/******************************************************************************
* @函 数 名： BspUSBInit
* @函数描述： 初始化USB相关数据
* @参    数： 
* @返 回 值： 
* @备    注： 
******************************************************************************/
void BspUSBInit(void)
{
    g_BspUSBReceiveProcess = NULL;
}

BSP_USB_ERR_STAT_ENUM BspUSBSendData(u8* Buf, u16 Size)
{
  static u8 SendBuf[64] = {0};
  u16 PackNum = 0;
  u8 ErrTimes = 0;
  //volatile USBD_HandleTypeDef* volatile FsPtr = &hUsbDeviceFS;
  volatile USBD_CUSTOM_HID_HandleTypeDef* volatile hhid = (USBD_CUSTOM_HID_HandleTypeDef*)(&hUsbDeviceFS)->pClassData;

  if(0 == Size)
  {
    //BspErr();
    return BSP_USB_ERR_SIZE_ZERO;
  }

  PackNum = ( ( Size%64 ) ? ( Size/64 + 1 ) : ( Size/64 ) );
  for( u16 i = 0; i < PackNum - 1; i++ )//前面PackNum-1 包数据
  {
    memcpy(SendBuf, Buf+64*i, 64);

    while(( CUSTOM_HID_IDLE != hhid->state ) || ( USBD_STATE_CONFIGURED != hUsbDeviceFS.dev_state))
    {
      if( (++ErrTimes) > BSP_TIME_OUT)
      {
        //BspErr();
        //SEGGER_RTT_printf(0,"stat:%d  dev stat:%d",hhid->state, hUsbDeviceFS.dev_state);
        return BSP_USB_ERR_NO_CONFIGURED_OR_BUSY;
      }
      else
        osDelay(1);
    }
    ErrTimes = 0;

    hhid->state = CUSTOM_HID_BUSY;
    USBD_LL_Transmit(&hUsbDeviceFS, CUSTOM_HID_EPIN_ADDR, SendBuf, 64);
      
  }

  memset(SendBuf,0,64);
  memcpy(SendBuf, Buf + 64*(PackNum - 1) ,Size - 64*(PackNum - 1));//最后一包数据

  while(( CUSTOM_HID_IDLE != hhid->state ) || ( USBD_STATE_CONFIGURED != hUsbDeviceFS.dev_state))
  {
    if( (++ErrTimes) > BSP_TIME_OUT)
    {
      //BspErr();
      //SEGGER_RTT_printf(0,"stat:%d  dev stat:%d",hhid->state, hUsbDeviceFS.dev_state);
      return BSP_USB_ERR_NO_CONFIGURED_OR_BUSY;
    }
    else
      osDelay(1);
  }
  
  ErrTimes = 0;
  hhid->state = CUSTOM_HID_BUSY;

  USBD_LL_Transmit(&hUsbDeviceFS, CUSTOM_HID_EPIN_ADDR, SendBuf, 64);

  return BSP_USB_OK;
  
}

/******************************************************************************
* @函 数 名： BspUSBReceiveCallBack
* @函数描述： 在usb_custom_hid_if.c中的CUSTOM_HID_OutEvent_FS函数中调用该函数
* @参    数： 
* @返 回 值： 
* @备    注： 
******************************************************************************/

void BspUSBReceiveCallBack(PCD_HandleTypeDef *PcdPtr)
{
  USBD_HandleTypeDef * pdev = (USBD_HandleTypeDef *)(PcdPtr->pData);
  USBD_CUSTOM_HID_HandleTypeDef     *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)pdev->pClassData;
  if( USBD_STATE_CONFIGURED == pdev->dev_state )
  {
    if(g_BspUSBReceiveProcess != NULL)
    {
      (*g_BspUSBReceiveProcess)(hhid->Report_buf,64);
    }
  }
}



/*********END OF FILE****/


