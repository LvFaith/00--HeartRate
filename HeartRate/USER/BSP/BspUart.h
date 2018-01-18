/*
******************************************************************************
*@�ļ�����  : BspUart.h
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
*									��BspUartTxPutDataToQueue����������if(Size>TxStruct->BufSize)�ж���
*             V1.04 2016/12/1 ���½ṹ���±�д�ĵײ㣬���������Ʋ��ֽ���common stream
******************************************************************************
*/
#ifndef _BSP_UART_H_
#define _BSP_UART_H_


#ifdef __cplusplus
 extern "C" {
#endif 


#include "Config.h"
//#include "usart.h"


/* USER CODE BEGIN Includes */

/* USER CODE END Includes */


/* USER CODE BEGIN 0*/

/* USER CODE END   0*/

typedef enum
{
  BSP_UART_OK = 0,
  BSP_UART_ERR,
}BspUartStatusEnum;

/*	����֧�ֵķ��ͽ���ģʽ: ���ȡ�DMA���ж�	*/
typedef enum
{
	BSP_UART_MODE_NONE = 0,
	BSP_UART_MODE_DMA,
	BSP_UART_MODE_IT,
}BspUartCommunicationModeEnum;

/*	uart buf struct	*/
typedef struct
{
	u16 size;
	u8* buf;    //�û���������
}BspUartBufStruct;//

/*	uart tx struct	*/
typedef struct
{
	UART_HandleTypeDef *huart;						//��Ӧ��huart��ָ��
	osSemaphoreId binarySemTxFinish;			//��ֵ�ź�������ʼ��Ϊ0����ʼ��Դ�����ã�ָʾһ֡���ݷ������
	BspUartCommunicationModeEnum mode;
}BspUartTxStruct;


/*	uart rx queue struct	*/
typedef struct
{
	UART_HandleTypeDef *huart;    //��Ӧ��huart��ָ��
	BspUartBufStruct uartRxBuf;
	u16 bufSize;                  //�����С�������һ�ν��յ�������
	u8 idleFlag;                  //stm32F4���������жϺ�����һ�����жϡ������ñ�־λ�����ж�֪���Ѿ������ˣ�����Ҫ���
	BspUartCommunicationModeEnum mode;
}BspUartRxStruct;




typedef void (*BspUartProRx)(u8 const * const , u16 const );

//����Э�鴦��������Drv��ʵ�־����Э�鴦��������ʼ����ʱ��ע���º����Ϳ���
typedef struct
{
  /* USER CODE BEGIN 1*/
    
//	BspUartProRx uart1;
//	BspUartProRx uart2;
	//BspUartProRx uart3;
//	BspUartProRx uart4;
//  BspUartProRx uart5;
//	BspUartProRx uart6;
	//BspUartProRx uart7;
	//BspUartProRx uart8;

	/* USER CODE BEGIN 1*/
}BspUartRxFunctionStruct;


/*	uart tx group struct	*/
typedef struct
{
  /* USER CODE BEGIN 2*/
  
//	BspUartTxStruct uart1;
	//BspUartTxStruct uart2;
	//BspUartTxStruct uart3;
//	BspUartTxStruct uart4;
//	BspUartTxStruct uart5;
//	BspUartTxStruct uart6;
	//BspUartTxStruct uart7;
	//BspUartTxStruct uart8;

	/* USER CODE BEGIN 2*/
}BspUartTxGroupStruct;

/*	uart rx group struct	*/
typedef struct
{
  /* USER CODE BEGIN 3*/
  
	BspUartRxStruct uart1;
	BspUartRxStruct uart2;
	BspUartRxStruct uart3;
	BspUartRxStruct uart4;
	BspUartRxStruct uart5;
	BspUartRxStruct uart6;
	//BspUartRxStruct uart7;
	//BspUartRxStruct uart8;

	/* USER CODE BEGIN 3*/
}BspUartRxGroupStruct;



/*************************���������********************************************/

extern BspUartRxFunctionStruct g_BspUartProRxFunc;//����Э�鴦�������

extern BspUartTxGroupStruct g_BspUartTxGroup;



/*************************����ӿں���********************************************/
extern void BspUartInit(void);
extern void BspUartRxIdleStateCheck(UART_HandleTypeDef *huart);
extern u8 BspUartTransmitData(BspUartTxStruct *TxStruct, u8 * const buf, u16 const size);
extern u8 BspUartTransmitDataBlock(BspUartTxStruct *TxStruct, u8 * const buf, u16 const size);


#ifdef __cplusplus
}
#endif


#endif

/*********END OF FILE****/

