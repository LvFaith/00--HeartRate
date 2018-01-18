/*
******************************************************************************
*@�ļ�����  : BspUart.c
*@����	    : �ṩ������ش��룬Ŀǰֻ֧��RTOS
******************************************************************************
*@����		: ��־��
*@ǿ���Թ涨: ��ֲʱֻ������ USER CODE BEGIN  USER CODE END  ֮��ɾ�Ĵ���!!!
*@�汾		: V1.00 2016/2/18
*             V1.01 2016/3/19 ��־��:��Ϣ���ж�Ӧ�ķ���BUF�ĳ�ָ�룬���û�������Ҫ����BUF��
*             V1.02 2016/4/5  ��־��:���Ӷ���ӿں������֡�
*             V1.03 2016/4/11 �ʼһ�:BSP_UART_BUF_STRUCT�ṹ���Size������u8�͸�Ϊu16;
*                                   ��BSP_UART_TX_STRUCT������QueueSize���������û�������Ҫ�೤�Ķ���;
*                                   ��Բ�ͬ�Ĵ���,���岻ͬ�Ķ��г��Ⱥ�,�ú긳��BSP_UART_TX_STRUCT������QueueSize����;
*									                  ��BspUartTxPutDataToQueue����������if(Size>TxStruct->BufSize)�ж���
*             V1.04 2016/5/25 ������: ����ڴ��㹻��Ҫ��֤���ڽ��յ�ÿ�����ݳ��ȶ�С�ڴ��ڽ���buffer��
*			    ����ڴ治����buffer����С�ڽ������ݰ����ȣ�����ʹ��DMA����ģʽ��DMA�����ж����ø����ȼ���
*			    �޸�: ���������ݰ����ȴ��ڴ��ڽ���buffer�ĳ���ʱ��DMA���ж�ģʽ��������쳣��bug��
*             V1.05 2016/12/1 ���½ṹ���±�д�ĵײ㣬���������Ʋ��ֽ���common stream
******************************************************************************
*/
#include "BspUart.h"


BspUartRxFunctionStruct     g_BspUartProRxFunc;//����Э�鴦�������

BspUartTxGroupStruct        g_BspUartTxGroup;

static BspUartRxGroupStruct s_UartRxGroup;


/*	����֧�ֵ�ģʽ: ֻ���͡�ֻ���ա����ͺͽ���	*/
typedef enum
{
	BSP_UART_SUPPORT_TX			    =	1,
	BSP_UART_SUPPORT_RX			    =	2,
	BSP_UART_SUPPORT_TX_AND_RX	=	3,
}BspUartSupportModeEnum;


/* USER CODE BEGIN PV */


//����ʵ���������ÿ�������շ���Ҫ���ֽ���


#define UART1_RX_BUF_SIZE           (2048)//ĿǰF4�������ó���255��ԭ����飬�ʹӻ�ͨѶ
#define UART2_RX_BUF_SIZE           (128)//SBUS2
//#define UART3_RX_BUF_SIZE           (128)//SBUS1
#define UART4_RX_BUF_SIZE           (192)//ĿǰF4�������ó���255��ԭ����飬�ʹӻ�ͨѶ
#define UART5_RX_BUF_SIZE           (192)//ĿǰF4�������ó���255��ԭ����飬�ʹӻ�ͨѶ
#define UART6_RX_BUF_SIZE           (192)//ĿǰF4�������ó���255��ԭ����飬�ʹӻ�ͨѶ




static u8 s_Uart1RxBuf[UART1_RX_BUF_SIZE];
static u8 s_Uart2RxBuf[UART2_RX_BUF_SIZE];
//static u8 s_Uart3RxBuf[UART3_RX_BUF_SIZE];
static u8 s_Uart4RxBuf[UART4_RX_BUF_SIZE];
static u8 s_Uart5RxBuf[UART5_RX_BUF_SIZE];
static u8 s_Uart6RxBuf[UART6_RX_BUF_SIZE];
//static u8 s_Uart7RxBuf[UART7_RX_BUF_SIZE];
//static u8 s_Uart8RxBuf[UART8_RX_BUF_SIZE];

/* USER CODE END PV */


static void UartCheckProtocol(UART_HandleTypeDef *huart,u8 const * const buf, u16 const size);
static void UartGenerateTxStruct(BspUartTxStruct **TxStruct, UART_HandleTypeDef *huart);
static void UartGenerateRxStruct(BspUartRxStruct **RxStruct ,UART_HandleTypeDef *huart);
static void UartXInit(UART_HandleTypeDef *huart, BspUartSupportModeEnum Support);
static void UartRxInit(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef USER_HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

/******************************************************************************
* @�� �� ���� UartCheckProtocol
* @���������� �Խ��յ������ݸ���Э�����У��
* @��    ���� 
* @�� �� ֵ�� �� 
* @��    ע�� 
******************************************************************************/
static void UartCheckProtocol(UART_HandleTypeDef *huart,u8 const * const buf, u16 const size)
{
  /* USER CODE BEGIN 0*/

  //BspUartTxPutDataToQueue(Huart,Buf,Size);//�˾�ֻ��������
  if(USART1 == huart->Instance)
  {
    if(g_BspUartProRxFunc.uart1 != NULL)
      (*(g_BspUartProRxFunc.uart1))(buf,size); 
  }
  
  else if(USART2 == huart->Instance)
  {
    if(g_BspUartProRxFunc.uart2 != NULL)
      (*(g_BspUartProRxFunc.uart2))(buf,size); 
  }

#if 0
	else if(USART3 == huart->Instance)
  {
    if(g_BspUartProRxFunc.uart3 != NULL)
      (*(g_BspUartProRxFunc.uart3))(buf,size); 
  }
#endif

	else if(UART4 == huart->Instance)
  {
    if(g_BspUartProRxFunc.uart4 != NULL)
      (*(g_BspUartProRxFunc.uart4))(buf,size); 
  }

	else if(UART5 == huart->Instance)
  {
    if(g_BspUartProRxFunc.uart5 != NULL)
      (*(g_BspUartProRxFunc.uart5))(buf,size); 
  }

	else if(USART6 == huart->Instance)
  {
    if(g_BspUartProRxFunc.uart6 != NULL)
      (*(g_BspUartProRxFunc.uart6))(buf,size); 
  }
  /* USER CODE END   0*/
}

/******************************************************************************
* @�� �� ���� BspUartInit
* @���������� ��ʼ�����ڹ�������
* @��    ���� ��
* @�� �� ֵ�� �� 
* @��    ע�� 
******************************************************************************/
void BspUartInit(void)
{
  /* USER CODE BEGIN 1*/

  UartXInit(&huart1, BSP_UART_SUPPORT_TX_AND_RX);
  UartXInit(&huart2, BSP_UART_SUPPORT_RX);
  //UartXInit(&huart3, BSP_UART_SUPPORT_RX);
  UartXInit(&huart4, BSP_UART_SUPPORT_TX_AND_RX);
  UartXInit(&huart5, BSP_UART_SUPPORT_TX_AND_RX);
  UartXInit(&huart6, BSP_UART_SUPPORT_TX_AND_RX);
  
  /* USER CODE END   1*/
}


/******************************************************************************
* @�� �� ���� BspUartTxInit
* @���������� ��ʼ�����ڷ��������Դ
* @��    ���� 
* @�� �� ֵ�� �� 
* @��    ע�� 
******************************************************************************/
static void UartTxInit(UART_HandleTypeDef *huart)
{

	BspUartTxStruct *TxStruct = NULL;
	
	UartGenerateTxStruct(&TxStruct, huart);
	if(NULL == TxStruct)
		return;

	/* USER CODE BEGIN 2*/
	
	if(USART1 == huart->Instance)
	{
		TxStruct->huart = huart;

		osSemaphoreDef(BinarySemUart1);
	  TxStruct->binarySemTxFinish = osUserSemaphoreCreate(osSemaphore(BinarySemUart1), 1, 0);
	  
		TxStruct->mode = BSP_UART_MODE_DMA;
	}

  #if 0
	else if(USART2 == huart->Instance)
	{
		TxStruct->huart = huart;

		osSemaphoreDef(BinarySemUart2);
	  TxStruct->binarySemTxFinish = osUserSemaphoreCreate(osSemaphore(BinarySemUart2), 1, 0);
	  
		TxStruct->mode = BSP_UART_MODE_DMA;
	}
	
	else if(USART3 == huart->Instance)
	{
		TxStruct->huart = huart;

		osSemaphoreDef(BinarySemUart3);
	  TxStruct->binarySemTxFinish = osUserSemaphoreCreate(osSemaphore(BinarySemUart3), 1, 0);
	  
		TxStruct->mode = BSP_UART_MODE_DMA;
	}
  #endif
	
	else if(UART4 == huart->Instance)
	{
		TxStruct->huart = huart;

		osSemaphoreDef(BinarySemUart4);
	  TxStruct->binarySemTxFinish = osUserSemaphoreCreate(osSemaphore(BinarySemUart4), 1, 0);
	  
		TxStruct->mode = BSP_UART_MODE_DMA;
	}

	else if(UART5 == huart->Instance)
	{
		TxStruct->huart = huart;

		osSemaphoreDef(BinarySemUart5);
	  TxStruct->binarySemTxFinish = osUserSemaphoreCreate(osSemaphore(BinarySemUart5), 1, 0);
	  
		TxStruct->mode = BSP_UART_MODE_DMA;
	}

	else if(USART6 == huart->Instance)
	{
		TxStruct->huart = huart;

		osSemaphoreDef(BinarySemUart6);
	  TxStruct->binarySemTxFinish = osUserSemaphoreCreate(osSemaphore(BinarySemUart6), 1, 0);
	  
		TxStruct->mode = BSP_UART_MODE_DMA;
	}
	

	/* USER CODE END   2*/
	
}

/******************************************************************************
* @�� �� ���� BspUartRxInit
* @���������� ��ʼ�����ڽ��������Դ
* @��    ���� 
* @�� �� ֵ�� �� 
* @��    ע�� 
******************************************************************************/
static void UartRxInit(UART_HandleTypeDef *huart)
{
  BspUartRxStruct *RxStruct = NULL;

  UartGenerateRxStruct(&RxStruct, huart);
  if(NULL == RxStruct)
    return;

  /* USER CODE BEGIN 3*/

	
  if(USART1 == huart->Instance)
  {
  	RxStruct->huart         = huart;

    RxStruct->uartRxBuf.buf = s_Uart1RxBuf;
    RxStruct->bufSize       = UART1_RX_BUF_SIZE;

    RxStruct->mode          = BSP_UART_MODE_DMA;
  }

  else if(USART2 == huart->Instance)
  {
  	RxStruct->huart         = huart;

    RxStruct->uartRxBuf.buf = s_Uart2RxBuf;
    RxStruct->bufSize       = UART2_RX_BUF_SIZE;

    RxStruct->mode          = BSP_UART_MODE_DMA;
  }

#if 0
	else if(USART3 == huart->Instance)
  {
  	RxStruct->huart         = huart;

    RxStruct->uartRxBuf.buf = s_Uart3RxBuf;
    RxStruct->bufSize       = UART3_RX_BUF_SIZE;

    RxStruct->mode          = BSP_UART_MODE_DMA;
  }
#endif
	
	else if(UART4 == huart->Instance)
  {
  	RxStruct->huart         = huart;

    RxStruct->uartRxBuf.buf = s_Uart4RxBuf;
    RxStruct->bufSize       = UART4_RX_BUF_SIZE;

    RxStruct->mode          = BSP_UART_MODE_DMA;
  }

	else if(UART5 == huart->Instance)
  {
  	RxStruct->huart         = huart;

    RxStruct->uartRxBuf.buf = s_Uart5RxBuf;
    RxStruct->bufSize       = UART5_RX_BUF_SIZE;

    RxStruct->mode          = BSP_UART_MODE_DMA;
  }

	else if(USART6 == huart->Instance)
  {
  	RxStruct->huart         = huart;

    RxStruct->uartRxBuf.buf = s_Uart6RxBuf;
    RxStruct->bufSize       = UART6_RX_BUF_SIZE;

    RxStruct->mode          = BSP_UART_MODE_DMA;
  }

  /* USER CODE END   3*/

  

  /*  UART receive Initial */
  if( BSP_UART_MODE_DMA == RxStruct->mode )
  {
    //SEGGER_RTT_printf(0,"size %d ", RxStruct->bufSize);
  	USER_HAL_UART_Receive_DMA(huart, RxStruct->uartRxBuf.buf, RxStruct->bufSize);
  }
  else
  {
    HAL_UART_Receive_IT(huart, RxStruct->uartRxBuf.buf, RxStruct->bufSize);
  }
  
  __HAL_UART_CLEAR_IDLEFLAG(huart);
  //ʹ�ܿ����ж�
  __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
  
}


/******************************************************************************
* @�� �� ���� UartGenerateTxStruct
* @���������� ���ݸ�����huartӳ�����Ӧ�Ľṹ��TxStruct
* @��    ���� 
* @�� �� ֵ�� �� 
* @��    ע�� 
******************************************************************************/
static void UartGenerateTxStruct(BspUartTxStruct **TxStruct, UART_HandleTypeDef *huart)
{
	*TxStruct = NULL;

	/* USER CODE BEGIN 4*/

	if(USART1 == huart->Instance)
	{
		*TxStruct = &(g_BspUartTxGroup.uart1);
	}
	
  #if 0
  else if(USART2 == huart->Instance)
	{
		*TxStruct = &(g_BspUartTxGroup.uart2);
	}

	else if(USART3 == huart->Instance)
	{
		*TxStruct = &(g_BspUartTxGroup.uart3);
	}
  #endif
  
	else if(UART4 == huart->Instance)
	{
		*TxStruct = &(g_BspUartTxGroup.uart4);
	}
  
	else if(UART5 == huart->Instance)
	{
		*TxStruct = &(g_BspUartTxGroup.uart5);
	}

	else if(USART6 == huart->Instance)
	{
		*TxStruct = &(g_BspUartTxGroup.uart6);
	}
	
	/* USER CODE END   4*/
}


/******************************************************************************
* @�� �� ���� UartGenerateRxStruct
* @���������� ���ݸ�����huartӳ�����Ӧ�Ľṹ��RxStruct
* @��    ���� 
* @�� �� ֵ��  
* @��    ע�� 
******************************************************************************/
static void UartGenerateRxStruct(BspUartRxStruct **RxStruct ,UART_HandleTypeDef *huart)
{

  *RxStruct = NULL;

  /* USER CODE BEGIN 5*/

  if(USART1 == huart->Instance)
  {
    *RxStruct = &(s_UartRxGroup.uart1);
  }

  else if(USART2 == huart->Instance)
  {
    *RxStruct = &(s_UartRxGroup.uart2);
  }
  
	#if 0
	else if(USART3 == huart->Instance)
  {
    *RxStruct = &(s_UartRxGroup.uart3);
  }
  #endif
  
	else if(UART4 == huart->Instance)
  {
    *RxStruct = &(s_UartRxGroup.uart4);
  }
	
	else if(UART5 == huart->Instance)
  {
    *RxStruct = &(s_UartRxGroup.uart5);
  }

	else if(USART6 == huart->Instance)
  {
    *RxStruct = &(s_UartRxGroup.uart6);
  }
  
  /* USER CODE END   5*/
	
}



/*************************���ϴ�����ֲ��������Ҫ�޸�********************************/
/*------------------------�ָ���---------------------------------------------------*/
/*************************���´�����ֲ�����в���Ҫ�޸�******************************/

/**
  * @brief  DMA UART receive process complete callback. 
  * @param  hdma: DMA handle
  * @retval None
  */
static void USER_UART_DMAReceiveCplt(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  /* DMA Normal mode*/
  if((hdma->Instance->CR & DMA_SxCR_CIRC) == 0U)
  {
    huart->RxXferCount = 0U;
  
    /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
    CLEAR_BIT(huart->Instance->CR1, USART_CR1_PEIE);
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
    
    /* Disable the DMA transfer for the receiver request by setting the DMAR bit 
       in the UART CR3 register */
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);
	
    /* At end of Rx process, restore huart->RxState to Ready */
    huart->RxState = HAL_UART_STATE_READY;
  }
  HAL_UART_RxCpltCallback(huart);
}

static HAL_StatusTypeDef USER_HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{  
  uint32_t *tmp;
  
  /* Check that a Rx process is not already ongoing */
  if(huart->RxState == HAL_UART_STATE_READY) 
  {
    if((pData == NULL ) || (Size == 0U)) 
    {
      return HAL_ERROR;
    }
    
    /* Process Locked */
    __HAL_LOCK(huart);
    
    huart->pRxBuffPtr = pData;
    huart->RxXferSize = Size;
    
    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->RxState = HAL_UART_STATE_BUSY_RX;
        
    /* Set the UART DMA transfer complete callback */
    huart->hdmarx->XferCpltCallback = USER_UART_DMAReceiveCplt;
    
    /* Set the UART DMA Half transfer complete callback */
    huart->hdmarx->XferHalfCpltCallback = NULL;//UART_DMARxHalfCplt;
    
    /* Set the DMA error callback */
    huart->hdmarx->XferErrorCallback = NULL;//UART_DMAError;
    
    /* Set the DMA abort callback */
    huart->hdmarx->XferAbortCallback = NULL;

    /* Enable the DMA Stream */
    tmp = (uint32_t*)&pData;
    HAL_DMA_Start_IT(huart->hdmarx, (uint32_t)&huart->Instance->RDR, *(uint32_t*)tmp, Size);
    
    /* Enable the DMA transfer for the receiver request by setting the DMAR bit 
    in the UART CR3 register */
    SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);
    
    /* Process Unlocked */
    __HAL_UNLOCK(huart);
    
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY; 
  }
}


/******************************************************************************
* @�� �� ���� BspUartxInit
* @���������� ��ʼ�����������Դ
* @��    ���� 
* @�� �� ֵ�� �� 
* @��    ע�� 
******************************************************************************/
static void UartXInit(UART_HandleTypeDef *huart, BspUartSupportModeEnum Support)
{
  if( Support & BSP_UART_SUPPORT_TX )
  {
    UartTxInit(huart);
  }

  if( Support & BSP_UART_SUPPORT_RX )
  {
    UartRxInit(huart);
  }
}


/******************************************************************************
* @�� �� ���� BspUartTransmitData
* @���������� ���ڷ��ͺ���
* @��    ���� 
* @�� �� ֵ��  
* @��    ע�� �����ȵ�����DrvUartInit
******************************************************************************/
u8 BspUartTransmitData(BspUartTxStruct *TxStruct, u8 * const buf, u16 const size)
{
	if(0 == size)
	{
		return BSP_UART_ERR;
	}
		
  if( BSP_UART_MODE_DMA == TxStruct->mode )
  {
  	HAL_UART_Transmit_DMA(TxStruct->huart, buf, size);
  }
  else if( BSP_UART_MODE_IT == TxStruct->mode )
  {
  	HAL_UART_Transmit_IT(TxStruct->huart, buf, size);
  }
  else
  {
  	return BSP_UART_ERR;
  }

  return BSP_UART_OK;
}

/******************************************************************************
* @�� �� ���� BspUartTransmitDataBlock
* @���������� ���ڷ��ͺ���,����ģʽ
* @��    ���� 
* @�� �� ֵ��  
* @��    ע�� �����ȵ�����DrvUartInit
******************************************************************************/
u8 BspUartTransmitDataBlock(BspUartTxStruct *TxStruct, u8 * const buf, u16 const size)
{
	if(0 == size)
	{
		return BSP_UART_ERR;
	}
		
  if( BSP_UART_MODE_DMA == TxStruct->mode )
  {
  	HAL_UART_Transmit_DMA(TxStruct->huart, buf, size);
  }
  else if( BSP_UART_MODE_IT == TxStruct->mode )
  {
  	HAL_UART_Transmit_IT(TxStruct->huart, buf, size);
  }
  else
  {
  	return BSP_UART_ERR;
  }

  osSemaphoreWait(TxStruct->binarySemTxFinish,osWaitForever);

  return BSP_UART_OK;
}



/******************************************************************************
* @�� �� ���� DrvUartRxIdleStateCheck
* @���������� ���տ����жϼ�⴦��������stm32xxx_it.c�еĴ����жϺ��������
* @��    ���� 
* @�� �� ֵ�� �� 
* @��    ע�� 
******************************************************************************/
void BspUartRxIdleStateCheck(UART_HandleTypeDef *huart)
{
  u32 tmp_flag = 0, tmp_it_source = 0;
  BspUartRxStruct *RxStruct;

  tmp_flag      = __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE);

  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  { 
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    //huart->State = HAL_UART_STATE_READY;//cube 4.11
    huart->RxState= HAL_UART_STATE_READY;//cube 4.19
    UartGenerateRxStruct(&RxStruct, huart);
    if(RxStruct != NULL)
    {
      if ( BSP_UART_MODE_DMA == RxStruct->mode )
      {
        __HAL_DMA_DISABLE(huart->hdmarx);

        #if defined(STM32F0)||defined(STM32F1)
          RxStruct->UartRxBuf.size = RxStruct->bufSize - huart->hdmarx->Instance->CNDTR;
        #endif

        #if defined(STM32F4)||defined(STM32F7)
          RxStruct->uartRxBuf.size = RxStruct->bufSize - huart->hdmarx->Instance->NDTR;
				  RxStruct->idleFlag = 1;
					//SEGGER_RTT_printf(0,"s%d ",RxStruct->uartRxBuf.size);
        #endif
		  }
      else
      {
        RxStruct->uartRxBuf.size = huart->RxXferSize - huart->RxXferCount;
      }

      //���յ�һ������
      if( RxStruct->uartRxBuf.size )
      {
        UartCheckProtocol(huart, RxStruct->uartRxBuf.buf, RxStruct->uartRxBuf.size);
      }

      if ( BSP_UART_MODE_DMA == RxStruct->mode )
      {
        #if defined(STM32F0)||defined(STM32F1)
          huart->hdmarx->Instance->CNDTR = RxStruct->bufSize;
        #endif

        #if defined(STM32F4)||defined(STM32F7)
          huart->hdmarx->Instance->NDTR = RxStruct->bufSize;
        #endif

				#if defined(STM32F7)
				  //USER_HAL_UART_Receive_DMA( huart, RxStruct->uartRxBuf.buf, RxStruct->bufSize );
				#endif
        __HAL_DMA_ENABLE(huart->hdmarx);
      }
      else
      {
        HAL_UART_Receive_IT(huart, RxStruct->uartRxBuf.buf, RxStruct->bufSize);
      }

    }
  }
	
}



/******************************************************************************
* @�� �� ���� HAL_UART_RxCpltCallback
* @���������� ���ڽ�����ɻص�������д
* @��    ���� 
* @�� �� ֵ�� �� 
* @��    ע�� 
******************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback can be implemented in the user file
   */
  #if defined(STM32F4)
	  SEGGER_RTT_printf(0,"ful ");
	  BspUartRxStruct *RxStruct;
    UartGenerateRxStruct(&RxStruct, huart);
		if(RxStruct->idleFlag)
		{
		  RxStruct->idleFlag = 0;
			return;
		}
	  UartCheckProtocol(huart, RxStruct->uartRxBuf.buf, RxStruct->bufSize);
  #endif

	#if defined(STM32F7)
	  
	  BspUartRxStruct *RxStruct;
    UartGenerateRxStruct(&RxStruct, huart);
//    if( USART2 == huart->Instance )
//	    SEGGER_RTT_printf(0,"fuluart2 %d ",RxStruct->idleFlag);
//		else if( USART3 == huart->Instance )
//			SEGGER_RTT_printf(0,"fuluart3 %d ",RxStruct->idleFlag);
//		else if( UART4 == huart->Instance )
//			SEGGER_RTT_printf(0,"fuluart4 %d ",RxStruct->idleFlag);
//		else if( UART5 == huart->Instance )
//			SEGGER_RTT_printf(0,"fuluart5 %d ",RxStruct->idleFlag);
//		else if( USART6 == huart->Instance )
//			SEGGER_RTT_printf(0,"fuluart6 %d ",RxStruct->idleFlag);
		//__HAL_DMA_DISABLE(huart->hdmarx);
		//huart->hdmarx->Instance->NDTR = RxStruct->bufSize;
		if(RxStruct->idleFlag)
		{
		  RxStruct->idleFlag = 0;
			//__HAL_DMA_ENABLE(huart->hdmarx);
			return;
		}
	  UartCheckProtocol(huart, RxStruct->uartRxBuf.buf, RxStruct->bufSize);
		//__HAL_DMA_ENABLE(huart->hdmarx);
	#endif

  #if defined(STM32F0)||defined(STM32F1)
    BspUartRxStruct *RxStruct;
    UartGenerateRxStruct(&RxStruct, huart);
    
    if( NULL != RxStruct )
    {
      UartCheckProtocol( huart, RxStruct->uartRxBuf.buf, RxStruct->bufSize );
      if( BSP_UART_MODE_DMA ==  RxStruct->mode  )
      {
        huart->hdmarx->Instance->CNDTR = RxStruct->bufSize;
      }
      else
      {
        HAL_UART_Receive_IT(huart, RxStruct->uartRxBuf.buf, RxStruct->bufSize);
      }
    }
  #endif
}


/******************************************************************************
* @�� �� ���� HAL_UART_RxCpltCallback
* @���������� ���ڷ�����ɻص�������д
* @��    ���� 
* @�� �� ֵ�� �� 
* @��    ע�� 
******************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback can be implemented in the user file
   */
  BspUartTxStruct *TxStruct;
  
  UartGenerateTxStruct(&TxStruct, huart);
  
  if(TxStruct != NULL)
  {
    osSemaphoreRelease(TxStruct->binarySemTxFinish);
  }
}


/*********END OF FILE****/


