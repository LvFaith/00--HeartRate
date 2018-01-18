/*
******************************************************************************
*@�ļ�����  : BspI2CSoftDriver.h
*@����	    : I2C���ӻ�ģ�⣬�����RTOS��֧��
******************************************************************************
*@����		: ��־��
*@ǿ���Թ涨: ��ֲʱֻ������ USER CODE BEGIN  USER CODE END  ֮��ɾ�Ĵ���!!!
*@�汾		: V1.00   �״α�дI2C���ģ�����ӻ�   2016/2/1
*             V1.01   2016/3/2  ������˵��
*             V1.02   2016/3/26 ��־��:��BSP_I2C_DELAY_US�Ƶ���ɾ�Ĵ��벿�֣�
*                                      �û����Զ������Ĵ�С��������I2CͨѶ����
*             V1.03   2016/3/28 ��־��:����ʱ���ص������ŵ�BspTim.c��
*             V1.04   2016/4/1  ��־��:����I2C��ģʽһ�ζ�д�ֽ�������������Ϊu16����֧�ָ��������ݰ�����
*             V1.05   2016/4/9  ��־��:BspI2CMasterDelayUS�������ĳ�BspI2CMasterDelay����I2C�ӻ����󲿷ֺ����ĳɺ�ʵ�֣����ͨ�����ʣ�
*                                      Ŀǰstm32f1 64M��Ƶ ����֧�ֵ�110K���ϣ�֮ǰ�ĺ���ʵ�ִ���ֻ�ܵ�30K
*             V1.06   2016/4/28  ۡ�˳�:�Ľ����ʵ�֡�ȥ���˲���Ҫ�Ŀ���䡣
*             V1.07   2016/12/1  �����¼ܹ�������BspTypeDef.h
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


/*************************ѡ��֧�ֵ�I2C���ͣ�����ѡ��һ��***************************/
/* USER CODE BEGIN 0*/
#define BSP_SUPPORT_I2C_SOFT_MASTER		//֧��I2C��ģʽ���ģ��
//#define BSP_SUPPORT_I2C_SOFT_SLAVER		//֧��I2C��ģʽ���ģ��
/* USER CODE END   0*/

/*************************���֧�ִӻ�ģʽ����Ҫ����ӻ���ַ��״̬��ö��************/
#ifdef BSP_SUPPORT_I2C_SOFT_SLAVER

    
	/* USER CODE BEGIN 1*/

	extern void (*BspI2CSlaverComMasterProRxData)(u8* RxBuf, u16 RxBufSize, u8* TxBuf);
	
	extern TIM_HandleTypeDef htim2;
	#define	BSP_I2C_SLAVER_ADDR				0x01
	#define BSP_I2C_SLAVER_BLOCK_SIZE  		270  		//������д��Ĵ�С�����ַ��0x00��ʼ������������С���ѭ������
	#define BSP_I2C_SLAVER_WDT          	htim2		//����I2C���ߣ�����һ��ʱ��û�з���SCL��ת���ʼ��I2CSlaver
	#define BSP_I2C_SLAVER_WDT_INSTANCE		TIM2
	#define BSP_I2C_SLAVER_WDT_SOURCE_HZ	(64000000)	//WDTʱ��ԴƵ��HZ
	#define BSP_I2C_SLAVER_IDLE_TIME_MS		(50)			//���ٺ���SCLû�仯����Ϊ�������ģʽ
	/* USER CODE END   1*/
	
	/*********************�ӻ�״̬��״̬ö��****************************************/
	typedef enum
	{
		BSP_I2C_SLAVER_MODE_IDLE		= 	0,	//����
		BSP_I2C_SLAVER_MODE_ADDR			,	//��⵽����ʼ�����������ȡ�ӻ���ַ�Ľ׶�
		//BSP_I2C_SLAVER_MODE_REG			,	//��һ�����ʺϣ�������þ�����I2Cģ����һ����EEPROM�ӻ��豸
		BSP_I2C_SLAVER_MODE_WRITE			,	//master write and slaver read
		BSP_I2C_SLAVER_MODE_READ			,	//master read and slaver write
		BSP_I2C_SLAVER_MODE_BUSY			,
	}BSP_I2C_SLAVER_MODE;

	/*************************�˿�ö�٣���ֲʱ���ݾ�����Ŀ����**************************/
	typedef enum 
	{
		/* USER CODE BEGIN 2*/
		BSP_I2C_SLAVER_COM_MASTER 	,		//�ͷɿ�����ͨѶ
		/* USER CODE END   2*/
	
		BSP_I2C_SLAVER_PORT_NUM		,
			
	}BSP_I2C_SLAVER_PORT_ENUM;
    
    
#endif



#ifdef BSP_SUPPORT_I2C_SOFT_MASTER

    /* USER CODE BEGIN 3*/
    
    #define BSP_I2C_DELAY_US    (60)//331KHz    //I2C��ʱʱ�䣬������ʵ�ֶ����ٶȣ���Ҫ����оƬ���ͺ���Ƶ����

    /* USER CODE END   3*/
    
	/*************************�˿�ö�٣���ֲʱ���ݾ�����Ŀ����**************************/
	typedef enum 
	{
		/* USER CODE BEGIN 4*/
		BSP_I2C_COM_MAX ,	//����ѹ��1ͨѶ
	  /* USER CODE END   4*/

		BSP_I2C_MASTER_PORT_NUM		,	
	}BSP_I2C_MASTER_PORT_ENUM;		   
#endif



/**************Ϊ��Ч�ʴӻ����뾡���ú�ʵ�֣�����õ��ӻ������´���Ҳ����Ҫ���ĵĵط���
***************ֻ��Ҫ�ڹ涨�����ļ���********************************/


#define MEM_ADD_SIZE_8BIT    	((u8)(0x01))
#define MEM_ADD_SIZE_16BIT		((u8)(0x10))

#define BSP_I2C_OK   (1)
#define BSP_I2C_ERR  (0)

#define BSP_I2C_DELAY_NUM   (5)

#define BSP_I2C_ADDR_WRITE_MASK  	(0xFE)
#define BSP_I2C_ADDR_READ_MASK		(0x01)

#define BSP_I2C_ACK		(1)
#define BSP_I2C_NO_ACK	(0)


/*************************I2�ӿڽṹ��**********************************************/
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
		u8 DataTemp;				//���ݷ��ͽ�����ת
		u8 DataBitPtr;				//ָʾ���ݴ���λ��
		u8 DataTxBuf[BSP_I2C_SLAVER_BLOCK_SIZE];
		u16 DataTxIndex;
		
		u8 DataRxBuf[BSP_I2C_SLAVER_BLOCK_SIZE];
		u16 DataRxIndex;
	}BSP_I2C_SLAVER_PORT_STRUCT;

#endif

/*************************����ӿڱ���********************************************/
#ifdef BSP_SUPPORT_I2C_SOFT_SLAVER  
    extern BSP_I2C_SLAVER_PORT_STRUCT g_I2CSlaver[BSP_I2C_SLAVER_PORT_NUM];
#endif
    
/*************************����ӿں���********************************************/
extern void BspI2CSoftInit(void);	//I2C�˿ڳ�ʼ������Ҫ����ʵ����Ŀ���ĺ����ڲ�ʵ��

#ifdef BSP_SUPPORT_I2C_SOFT_MASTER
	extern u8 BspI2CMasterMemWrite(BSP_I2C_MASTER_PORT_ENUM Port, u8 DevAddress, u16 MemAddress, u8 MemAddSize, u8 *Ptr, u16 Num);
	extern u8 BspI2CMasterMemRead(BSP_I2C_MASTER_PORT_ENUM Port, u8 DevAddress, u16 MemAddress, u8 MemAddSize, u8 *Ptr, u16 Num);
	extern u8 BspI2CMasterTransmit(BSP_I2C_MASTER_PORT_ENUM Port, u8 SlaverAdd, u8 *Ptr, u16 Num);
	extern u8 BspI2CMasterReceive(BSP_I2C_MASTER_PORT_ENUM Port, u8 DevAddress, u8 *Ptr, u16 Num);
  extern void BspI2CMasterInit(void);
#endif

#ifdef BSP_SUPPORT_I2C_SOFT_SLAVER
    //ֻ��BspI2CPinInt��ᱻ��Ӧ�Ĺܽ��жϵ��ã������궼��Ϊ���������

    /******************************************************************************
    * @���ƣ� BspI2CSlaverSCLHigh
    * @������ SCL�ø� 
    ******************************************************************************/
    #define BspI2CSlaverSCLHigh(X)  \
    do{\
        g_I2CSlaver[X].Port.SCL.GPIOX->BSRR = g_I2CSlaver[X].Port.SCL.PIN;\
      }while(0)

    /******************************************************************************
    * @���ƣ� BspI2CSlaverSCLLow
    * @������ SCL�õ� 
    ******************************************************************************/
    #define BspI2CSlaverSCLLow(X)   \
    do{\
        g_I2CSlaver[X].Port.SCL.GPIOX->BSRR = (u32)((u32)g_I2CSlaver[X].Port.SCL.PIN << 16);\
       }while(0)

    /******************************************************************************
    * @���ƣ� BspI2CSlaverSDAHigh
    * @������ SDA�ø� 
    ******************************************************************************/
    #define BspI2CSlaverSDAHigh(X)  \
    do{\
        g_I2CSlaver[X].Port.SDA.GPIOX->BSRR = g_I2CSlaver[X].Port.SDA.PIN;\
      }while(0)
    /******************************************************************************
    * @���ƣ� BspI2CSlaverSDALow
    * @������ SDA�õ� 
    ******************************************************************************/
    #define BspI2CSlaverSDALow(X)   \
        do{\
            g_I2CSlaver[X].Port.SDA.GPIOX->BSRR = (u32)((u32)g_I2CSlaver[X].Port.SDA.PIN << 16);\
          }while(0)

    /******************************************************************************
    * @���ƣ� BspI2CSlaverSDARead
    * @������ ��ȡSDA״̬ 
    ******************************************************************************/
    #define BspI2CSlaverSDARead(X) ((((g_I2CSlaver[X].Port.SDA.GPIOX)->IDR) & (g_I2CSlaver[X].Port.SDA.PIN)) != (u32)GPIO_PIN_RESET)

    /******************************************************************************
    * @���ƣ� I2CSlaverSCLRead
    * @������ ��ȡSCL��ƽ 
    ******************************************************************************/
    #define BspI2CSlaverSCLRead(X) (( ((g_I2CSlaver[X].Port.SCL.GPIOX)->IDR) & (g_I2CSlaver[X].Port.SCL.PIN) ) != (u32)GPIO_PIN_RESET)
 
    /******************************************************************************
    * @���ƣ� BspI2CSlaverReleasePort
    * @������ �ͷ�I2C��SCL��SDA�ӿ� 
    ******************************************************************************/
    #define BspI2CSlaverReleasePort( X ) \
    do{\
        BspI2CSlaverSCLHigh(X);\
        BspI2CSlaverSDAHigh(X);\
      }while(0)

	/******************************************************************************
    * @���ƣ� BspI2CSlaverProRxData
    * @������ ������յ������� master to slaver
    ******************************************************************************/
    #define BspI2CSlaverProRxData(X) \
    do{\
        /* USER CODE BEGIN SLAVER0*/\
        \
        switch(X)\
    	{\
    		case BSP_I2C_SLAVER_COM_MASTER:\
    			/*do someting:  ������յ������ݣ����ݴ��������DataTxBuf,������AppDriver��ʵ�ֽӿ�*/\
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
    * @���ƣ� BspI2CSlaverProTxData
    * @������ ������ȡ������Ϻ�������Ӧ����
    ******************************************************************************/
    #define BspI2CSlaverProTxData(X) \
    do{\
        /* USER CODE BEGIN SLAVER1*/\
        \
        switch(X)\
    	{\
    		case BSP_I2C_SLAVER_COM_MASTER:\
    			/*do someting: �������ˢ��DataTxBufΪȫ0xFF*/\
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
    * @���ƣ� BspI2CSlaverWDTStart
    * @������ �ӻ�I2C���Ź���������Ҫ�Ƕ���SCL�ߣ���ʱ�䲻��������Ϊ������
    ******************************************************************************/
    #define BspI2CSlaverWDTStart() \
    do{\
        __HAL_TIM_CLEAR_IT(&BSP_I2C_SLAVER_WDT, TIM_IT_UPDATE);\
        __HAL_TIM_ENABLE_IT(&BSP_I2C_SLAVER_WDT, TIM_IT_UPDATE);\
        __HAL_TIM_ENABLE(&BSP_I2C_SLAVER_WDT);\
    }while(0)
    
    /******************************************************************************
    * @���ƣ� BspI2CSlaverReceiveBuf
    * @������ �洢���յ������ݵ���ʱbuf��
    ******************************************************************************/
    #define BspI2CSlaverReceiveBuf( X, INDEX, VALUE ) \
    do{\
        g_I2CSlaver[X].DataRxBuf[(INDEX)%BSP_I2C_SLAVER_BLOCK_SIZE] = VALUE;\
    }while(0)

   
    /******************************************************************************
    * @���ƣ� BspI2CSlaverTransmitBuf
    * @������ ��ȡ���ͻ���buf������ݣ������ͳ�ȥ
    ******************************************************************************/
    //#define BspI2CSlaverTransmitBuf( X, INDEX ) g_I2CSlaver[X].DataTxBuf[INDEX%BSP_I2C_SLAVER_BLOCK_SIZE]

    
    /******************************************************************************
    * @���ƣ� BspI2CSlaverSCLRising
    * @������ ��SCL�����ش���
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
    * @���ƣ� BspI2CSlaverSCLRising
    * @������ ��SCL�½��ش���
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
    * @���ƣ� BspI2CSlaverSDARising
    * @������ ��SDA�����ش���
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
    * @���ƣ� BspI2CSlaverSDAFalling
    * @������ ��SDA�½��ش���
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
    * @���ƣ� BspI2CPinSclInt
    * @������ SCL���ŷ����жϵĴ�������Ӧ�ж��е��ã�����HAL_GPIO_EXTI_IRQHandler����ǰ�ǵõ���return���
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
    * @���ƣ� BspI2CPinSdaInt
    * @������ SDA���ŷ����жϵĴ�������Ӧ�ж��е��ã�����HAL_GPIO_EXTI_IRQHandler����ǰ�ǵõ���return���
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

