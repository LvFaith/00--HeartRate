/*
******************************************************************************
*@�ļ�����  : BspI2CSoftDriver.c
*@����	    : I2C���ӻ�ģ�⣬�����RTOS��֧��
******************************************************************************
*@����		: ��־��
*@ǿ���Թ涨: ��ֲʱֻ������ USER CODE BEGIN  USER CODE END  ֮��ɾ�Ĵ���!!!
*@�汾		: V1.00   2016/2/1  �״α�дI2C���ģ�����ӻ�   
*           V1.01   2016/3/2  ��־��:������BspI2CSlaverProRxData�����ķ�װ��ʽ 
*           V1.02   2016/3/26 ��־��:������BspI2CMasterStop��ͷ��������˳��
*                             ��ĳЩ�ٶȷɿ��ʱ�򳬼��ϸ�������в���
*                             �ڼ�������»������������������ѹ��!!
*           V1.03   2016/3/28 ��־��:����ʱ���ص������ŵ�BspTim.c��
*           V1.04   2016/4/1  ��־��:����I2C��ģʽһ�ζ�д�ֽ�������������Ϊu16����֧�ָ��������ݰ�����
*           V1.05   2016/4/9  ��־��:BspI2CMasterDelayUS�������ĳ�BspI2CMasterDelay����I2C�ӻ����󲿷ֺ����ĳɺ�ʵ�֣����ͨ�����ʣ�
*                                      Ŀǰstm32f1 64M��Ƶ ����֧�ֵ�110K���ϣ�֮ǰ�ĺ���ʵ�ִ���ֻ�ܵ�30K
*           V1.06   2016/5/25 ������:�޸ĺ���BspI2CMasterMemRead��BspI2CMasterReceive�У���NUM=0ʱ������0-1��bug
*           V1.07   2016/7/13 ��־��:������ʱ���ԣ�ʹÿ���˿ڵ���ʱʱ���������
******************************************************************************
*/

#include "BspI2CSoftDriver.h"


/* USER CODE BEGIN Includes */

/* USER CODE END Includes */


#ifdef BSP_SUPPORT_I2C_SOFT_MASTER
	static BSP_I2C_MASTER_PORT_STRUCT s_I2CMaster[BSP_I2C_MASTER_PORT_NUM];
  static void BspI2CMasterSCLHigh(BSP_I2C_MASTER_PORT_ENUM Port);
  static void BspI2CMasterSDAHigh(BSP_I2C_MASTER_PORT_ENUM Port);
	static void BspI2CMasterStop(BSP_I2C_MASTER_PORT_ENUM Port);
#endif

#ifdef BSP_SUPPORT_I2C_SOFT_SLAVER

    /* USER CODE BEGIN 0*/
    
    //����Э�鴦����ָ�룬��Drv��ע�ᡣ
    void (*BspI2CSlaverComMasterProRxData)(u8* RxBuf, u16 RxBufSize, u8* TxBuf);
    
    BSP_I2C_SLAVER_PORT_STRUCT g_I2CSlaver[BSP_I2C_SLAVER_PORT_NUM];
    static void BspI2CSlaverInit(void);
	static void BspI2CSlaverWDTInit(void);

    /* USER CODE END 0*/
#endif

#ifdef BSP_SUPPORT_I2C_SOFT_MASTER

/******************************************************************************
* @�� �� ���� BspI2CMasterInit
* @���������� ��ʼ����ģʽI2C
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע�� ����ö������BSP_I2C_MASTER_PORT_ENUM���иı�
******************************************************************************/ 
void BspI2CMasterInit(void)
{
	/* USER CODE BEGIN 1*/
	
	s_I2CMaster[BSP_I2C_COM_MAX].Port.SCL.GPIOX   = MAX_SCL_GPIO_Port;
	s_I2CMaster[BSP_I2C_COM_MAX].Port.SCL.PIN     = MAX_SCL_Pin;
	s_I2CMaster[BSP_I2C_COM_MAX].Port.SDA.GPIOX   = MAX_SDA_GPIO_Port;
	s_I2CMaster[BSP_I2C_COM_MAX].Port.SDA.PIN     = MAX_SDA_Pin;
	s_I2CMaster[BSP_I2C_COM_MAX].delay            = 10;
	/* USER CODE END 1*/
}
#endif

#ifdef BSP_SUPPORT_I2C_SOFT_SLAVER

/******************************************************************************
* @�� �� ���� BspI2CSlaverInit
* @���������� ��ʼ����ģʽI2C
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע�� ����ö������BSP_I2C_SLAVER_PORT_ENUM���иı�
******************************************************************************/ 
static void BspI2CSlaverInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	memset(g_I2CSlaver,0,sizeof(g_I2CSlaver));
	
	/* USER CODE BEGIN 2*/
	g_I2CSlaver[BSP_I2C_SLAVER_COM_MASTER].Port.SCL.GPIOX = BSP_I2C_SLAVER_COM_MASTER_SCL_GPIO_Port;//����Cube���ɵ�������д
	g_I2CSlaver[BSP_I2C_SLAVER_COM_MASTER].Port.SCL.PIN   = BSP_I2C_SLAVER_COM_MASTER_SCL_Pin;
	g_I2CSlaver[BSP_I2C_SLAVER_COM_MASTER].Port.SDA.GPIOX = BSP_I2C_SLAVER_COM_MASTER_SDA_GPIO_Port;
	g_I2CSlaver[BSP_I2C_SLAVER_COM_MASTER].Port.SDA.PIN   = BSP_I2C_SLAVER_COM_MASTER_SDA_Pin;
	g_I2CSlaver[BSP_I2C_SLAVER_COM_MASTER].Mode           = BSP_I2C_SLAVER_MODE_IDLE;

	GPIO_InitStruct.Pin = g_I2CSlaver[BSP_I2C_SLAVER_COM_MASTER].Port.SCL.PIN|g_I2CSlaver[BSP_I2C_SLAVER_COM_MASTER].Port.SDA.PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	BspI2CSlaverSCLHigh(BSP_I2C_SLAVER_COM_MASTER);
	BspI2CSlaverSDAHigh(BSP_I2C_SLAVER_COM_MASTER);
	
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	
	/* USER CODE END 2*/
	
	__HAL_TIM_DISABLE_IT(&BSP_I2C_SLAVER_WDT, TIM_IT_UPDATE);
    __HAL_TIM_DISABLE(&BSP_I2C_SLAVER_WDT);
	BspI2CSlaverWDTInit();
}



#endif



/*************************���ϴ�����ֲ��������Ҫ�޸�********************************/
/*------------------------�ָ���---------------------------------------------------*/
/*************************���´�����ֲ�����в���Ҫ�޸�******************************/



/******************************************************************************
* @�� �� ���� BspI2CSoftInit
* @���������� ��ʼ��I2C
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע�� ����ö������BSP_I2C_PortTypeDef���иı�
******************************************************************************/  
void BspI2CSoftInit(void)
{
	#ifdef BSP_SUPPORT_I2C_SOFT_MASTER
		BspI2CMasterInit();
	#endif

	#ifdef BSP_SUPPORT_I2C_SOFT_SLAVER
		BspI2CSlaverInit();
	#endif
}




#ifdef BSP_SUPPORT_I2C_SOFT_MASTER

/******************************************************************************
* @�� �� ���� BspI2CMasterDelay
* @���������� ��ʱʱ�䣬�;��嵥Ƭ���Լ�ʱ��Ƶ���й�
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע��
******************************************************************************/
static void BspI2CMasterDelay(BSP_I2C_MASTER_PORT_ENUM Port)
{
    uint32_t i = 0;
	  uint32_t delay = 0;
	  if( Port >= BSP_I2C_MASTER_PORT_NUM )
	  {
	    while(1)//ֱ�����������Խ�����������������
	    {
//	      SEGGER_RTT_printf(0,"port >= BSP_I2C_MASTER_PORT_NUM\n");
	      osDelay(1000);
	    }
	  }

	  delay = s_I2CMaster[Port].delay;
	  
    while(delay--)    
    {        
        i = BSP_I2C_DELAY_NUM;    //�Լ�����       
        while(i--);         
    }
}

/******************************************************************************
* @�� �� ���� BspI2CMasterSCLHigh
* @���������� SCL�ø�
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע��
******************************************************************************/  
static void BspI2CMasterSCLHigh(BSP_I2C_MASTER_PORT_ENUM Port)
{
	s_I2CMaster[Port].Port.SCL.GPIOX->BSRR = s_I2CMaster[Port].Port.SCL.PIN; 
}

/******************************************************************************
* @�� �� ���� BspI2CMasterSCLLow
* @���������� SCL�õ�
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע��
******************************************************************************/  
static void BspI2CMasterSCLLow(BSP_I2C_MASTER_PORT_ENUM Port)
{
	s_I2CMaster[Port].Port.SCL.GPIOX->BSRR = (u32)s_I2CMaster[Port].Port.SCL.PIN << 16;
}

/******************************************************************************
* @�� �� ���� SDAH
* @���������� SDA�ø�
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע��
******************************************************************************/  
static void BspI2CMasterSDAHigh(BSP_I2C_MASTER_PORT_ENUM Port)
{
	s_I2CMaster[Port].Port.SDA.GPIOX->BSRR = s_I2CMaster[Port].Port.SDA.PIN;	
}

/******************************************************************************
* @�� �� ���� BspI2CMasterSDALow
* @���������� SDA�õ�
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע��
******************************************************************************/  
static void BspI2CMasterSDALow(BSP_I2C_MASTER_PORT_ENUM Port)
{
	s_I2CMaster[Port].Port.SDA.GPIOX->BSRR = (u32)s_I2CMaster[Port].Port.SDA.PIN << 16;	
}

/******************************************************************************
* @�� �� ���� BspI2CMasterSDARead
* @���������� ��ȡSDA״̬
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע��
******************************************************************************/  
static GPIO_PinState BspI2CMasterSDARead(BSP_I2C_MASTER_PORT_ENUM Port)
{
	return HAL_GPIO_ReadPin(s_I2CMaster[Port].Port.SDA.GPIOX, s_I2CMaster[Port].Port.SDA.PIN); 
}

/******************************************************************************
* @�� �� ���� BspI2CMasterStart
* @���������� ����I2c��ʼ�ź�
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע��
******************************************************************************/ 
static void BspI2CMasterStart(BSP_I2C_MASTER_PORT_ENUM Port)
{
	BspI2CMasterSCLHigh(Port);
	BspI2CMasterSDAHigh(Port);
  BspI2CMasterDelay(Port);
  
  BspI2CMasterSDALow(Port);
  BspI2CMasterDelay(Port);
	
  BspI2CMasterSCLLow(Port);       //ǯסI2C���ߣ�׼�����ͻ�������� 
    //BspI2CMasterDelay(Port);
}	  


/******************************************************************************
* @�� �� ���� BspI2CMasterStop
* @���������� ����IICֹͣ�ź�
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע��
******************************************************************************/ 
static void BspI2CMasterStop(BSP_I2C_MASTER_PORT_ENUM Port)
{
  BspI2CMasterSCLLow(Port);
  BspI2CMasterSDALow(Port);//������start�ź�
  BspI2CMasterDelay(Port);

  BspI2CMasterSCLHigh(Port);
  BspI2CMasterDelay(Port);

  BspI2CMasterSDAHigh(Port);
  //BspI2CMasterDelay(Port);    
}


/******************************************************************************
* @�� �� ���� BspI2CMasterWaitAck
* @���������� �ȴ�Ӧ���źŵ���
* @��    ���� 
* @�� �� ֵ�� ����Ӧ��ɹ�0, ����Ӧ��ʧ��1
* @��    ע��
******************************************************************************/ 
static u8 BspI2CMasterWaitAck(BSP_I2C_MASTER_PORT_ENUM Port)
{  
    BspI2CMasterSCLLow(Port);
    //BspI2CMasterDelay(Port);
	
    BspI2CMasterSDAHigh(Port);  
    BspI2CMasterDelay(Port);
	
    BspI2CMasterSCLHigh(Port);
    BspI2CMasterDelay(Port);
    while(BspI2CMasterSDARead(Port))
    {
        BspI2CMasterStop(Port);
        return BSP_I2C_ERR;
    }
    BspI2CMasterSCLLow(Port);
    
    return BSP_I2C_OK;
} 


/******************************************************************************
* @�� �� ���� BspI2CMasterAck
* @���������� ����ACKӦ��
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע��SDA = 0 ΪӦ���ź�
*****************************************************************************/ 
static void BspI2CMasterAck(BSP_I2C_MASTER_PORT_ENUM Port)
{ 
    BspI2CMasterSCLLow(Port);
    //BspI2CMasterDelay(Port);
   
    BspI2CMasterSDALow(Port);
    BspI2CMasterDelay(Port);
  
    BspI2CMasterSCLHigh(Port);
    BspI2CMasterDelay(Port);
  
    BspI2CMasterSCLLow(Port);
    //BspI2CMasterDelay(Port);
}


/******************************************************************************
* @�� �� ���� BspI2CMasterNoAck
* @���������� ������ACKӦ
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע��SDA = 1 Ϊ��Ӧ���ź�
******************************************************************************/ 
static void BspI2CMasterNoAck(BSP_I2C_MASTER_PORT_ENUM Port)
{
    BspI2CMasterSCLLow(Port);
    //BspI2CMasterDelay(Port);
  
    BspI2CMasterSDAHigh(Port);
    BspI2CMasterDelay(Port);
  
    BspI2CMasterSCLHigh(Port);
    BspI2CMasterDelay(Port);
  
    BspI2CMasterSCLLow(Port);
    //BspI2CMasterDelay(Port);
}					 				     

/******************************************************************************
* @�� �� ���� BspI2CMasterSendByte
* @���������� I2C����һ���ֽ�
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע��
******************************************************************************/
static void BspI2CMasterSendByte(BSP_I2C_MASTER_PORT_ENUM Port, u8 SendByte)
{  
  u8 i = 8;

  while(i--)
  {
      BspI2CMasterSCLLow(Port);
      //BspI2CMasterDelay(Port);
      if(SendByte & 0x80)
      {
          BspI2CMasterSDAHigh(Port);
      }  
      else 
      {
          BspI2CMasterSDALow(Port); 
      }  
      SendByte <<= 1;
      BspI2CMasterDelay(Port);
      BspI2CMasterSCLHigh(Port);
      BspI2CMasterDelay(Port);
  }
  
  BspI2CMasterSCLLow(Port);
} 	    


/******************************************************************************
* @�� �� ���� BspI2CMasterReadByte
* @���������� ��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK  
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע��
******************************************************************************/
static u8 BspI2CMasterReadByte(BSP_I2C_MASTER_PORT_ENUM Port, u8 Ack)
{
    u8 i = 8;
    u8 RecvByte = 0;
	
    BspI2CMasterSDAHigh(Port);
  
    while(i--)
    {
        RecvByte <<= 1; 
 
        BspI2CMasterSCLLow(Port);
        BspI2CMasterDelay(Port);
		
        BspI2CMasterSCLHigh(Port);
        BspI2CMasterDelay(Port);
      
        if(BspI2CMasterSDARead(Port))
        {
            RecvByte |= 0x01;
        }
    }
    
    if (Ack)
    {
        BspI2CMasterAck(Port);  //����ACK
    }
    else
    {
        BspI2CMasterNoAck(Port);   //����NACK  
    } 
    
    return RecvByte;    
    
}


/******************************************************************************
 * ����ԭ��: BspI2CMasterMemWrite										  *
 * ����: 	 д1�����ݸ�Memory���豸                                       *
 * ˵��:                                        							  *
 ******************************************************************************/
u8 BspI2CMasterMemWrite(BSP_I2C_MASTER_PORT_ENUM Port, u8 DevAddress, u16 MemAddress, u8 MemAddSize, u8 *Ptr, u16 Num)
{
	u16 i;
	
	BspI2CMasterStart(Port);
	BspI2CMasterSendByte(Port, (DevAddress<<1) & BSP_I2C_ADDR_WRITE_MASK);
	if(BSP_I2C_ERR == BspI2CMasterWaitAck(Port))
	{
		return BSP_I2C_ERR;
	}

	if(MEM_ADD_SIZE_8BIT == MemAddSize)
	{
		BspI2CMasterSendByte(Port, U16_LSB(MemAddress));
		if(BSP_I2C_ERR == BspI2CMasterWaitAck(Port))
		{
			return BSP_I2C_ERR;
		}
	}
	else// MEM_ADD_SIZE_16BIT
	{
		BspI2CMasterSendByte(Port, U16_MSB(MemAddress));
		if(BSP_I2C_ERR == BspI2CMasterWaitAck(Port))
		{
			return BSP_I2C_ERR;
		}
		
		BspI2CMasterSendByte(Port, U16_LSB(MemAddress));
		if(BSP_I2C_ERR == BspI2CMasterWaitAck(Port))
		{
			return BSP_I2C_ERR;
		}
	}
	
	for(i = 0; i < Num; i++)
	{
		BspI2CMasterSendByte(Port, *Ptr);
		if(BSP_I2C_ERR == BspI2CMasterWaitAck(Port))
		{
			return BSP_I2C_ERR;
		}
		Ptr++;
	}
	BspI2CMasterStop(Port);
	return BSP_I2C_OK;
}

/******************************************************************************
 * ����ԭ��: BspI2CMasterMemRead                                              *
 * ����: 	 ��Memory���豸��ȡ1������                                        *
 * ˵��:                                                                      *
 ******************************************************************************/
u8 BspI2CMasterMemRead(BSP_I2C_MASTER_PORT_ENUM Port, u8 DevAddress, u16 MemAddress, u8 MemAddSize, u8 *Ptr, u16 Num)
{
	u16 i;

	if(Num == 0)
	{
		return BSP_I2C_ERR;
	}
	
	BspI2CMasterStart(Port);
	BspI2CMasterSendByte(Port, (DevAddress<<1) & BSP_I2C_ADDR_WRITE_MASK);
	if(BSP_I2C_ERR == BspI2CMasterWaitAck(Port))                                            // û���յ�Ӧ��
	{
		return BSP_I2C_ERR;
	}

	if(MEM_ADD_SIZE_8BIT == MemAddSize)
	{
		BspI2CMasterSendByte(Port, U16_LSB(MemAddress));
		if(BSP_I2C_ERR == BspI2CMasterWaitAck(Port))
		{
			return BSP_I2C_ERR;
		}
	}
	else// MEM_ADD_SIZE_16BIT
	{
		BspI2CMasterSendByte(Port, U16_MSB(MemAddress));
		if(BSP_I2C_ERR == BspI2CMasterWaitAck(Port))
		{
			return BSP_I2C_ERR;
		}
		
		BspI2CMasterSendByte(Port, U16_LSB(MemAddress));
		if(BSP_I2C_ERR == BspI2CMasterWaitAck(Port))
		{
			return BSP_I2C_ERR;
		}
	}
	
	BspI2CMasterStop(Port);
	
	BspI2CMasterStart(Port);
	BspI2CMasterSendByte(Port, (DevAddress<<1) | BSP_I2C_ADDR_READ_MASK);
	if(BSP_I2C_ERR == BspI2CMasterWaitAck(Port))
	{
		return BSP_I2C_ERR;
	}
	for(i = 0; i < Num - 1; i++)
	{
		*Ptr = BspI2CMasterReadByte(Port,BSP_I2C_ACK);
		Ptr++;
	}
	*Ptr = BspI2CMasterReadByte(Port,BSP_I2C_NO_ACK);
	BspI2CMasterStop(Port);
	return BSP_I2C_OK;
}

/******************************************************************************
 * ����ԭ��: BspI2CMasterMemWrite										      *
 * ����: 	 д1�����ݸ�ͨѶ���豸                                            *
 * ˵��:                                        							  *
 ******************************************************************************/
u8 BspI2CMasterTransmit(BSP_I2C_MASTER_PORT_ENUM Port, u8 SlaverAdd, u8 *Ptr, u16 Num)
{
	u16 i;
	
	BspI2CMasterStart(Port);
	BspI2CMasterSendByte(Port, (SlaverAdd<<1) & BSP_I2C_ADDR_WRITE_MASK);
	if(BspI2CMasterWaitAck(Port) == BSP_I2C_ERR)
	{
		return BSP_I2C_ERR;
	}

	for(i = 0; i < Num; i++)
	{
		BspI2CMasterSendByte(Port, *Ptr);
		if(BSP_I2C_ERR == BspI2CMasterWaitAck(Port))
		{
			return BSP_I2C_ERR;
		}
		Ptr++;
	}
	BspI2CMasterStop(Port);
	return BSP_I2C_OK;
}

/******************************************************************************
 * ����ԭ��: BspI2CMasterReceive                                              *
 * ����: 	 ��ͨѶ���豸��ȡ1������                                                      *
 * ˵��:                                                                      *
 ******************************************************************************/
u8 BspI2CMasterReceive(BSP_I2C_MASTER_PORT_ENUM Port, u8 DevAddress, u8 *Ptr, u16 Num)
{
	u16 i;

	if(Num == 0)
	{
		return BSP_I2C_ERR;
	}
	
	BspI2CMasterStart(Port);
	BspI2CMasterSendByte(Port, (DevAddress<<1) | BSP_I2C_ADDR_READ_MASK);
	if(BSP_I2C_ERR == BspI2CMasterWaitAck(Port))
	{
		return BSP_I2C_ERR;
	}
	
	for(i = 0; i < Num - 1; i++)
	{
		*Ptr = BspI2CMasterReadByte(Port,BSP_I2C_ACK);
		Ptr++;
	}
	*Ptr = BspI2CMasterReadByte(Port,BSP_I2C_NO_ACK);
	BspI2CMasterStop(Port);
	
	return BSP_I2C_OK;
}

#endif //end of #ifdef BSP_SUPPORT_I2C_SOFT_MASTER







#ifdef BSP_SUPPORT_I2C_SOFT_SLAVER


/******************************************************************************
* @�� �� ���� BspI2CSlaverWDTInit
* @���������� �ӻ�I2C���Ź������ӵ��������ģʽ�������ͷ�����
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע�� 
******************************************************************************/ 
static void BspI2CSlaverWDTInit(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;

	BSP_I2C_SLAVER_WDT.Instance = BSP_I2C_SLAVER_WDT_INSTANCE;
	BSP_I2C_SLAVER_WDT.Init.Prescaler = (BSP_I2C_SLAVER_WDT_SOURCE_HZ/10000 - 1);
	BSP_I2C_SLAVER_WDT.Init.CounterMode = TIM_COUNTERMODE_UP;
	BSP_I2C_SLAVER_WDT.Init.Period = (BSP_I2C_SLAVER_IDLE_TIME_MS*10 - 1);
	BSP_I2C_SLAVER_WDT.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&BSP_I2C_SLAVER_WDT);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&BSP_I2C_SLAVER_WDT, &sClockSourceConfig);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&BSP_I2C_SLAVER_WDT, &sMasterConfig);
}

#endif	//end of #ifdef BSP_SUPPORT_I2C_SOFT_SLAVER

/*********END OF FILE****/

