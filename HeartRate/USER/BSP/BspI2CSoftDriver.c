/*
******************************************************************************
*@文件名字  : BspI2CSoftDriver.c
*@描述	    : I2C主从机模拟，裸机和RTOS都支持
******************************************************************************
*@作者		: 丁志铖
*@强制性规定: 移植时只可以在 USER CODE BEGIN  USER CODE END  之间删改代码!!!
*@版本		: V1.00   2016/2/1  首次编写I2C软件模拟主从机   
*           V1.01   2016/3/2  丁志铖:更改了BspI2CSlaverProRxData函数的封装形式 
*           V1.02   2016/3/26 丁志铖:更改了BspI2CMasterStop开头两条语句的顺序，
*                             在某些速度飞快而时序超级严格的器件中并且
*                             在极端情况下会产生致命错误，例如气压计!!
*           V1.03   2016/3/28 丁志铖:将定时器回调函数放到BspTim.c中
*           V1.04   2016/4/1  丁志铖:更改I2C主模式一次读写字节数的数据类型为u16，以支持更长的数据包传送
*           V1.05   2016/4/9  丁志铖:BspI2CMasterDelayUS函数名改成BspI2CMasterDelay，把I2C从机绝大部分函数改成宏实现，提高通信速率，
*                                      目前stm32f1 64M主频 可以支持到110K以上，之前的函数实现代码只能到30K
*           V1.06   2016/5/25 吴力民:修改函数BspI2CMasterMemRead和BspI2CMasterReceive中，当NUM=0时，出现0-1的bug
*           V1.07   2016/7/13 丁志铖:更改延时策略，使每个端口的延时时间可以配置
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
    
    //定义协议处理函数指针，供Drv层注册。
    void (*BspI2CSlaverComMasterProRxData)(u8* RxBuf, u16 RxBufSize, u8* TxBuf);
    
    BSP_I2C_SLAVER_PORT_STRUCT g_I2CSlaver[BSP_I2C_SLAVER_PORT_NUM];
    static void BspI2CSlaverInit(void);
	static void BspI2CSlaverWDTInit(void);

    /* USER CODE END 0*/
#endif

#ifdef BSP_SUPPORT_I2C_SOFT_MASTER

/******************************************************************************
* @函 数 名： BspI2CMasterInit
* @函数描述： 初始化主模式I2C
* @参    数： 
* @返 回 值： 
* @备    注： 根据枚举类型BSP_I2C_MASTER_PORT_ENUM进行改变
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
* @函 数 名： BspI2CSlaverInit
* @函数描述： 初始化从模式I2C
* @参    数： 
* @返 回 值： 
* @备    注： 根据枚举类型BSP_I2C_SLAVER_PORT_ENUM进行改变
******************************************************************************/ 
static void BspI2CSlaverInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	memset(g_I2CSlaver,0,sizeof(g_I2CSlaver));
	
	/* USER CODE BEGIN 2*/
	g_I2CSlaver[BSP_I2C_SLAVER_COM_MASTER].Port.SCL.GPIOX = BSP_I2C_SLAVER_COM_MASTER_SCL_GPIO_Port;//根据Cube生成的名字填写
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



/*************************以上代码移植过程中需要修改********************************/
/*------------------------分割线---------------------------------------------------*/
/*************************以下代码移植过程中不需要修改******************************/



/******************************************************************************
* @函 数 名： BspI2CSoftInit
* @函数描述： 初始化I2C
* @参    数： 
* @返 回 值： 
* @备    注： 根据枚举类型BSP_I2C_PortTypeDef进行改变
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
* @函 数 名： BspI2CMasterDelay
* @函数描述： 延时时间，和具体单片机以及时钟频率有关
* @参    数： 
* @返 回 值： 
* @备    注：
******************************************************************************/
static void BspI2CMasterDelay(BSP_I2C_MASTER_PORT_ENUM Port)
{
    uint32_t i = 0;
	  uint32_t delay = 0;
	  if( Port >= BSP_I2C_MASTER_PORT_NUM )
	  {
	    while(1)//直接死掉，免得越界或者引起其他错误
	    {
//	      SEGGER_RTT_printf(0,"port >= BSP_I2C_MASTER_PORT_NUM\n");
	      osDelay(1000);
	    }
	  }

	  delay = s_I2CMaster[Port].delay;
	  
    while(delay--)    
    {        
        i = BSP_I2C_DELAY_NUM;    //自己定义       
        while(i--);         
    }
}

/******************************************************************************
* @函 数 名： BspI2CMasterSCLHigh
* @函数描述： SCL置高
* @参    数： 
* @返 回 值： 
* @备    注：
******************************************************************************/  
static void BspI2CMasterSCLHigh(BSP_I2C_MASTER_PORT_ENUM Port)
{
	s_I2CMaster[Port].Port.SCL.GPIOX->BSRR = s_I2CMaster[Port].Port.SCL.PIN; 
}

/******************************************************************************
* @函 数 名： BspI2CMasterSCLLow
* @函数描述： SCL置低
* @参    数： 
* @返 回 值： 
* @备    注：
******************************************************************************/  
static void BspI2CMasterSCLLow(BSP_I2C_MASTER_PORT_ENUM Port)
{
	s_I2CMaster[Port].Port.SCL.GPIOX->BSRR = (u32)s_I2CMaster[Port].Port.SCL.PIN << 16;
}

/******************************************************************************
* @函 数 名： SDAH
* @函数描述： SDA置高
* @参    数： 
* @返 回 值： 
* @备    注：
******************************************************************************/  
static void BspI2CMasterSDAHigh(BSP_I2C_MASTER_PORT_ENUM Port)
{
	s_I2CMaster[Port].Port.SDA.GPIOX->BSRR = s_I2CMaster[Port].Port.SDA.PIN;	
}

/******************************************************************************
* @函 数 名： BspI2CMasterSDALow
* @函数描述： SDA置低
* @参    数： 
* @返 回 值： 
* @备    注：
******************************************************************************/  
static void BspI2CMasterSDALow(BSP_I2C_MASTER_PORT_ENUM Port)
{
	s_I2CMaster[Port].Port.SDA.GPIOX->BSRR = (u32)s_I2CMaster[Port].Port.SDA.PIN << 16;	
}

/******************************************************************************
* @函 数 名： BspI2CMasterSDARead
* @函数描述： 读取SDA状态
* @参    数： 
* @返 回 值： 
* @备    注：
******************************************************************************/  
static GPIO_PinState BspI2CMasterSDARead(BSP_I2C_MASTER_PORT_ENUM Port)
{
	return HAL_GPIO_ReadPin(s_I2CMaster[Port].Port.SDA.GPIOX, s_I2CMaster[Port].Port.SDA.PIN); 
}

/******************************************************************************
* @函 数 名： BspI2CMasterStart
* @函数描述： 产生I2c起始信号
* @参    数： 
* @返 回 值： 
* @备    注：
******************************************************************************/ 
static void BspI2CMasterStart(BSP_I2C_MASTER_PORT_ENUM Port)
{
	BspI2CMasterSCLHigh(Port);
	BspI2CMasterSDAHigh(Port);
  BspI2CMasterDelay(Port);
  
  BspI2CMasterSDALow(Port);
  BspI2CMasterDelay(Port);
	
  BspI2CMasterSCLLow(Port);       //钳住I2C总线，准备发送或接收数据 
    //BspI2CMasterDelay(Port);
}	  


/******************************************************************************
* @函 数 名： BspI2CMasterStop
* @函数描述： 产生IIC停止信号
* @参    数： 
* @返 回 值： 
* @备    注：
******************************************************************************/ 
static void BspI2CMasterStop(BSP_I2C_MASTER_PORT_ENUM Port)
{
  BspI2CMasterSCLLow(Port);
  BspI2CMasterSDALow(Port);//避免变成start信号
  BspI2CMasterDelay(Port);

  BspI2CMasterSCLHigh(Port);
  BspI2CMasterDelay(Port);

  BspI2CMasterSDAHigh(Port);
  //BspI2CMasterDelay(Port);    
}


/******************************************************************************
* @函 数 名： BspI2CMasterWaitAck
* @函数描述： 等待应答信号到来
* @参    数： 
* @返 回 值： 接收应答成功0, 接收应答失败1
* @备    注：
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
* @函 数 名： BspI2CMasterAck
* @函数描述： 产生ACK应答
* @参    数： 
* @返 回 值： 
* @备    注：SDA = 0 为应答信号
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
* @函 数 名： BspI2CMasterNoAck
* @函数描述： 不产生ACK应
* @参    数： 
* @返 回 值： 
* @备    注：SDA = 1 为非应答信号
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
* @函 数 名： BspI2CMasterSendByte
* @函数描述： I2C发送一个字节
* @参    数： 
* @返 回 值： 
* @备    注：
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
* @函 数 名： BspI2CMasterReadByte
* @函数描述： 读1个字节，ack=1时，发送ACK，ack=0，发送nACK  
* @参    数： 
* @返 回 值： 
* @备    注：
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
        BspI2CMasterAck(Port);  //发送ACK
    }
    else
    {
        BspI2CMasterNoAck(Port);   //发送NACK  
    } 
    
    return RecvByte;    
    
}


/******************************************************************************
 * 函数原型: BspI2CMasterMemWrite										  *
 * 功能: 	 写1组数据给Memory类设备                                       *
 * 说明:                                        							  *
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
 * 函数原型: BspI2CMasterMemRead                                              *
 * 功能: 	 从Memory类设备读取1组数据                                        *
 * 说明:                                                                      *
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
	if(BSP_I2C_ERR == BspI2CMasterWaitAck(Port))                                            // 没有收到应答
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
 * 函数原型: BspI2CMasterMemWrite										      *
 * 功能: 	 写1组数据给通讯类设备                                            *
 * 说明:                                        							  *
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
 * 函数原型: BspI2CMasterReceive                                              *
 * 功能: 	 从通讯类设备读取1组数据                                                      *
 * 说明:                                                                      *
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
* @函 数 名： BspI2CSlaverWDTInit
* @函数描述： 从机I2C看门狗，监视到进入空闲模式则立马释放总线
* @参    数： 
* @返 回 值： 
* @备    注： 
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

