/*
******************************************************************************
*@文件名字  : BspI2CSoftDriver.h
*@描述	    : I2C主从机模拟，裸机和RTOS都支持
******************************************************************************
*@作者		: 丁志铖
*@强制性规定: 移植时只可以在 USER CODE BEGIN  USER CODE END  之间删改代码!!!
*@版本		: V1.00   首次编写I2C软件模拟主从机   2016/2/1
*             V1.01   2016/3/2  更改了说明
*             V1.02   2016/3/26 丁志铖:将BSP_I2C_DELAY_US移到可删改代码部分，
*                                      用户可以定义它的大小来控制主I2C通讯速率
*             V1.03   2016/3/28 丁志铖:将定时器回调函数放到BspTim.c中
*             V1.04   2016/4/1  丁志铖:更改I2C主模式一次读写字节数的数据类型为u16，以支持更长的数据包传送
*             V1.05   2016/4/9  丁志铖:BspI2CMasterDelayUS函数名改成BspI2CMasterDelay，把I2C从机绝大部分函数改成宏实现，提高通信速率，
*                                      目前stm32f1 64M主频 可以支持到110K以上，之前的函数实现代码只能到30K
*             V1.06   2016/4/28  邸兴超:改进宏的实现。去掉了不必要的空语句。
*             V1.07   2016/12/1  采用新架构，丢掉BspTypeDef.h
******************************************************************************
*/

#ifndef _BSP_I2C_DRIVER_H_
#define _BSP_I2C_DRIVER_H_


#ifdef __cplusplus
 extern "C" {
#endif 


#include "config.h"


/* USER CODE BEGIN Includes */

/* USER CODE END Includes */


/*************************选择支持的I2C类型，至少选择一项***************************/
/* USER CODE BEGIN 0*/
#define BSP_SUPPORT_I2C_SOFT_MASTER		//支持I2C主模式软件模拟
//#define BSP_SUPPORT_I2C_SOFT_SLAVER		//支持I2C从模式软件模拟
/* USER CODE END   0*/

/*************************如果支持从机模式，需要定义从机地址和状态机枚举************/
#ifdef BSP_SUPPORT_I2C_SOFT_SLAVER

    
	/* USER CODE BEGIN 1*/

	extern void (*BspI2CSlaverComMasterProRxData)(u8* RxBuf, u16 RxBufSize, u8* TxBuf);
	
	extern TIM_HandleTypeDef htim2;
	#define	BSP_I2C_SLAVER_ADDR				0x01
	#define BSP_I2C_SLAVER_BLOCK_SIZE  		270  		//连续读写块的大小，块地址从0x00开始，如果超过块大小则会循环覆盖
	#define BSP_I2C_SLAVER_WDT          	htim2		//监视I2C总线，超过一定时间没有发生SCL反转则初始化I2CSlaver
	#define BSP_I2C_SLAVER_WDT_INSTANCE		TIM2
	#define BSP_I2C_SLAVER_WDT_SOURCE_HZ	(64000000)	//WDT时钟源频率HZ
	#define BSP_I2C_SLAVER_IDLE_TIME_MS		(50)			//多少毫秒SCL没变化就认为进入空闲模式
	/* USER CODE END   1*/
	
	/*********************从机状态机状态枚举****************************************/
	typedef enum
	{
		BSP_I2C_SLAVER_MODE_IDLE		= 	0,	//空闲
		BSP_I2C_SLAVER_MODE_ADDR			,	//检测到了起始条件，进入读取从机地址的阶段
		//BSP_I2C_SLAVER_MODE_REG			,	//这一条不适合，如果采用就限制I2C模拟了一个类EEPROM从机设备
		BSP_I2C_SLAVER_MODE_WRITE			,	//master write and slaver read
		BSP_I2C_SLAVER_MODE_READ			,	//master read and slaver write
		BSP_I2C_SLAVER_MODE_BUSY			,
	}BSP_I2C_SLAVER_MODE;

	/*************************端口枚举，移植时根据具体项目定义**************************/
	typedef enum 
	{
		/* USER CODE BEGIN 2*/
		BSP_I2C_SLAVER_COM_MASTER 	,		//和飞控主机通讯
		/* USER CODE END   2*/
	
		BSP_I2C_SLAVER_PORT_NUM		,
			
	}BSP_I2C_SLAVER_PORT_ENUM;
    
    
#endif



#ifdef BSP_SUPPORT_I2C_SOFT_MASTER

    /* USER CODE BEGIN 3*/
    
    #define BSP_I2C_DELAY_US    (60)//331KHz    //I2C延时时间，具体能实现多少速度，需要根据芯片类型和主频来看

    /* USER CODE END   3*/
    
	/*************************端口枚举，移植时根据具体项目定义**************************/
	typedef enum 
	{
		/* USER CODE BEGIN 4*/
		BSP_I2C_COM_MAX ,	//和气压计1通讯
	  /* USER CODE END   4*/

		BSP_I2C_MASTER_PORT_NUM		,	
	}BSP_I2C_MASTER_PORT_ENUM;		   
#endif



/**************为了效率从机代码尽量用宏实现，如果用到从机，以下代码也有需要更改的地方，
***************只需要在规定处更改即可********************************/


#define MEM_ADD_SIZE_8BIT    	((u8)(0x01))
#define MEM_ADD_SIZE_16BIT		((u8)(0x10))

#define BSP_I2C_OK   (1)
#define BSP_I2C_ERR  (0)

#define BSP_I2C_DELAY_NUM   (5)

#define BSP_I2C_ADDR_WRITE_MASK  	(0xFE)
#define BSP_I2C_ADDR_READ_MASK		(0x01)

#define BSP_I2C_ACK		(1)
#define BSP_I2C_NO_ACK	(0)


/*************************I2接口结构体**********************************************/
typedef struct
{
	BSP_PIN_STRUCT SCL;
	BSP_PIN_STRUCT SDA;
}BSP_I2C_PORT_STRUCT;


#ifdef BSP_SUPPORT_I2C_SOFT_MASTER

	typedef struct
	{
		BSP_I2C_PORT_STRUCT Port;
		u32 delay;
	}BSP_I2C_MASTER_PORT_STRUCT;

#endif


#ifdef BSP_SUPPORT_I2C_SOFT_SLAVER
	
	typedef struct
	{
		BSP_I2C_PORT_STRUCT Port;
		BSP_I2C_SLAVER_MODE Mode;
		u8 DataTemp;				//数据发送接收中转
		u8 DataBitPtr;				//指示数据传输位数
		u8 DataTxBuf[BSP_I2C_SLAVER_BLOCK_SIZE];
		u16 DataTxIndex;
		
		u8 DataRxBuf[BSP_I2C_SLAVER_BLOCK_SIZE];
		u16 DataRxIndex;
	}BSP_I2C_SLAVER_PORT_STRUCT;

#endif

/*************************对外接口变量********************************************/
#ifdef BSP_SUPPORT_I2C_SOFT_SLAVER  
    extern BSP_I2C_SLAVER_PORT_STRUCT g_I2CSlaver[BSP_I2C_SLAVER_PORT_NUM];
#endif
    
/*************************对外接口函数********************************************/
extern void BspI2CSoftInit(void);	//I2C端口初始化，需要根据实际项目更改函数内部实现

#ifdef BSP_SUPPORT_I2C_SOFT_MASTER
	extern u8 BspI2CMasterMemWrite(BSP_I2C_MASTER_PORT_ENUM Port, u8 DevAddress, u16 MemAddress, u8 MemAddSize, u8 *Ptr, u16 Num);
	extern u8 BspI2CMasterMemRead(BSP_I2C_MASTER_PORT_ENUM Port, u8 DevAddress, u16 MemAddress, u8 MemAddSize, u8 *Ptr, u16 Num);
	extern u8 BspI2CMasterTransmit(BSP_I2C_MASTER_PORT_ENUM Port, u8 SlaverAdd, u8 *Ptr, u16 Num);
	extern u8 BspI2CMasterReceive(BSP_I2C_MASTER_PORT_ENUM Port, u8 DevAddress, u8 *Ptr, u16 Num);
  extern void BspI2CMasterInit(void);
#endif

#ifdef BSP_SUPPORT_I2C_SOFT_SLAVER
    //只有BspI2CPinInt宏会被相应的管脚中断调用，其他宏都是为这个宏服务的

    /******************************************************************************
    * @名称： BspI2CSlaverSCLHigh
    * @描述： SCL置高 
    ******************************************************************************/
    #define BspI2CSlaverSCLHigh(X)  \
    do{\
        g_I2CSlaver[X].Port.SCL.GPIOX->BSRR = g_I2CSlaver[X].Port.SCL.PIN;\
      }while(0)

    /******************************************************************************
    * @名称： BspI2CSlaverSCLLow
    * @描述： SCL置低 
    ******************************************************************************/
    #define BspI2CSlaverSCLLow(X)   \
    do{\
        g_I2CSlaver[X].Port.SCL.GPIOX->BSRR = (u32)((u32)g_I2CSlaver[X].Port.SCL.PIN << 16);\
       }while(0)

    /******************************************************************************
    * @名称： BspI2CSlaverSDAHigh
    * @描述： SDA置高 
    ******************************************************************************/
    #define BspI2CSlaverSDAHigh(X)  \
    do{\
        g_I2CSlaver[X].Port.SDA.GPIOX->BSRR = g_I2CSlaver[X].Port.SDA.PIN;\
      }while(0)
    /******************************************************************************
    * @名称： BspI2CSlaverSDALow
    * @描述： SDA置低 
    ******************************************************************************/
    #define BspI2CSlaverSDALow(X)   \
        do{\
            g_I2CSlaver[X].Port.SDA.GPIOX->BSRR = (u32)((u32)g_I2CSlaver[X].Port.SDA.PIN << 16);\
          }while(0)

    /******************************************************************************
    * @名称： BspI2CSlaverSDARead
    * @描述： 读取SDA状态 
    ******************************************************************************/
    #define BspI2CSlaverSDARead(X) ((((g_I2CSlaver[X].Port.SDA.GPIOX)->IDR) & (g_I2CSlaver[X].Port.SDA.PIN)) != (u32)GPIO_PIN_RESET)

    /******************************************************************************
    * @名称： I2CSlaverSCLRead
    * @描述： 读取SCL电平 
    ******************************************************************************/
    #define BspI2CSlaverSCLRead(X) (( ((g_I2CSlaver[X].Port.SCL.GPIOX)->IDR) & (g_I2CSlaver[X].Port.SCL.PIN) ) != (u32)GPIO_PIN_RESET)
 
    /******************************************************************************
    * @名称： BspI2CSlaverReleasePort
    * @描述： 释放I2C的SCL和SDA接口 
    ******************************************************************************/
    #define BspI2CSlaverReleasePort( X ) \
    do{\
        BspI2CSlaverSCLHigh(X);\
        BspI2CSlaverSDAHigh(X);\
      }while(0)

	/******************************************************************************
    * @名称： BspI2CSlaverProRxData
    * @描述： 处理接收到的数据 master to slaver
    ******************************************************************************/
    #define BspI2CSlaverProRxData(X) \
    do{\
        /* USER CODE BEGIN SLAVER0*/\
        \
        switch(X)\
    	{\
    		case BSP_I2C_SLAVER_COM_MASTER:\
    			/*do someting:  处理接收到的数据，根据处理结果填充DataTxBuf,建议在AppDriver层实现接口*/\
    			if(BspI2CSlaverComMasterProRxData != NULL)\
                {\
                    BspI2CSlaverComMasterProRxData( g_I2CSlaver[X].DataRxBuf,\
                                                    g_I2CSlaver[X].DataRxIndex%BSP_I2C_SLAVER_BLOCK_SIZE,\
                                                    g_I2CSlaver[X].DataTxBuf);\
    			}\
    			break;\
    	    default:\
    			break;\
    	}\
    	\
    	/* USER CODE END   SLAVER0*/\
    }while(0)

    /******************************************************************************
    * @名称： BspI2CSlaverProTxData
    * @描述： 主机读取数据完毕后做的相应处理
    ******************************************************************************/
    #define BspI2CSlaverProTxData(X) \
    do{\
        /* USER CODE BEGIN SLAVER1*/\
        \
        switch(X)\
    	{\
    		case BSP_I2C_SLAVER_COM_MASTER:\
    			/*do someting: 例如可以刷新DataTxBuf为全0xFF*/\
    			g_I2CSlaver[X].DataTxBuf[0] = 0xFF;\
    			break;\
    			\
    		default:\
    			break;\
    	}\
    	\
    	/* USER CODE END   SLAVER1*/\
    }while(0)
    
    
    /******************************************************************************
    * @名称： BspI2CSlaverWDTStart
    * @描述： 从机I2C看门狗开启，主要是盯着SCL线，长时间不跳变则认为出错了
    ******************************************************************************/
    #define BspI2CSlaverWDTStart() \
    do{\
        __HAL_TIM_CLEAR_IT(&BSP_I2C_SLAVER_WDT, TIM_IT_UPDATE);\
        __HAL_TIM_ENABLE_IT(&BSP_I2C_SLAVER_WDT, TIM_IT_UPDATE);\
        __HAL_TIM_ENABLE(&BSP_I2C_SLAVER_WDT);\
    }while(0)
    
    /******************************************************************************
    * @名称： BspI2CSlaverReceiveBuf
    * @描述： 存储接收到的数据到临时buf里
    ******************************************************************************/
    #define BspI2CSlaverReceiveBuf( X, INDEX, VALUE ) \
    do{\
        g_I2CSlaver[X].DataRxBuf[(INDEX)%BSP_I2C_SLAVER_BLOCK_SIZE] = VALUE;\
    }while(0)

   
    /******************************************************************************
    * @名称： BspI2CSlaverTransmitBuf
    * @描述： 读取发送缓冲buf里的数据，并发送出去
    ******************************************************************************/
    //#define BspI2CSlaverTransmitBuf( X, INDEX ) g_I2CSlaver[X].DataTxBuf[INDEX%BSP_I2C_SLAVER_BLOCK_SIZE]

    
    /******************************************************************************
    * @名称： BspI2CSlaverSCLRising
    * @描述： 对SCL上升沿处理
    ******************************************************************************/
    #define BspI2CSlaverSCLRising( X ) \
    do{\
        switch( g_I2CSlaver[X].Mode )\
        {\
            case BSP_I2C_SLAVER_MODE_ADDR:\
            case BSP_I2C_SLAVER_MODE_WRITE:\
                g_I2CSlaver[X].DataTemp <<= 1;\
                if( BspI2CSlaverSDARead(X) )\
                    g_I2CSlaver[X].DataTemp++;\
                g_I2CSlaver[X].DataBitPtr++;\
                break;\
            case BSP_I2C_SLAVER_MODE_READ:\
                if( g_I2CSlaver[X].DataBitPtr > 8)\
                {\
                    if(BspI2CSlaverSDARead(X))\
                    {\
                        BspI2CSlaverReleasePort(X);\
                    }\
                    else\
                    {\
                        g_I2CSlaver[X].DataTemp = g_I2CSlaver[X].DataTxBuf[g_I2CSlaver[X].DataTxIndex%BSP_I2C_SLAVER_BLOCK_SIZE];\
                        g_I2CSlaver[X].DataTxIndex++;\
                        g_I2CSlaver[X].DataBitPtr = 0;\
                    }\
                }\
                break;\
            default:break;\
        }\
    }while(0)
    
    
    /******************************************************************************
    * @名称： BspI2CSlaverSCLRising
    * @描述： 对SCL下降沿处理
    ******************************************************************************/
    #define BspI2CSlaverSCLFalling( X ) \
    do{\
        if( g_I2CSlaver[X].DataBitPtr > 8 )\
        {\
            g_I2CSlaver[X].DataBitPtr = 0;\
            BspI2CSlaverSDAHigh(X);\
            return;\
        }\
        /*BspI2CSlaverSCLLow(X);*/\
        switch( g_I2CSlaver[X].Mode )\
        {\
            case BSP_I2C_SLAVER_MODE_ADDR:\
                if( g_I2CSlaver[X].DataBitPtr == 8 )\
                {\
                    if( g_I2CSlaver[X].DataTemp >> 1 == BSP_I2C_SLAVER_ADDR )\
                    {\
                        BspI2CSlaverSDALow(X);\
                        if( g_I2CSlaver[X].DataTemp & BSP_I2C_ADDR_READ_MASK )\
                        {\
                            g_I2CSlaver[X].Mode = BSP_I2C_SLAVER_MODE_READ;\
                            g_I2CSlaver[X].DataBitPtr  = 0;\
                            g_I2CSlaver[X].DataTxIndex = 0;\
                            g_I2CSlaver[X].DataTemp = g_I2CSlaver[X].DataTxBuf[g_I2CSlaver[X].DataTxIndex%BSP_I2C_SLAVER_BLOCK_SIZE];\
                            g_I2CSlaver[X].DataTxIndex++;\
                        }\
                        else\
                        {\
                            g_I2CSlaver[X].Mode = BSP_I2C_SLAVER_MODE_WRITE;\
                        }\
                    }\
                    else\
                    {\
                        g_I2CSlaver[X].Mode = BSP_I2C_SLAVER_MODE_BUSY;\
                        BspI2CSlaverReleasePort(X);\
                    }\
                }\
                break;\
            case BSP_I2C_SLAVER_MODE_WRITE:\
                if( g_I2CSlaver[X].DataBitPtr == 8 )\
                {\
                    BspI2CSlaverReceiveBuf( X, g_I2CSlaver[X].DataRxIndex++, g_I2CSlaver[X].DataTemp );\
                    BspI2CSlaverSDALow(X);\
                }\
                break;\
            case BSP_I2C_SLAVER_MODE_READ:\
                if( g_I2CSlaver[X].DataBitPtr < 8 )\
                {\
                    if( g_I2CSlaver[X].DataTemp & 0x80 )\
                    {\
                        BspI2CSlaverSDAHigh(X);\
                    }\
                    else\
                    {\
                        BspI2CSlaverSDALow(X);\
                    }\
                    g_I2CSlaver[X].DataTemp <<= 1;\
                }\
                else if( g_I2CSlaver[X].DataBitPtr == 8 )\
                {\
                    BspI2CSlaverSDAHigh(X);\
                }\
                g_I2CSlaver[X].DataBitPtr++;\
                break;\
            default:\
                break;\
        }\
        BspI2CSlaverSCLHigh(X);\
    }while(0)
    
    
    /******************************************************************************
    * @名称： BspI2CSlaverSDARising
    * @描述： 对SDA上升沿处理
    ******************************************************************************/
    #define BspI2CSlaverSDARising( X ) \
    do{\
        if( BspI2CSlaverSCLRead(X) )\
        {\
            if( BSP_I2C_SLAVER_MODE_WRITE == g_I2CSlaver[X].Mode )\
            {\
                BspI2CSlaverProRxData(X);\
            }\
            else if( BSP_I2C_SLAVER_MODE_READ == g_I2CSlaver[X].Mode )\
            {\
                BspI2CSlaverProTxData(X);\
            }\
            g_I2CSlaver[X].Mode = BSP_I2C_SLAVER_MODE_IDLE;\
        }\
    }while(0)
    
   
    /******************************************************************************
    * @名称： BspI2CSlaverSDAFalling
    * @描述： 对SDA下降沿处理
    ******************************************************************************/
    #define BspI2CSlaverSDAFalling( X ) \
    do{\
        switch( g_I2CSlaver[X].Mode )\
        {\
            case BSP_I2C_SLAVER_MODE_IDLE:\
                if( BspI2CSlaverSCLRead(X) )\
                {\
                    g_I2CSlaver[X].Mode = BSP_I2C_SLAVER_MODE_ADDR;\
                    g_I2CSlaver[X].DataBitPtr = 0;\
                    g_I2CSlaver[X].DataTemp = 0;\
                    g_I2CSlaver[X].DataRxIndex =  g_I2CSlaver[X].DataTxIndex = 0;\
                }\
                break;\
            default:break;\
        }\
    }while(0)

  
    /******************************************************************************
    * @名称： BspI2CPinSclInt
    * @描述： SCL引脚发生中断的处理，在相应中断中调用，并在HAL_GPIO_EXTI_IRQHandler函数前记得调用return语句
    ******************************************************************************/
    #define BspI2CPinSclInt() \
    do{\
        /* USER CODE BEGIN SLAVER2*/\
        \
        if(__HAL_GPIO_EXTI_GET_IT(BSP_I2C_SLAVER_COM_MASTER_SCL_Pin) != RESET)\
        {\
            __HAL_GPIO_EXTI_CLEAR_IT(BSP_I2C_SLAVER_COM_MASTER_SCL_Pin);\
            BspI2CSlaverWDTStart();\
            \
            if( BspI2CSlaverSCLRead(BSP_I2C_SLAVER_COM_MASTER) )\
            {\
                BspI2CSlaverSCLRising(BSP_I2C_SLAVER_COM_MASTER);\
            }\
            else\
            {\
                BspI2CSlaverSCLFalling(BSP_I2C_SLAVER_COM_MASTER);\
            }\
        }\
        \
        /* USER CODE END   SLAVER2*/\
    }while(0)

    /******************************************************************************
    * @名称： BspI2CPinSdaInt
    * @描述： SDA引脚发生中断的处理，在相应中断中调用，并在HAL_GPIO_EXTI_IRQHandler函数前记得调用return语句
    ******************************************************************************/
    #define BspI2CPinSdaInt() \
    do{\
        /* USER CODE BEGIN SLAVER3*/\
        \
        if(__HAL_GPIO_EXTI_GET_IT(BSP_I2C_SLAVER_COM_MASTER_SDA_Pin) != RESET)\
        {\
            __HAL_GPIO_EXTI_CLEAR_IT(BSP_I2C_SLAVER_COM_MASTER_SDA_Pin);\
            if( BspI2CSlaverSDARead(BSP_I2C_SLAVER_COM_MASTER) )\
            {\
                BspI2CSlaverSDARising(BSP_I2C_SLAVER_COM_MASTER);\
            }\
            else\
            {\
                BspI2CSlaverSDAFalling(BSP_I2C_SLAVER_COM_MASTER);\
            }\
        }\
        \
        /* USER CODE END   SLAVER3*/\
    }while(0)
    
#endif


#ifdef __cplusplus
}
#endif


#endif

/*********END OF FILE****/

