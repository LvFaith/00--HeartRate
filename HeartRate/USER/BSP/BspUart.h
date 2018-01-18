/*
******************************************************************************
*@文件名字  : BspUart.h
*@描述	    : 提供串口相关代码，目前只支持RTOS
******************************************************************************
*@作者		: 丁志铖
*@强制性规定: 移植时只可以在 USER CODE BEGIN  USER CODE END  之间删改代码!!!
*@版本		: V1.00 2016/2/18
*             V1.01 2016/3/19 丁志铖:消息队列对应的发送BUF改成指针，由用户定义需要多大的BUF。
*             V1.02 2016/4/5  丁志铖:增加对外接口函数部分。
*             V1.03 2016/4/11 甘家华:BSP_UART_BUF_STRUCT结构体的Size变量由u8型改为u16;
*                                   在BSP_UART_TX_STRUCT中引入QueueSize变量，由用户定义需要多长的队列;
*                                   针对不同的串口,定义不同的队列长度宏,该宏赋予BSP_UART_TX_STRUCT中引入QueueSize变量;
*									在BspUartTxPutDataToQueue函数中引入if(Size>TxStruct->BufSize)判定。
*             V1.04 2016/12/1 按新结构从新编写的底层，将流量控制部分交给common stream
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

/*	串口支持的发送接收模式: 死等、DMA、中断	*/
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
	u8* buf;    //用户定义数组
}BspUartBufStruct;//

/*	uart tx struct	*/
typedef struct
{
	UART_HandleTypeDef *huart;						//对应的huart的指针
	osSemaphoreId binarySemTxFinish;			//二值信号量，初始化为0，起始资源不可用，指示一帧数据发送完成
	BspUartCommunicationModeEnum mode;
}BspUartTxStruct;


/*	uart rx queue struct	*/
typedef struct
{
	UART_HandleTypeDef *huart;    //对应的huart的指针
	BspUartBufStruct uartRxBuf;
	u16 bufSize;                  //数组大小，即最大一次接收的数据量
	u8 idleFlag;                  //stm32F4产生空闲中断后会产生一个满中断。。。该标志位让满中断知道已经处理了，不需要理会
	BspUartCommunicationModeEnum mode;
}BspUartRxStruct;




typedef void (*BspUartProRx)(u8 const * const , u16 const );

//接收协议处理函数，在Drv层实现具体的协议处理函数，初始化的时候注册下函数就可以
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



/*************************对外接数据********************************************/

extern BspUartRxFunctionStruct g_BspUartProRxFunc;//接收协议处理函数组合

extern BspUartTxGroupStruct g_BspUartTxGroup;



/*************************对外接口函数********************************************/
extern void BspUartInit(void);
extern void BspUartRxIdleStateCheck(UART_HandleTypeDef *huart);
extern u8 BspUartTransmitData(BspUartTxStruct *TxStruct, u8 * const buf, u16 const size);
extern u8 BspUartTransmitDataBlock(BspUartTxStruct *TxStruct, u8 * const buf, u16 const size);


#ifdef __cplusplus
}
#endif


#endif

/*********END OF FILE****/

