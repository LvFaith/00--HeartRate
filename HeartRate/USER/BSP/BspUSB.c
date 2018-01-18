/*
******************************************************************************
*@�ļ�����  : BspUSB.c
*@����	    : USB��صײ������������RTOS��֧��
******************************************************************************
*@����		: ��־��
*@ǿ���Թ涨: ��ֲʱֻ������ USER CODE BEGIN  USER CODE END  ֮��ɾ�Ĵ���!!!
*@�汾		: V1.00      2016/4/14
******************************************************************************
*/


#include "BspUSB.h"
#include "CMSIS_OS.h"

/* USER CODE BEGIN 0*/ 

#define BSP_TIME_OUT    (3)

/* USER CODE END   0*/

/*************************���ϴ�����ֲ��������Ҫ�޸�********************************/
/*------------------------�ָ���---------------------------------------------------*/
/*************************���´�����ֲ�����в���Ҫ�޸�******************************/


/******************************************************************************
* @�� �� ���� BspUSBReceiveProcess
* @���������� USB���յ����ݵĴ�����ָ�룬��Drv��ע����������� 
******************************************************************************/
void (*g_BspUSBReceiveProcess)(u8 const * Buf, u16 Size);



/******************************************************************************
* @�� �� ���� BspUSBInit
* @���������� ��ʼ��USB�������
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע�� 
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
  for( u16 i = 0; i < PackNum - 1; i++ )//ǰ��PackNum-1 ������
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
  memcpy(SendBuf, Buf + 64*(PackNum - 1) ,Size - 64*(PackNum - 1));//���һ������

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
* @�� �� ���� BspUSBReceiveCallBack
* @���������� ��usb_custom_hid_if.c�е�CUSTOM_HID_OutEvent_FS�����е��øú���
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע�� 
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


