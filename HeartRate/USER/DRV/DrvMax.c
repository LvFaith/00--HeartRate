/*
******************************************************************************
*@文件名字  : DrvBaro.c
*@描述	    : 气压计读取
******************************************************************************
*@作者		: 丁志铖
*@强制性规定: 移植时只可以在 USER CODE BEGIN  USER CODE END  之间删改代码!!!
*@版本		: V1.00      2016/3/25
******************************************************************************
*/


#include "DrvMax.h"



/******************************************************************************
* @函 数 名： DrvDrvMaxWriteCmd
* @函数描述： MAX32102向一个寄存器写入data
* @参    数：               
* @返 回 值： TRUE - 写成功
              FALSE - 写失败
* @备    注：
******************************************************************************/ 
u8 DrvDrvMaxWriteCmd(BSP_I2C_MASTER_PORT_ENUM port, u8 reg, u8 data)
{
  u8 data_send = data;
	/*//测试错误
	static u8 Test = 0;

	if(Test == 0)
	{
    Test = 1;
    BspI2CMasterMemWrite(BSP_I2C_COM_PRESSURE1, 0xff, reg, MEM_ADD_SIZE_8BIT,&NotUsed, 0);
    trace(TEROR,"Test Send ERR Add First!!!");
    BspI2CMasterMemWrite(BSP_I2C_COM_PRESSURE1, DRV_MS5611_ADDR, reg, MEM_ADD_SIZE_8BIT,&NotUsed, 0);
    return;
	}*/
	if(BSP_I2C_ERR == BspI2CMasterMemWrite(port, DRV_MAX_ADDR, reg, MEM_ADD_SIZE_8BIT,&data_send, 1))
	{
    SEGGER_RTT_printf(0,"MAX32012Err L %d\n",__LINE__);
    return FALSE;
	}
  
  return TRUE;
}


/******************************************************************************
* @函 数 名： DrvBaroReadRom
* @函数描述： 
* @参    数： 
* @返 回 值： 
* @备    注：
******************************************************************************/
u8 DrvBaroMaxReadData(BSP_I2C_MASTER_PORT_ENUM port, u8 reg, u8 *rxdata, u8 num)
{
  if( BspI2CMasterMemRead(port, DRV_MAX_ADDR, reg, MEM_ADD_SIZE_8BIT, rxdata, num))
  {
    SEGGER_RTT_printf(0,"MAX32012Err L %d\n",__LINE__);
    return FALSE;
	}
  return TRUE;
}





/******************************************************************************
* @函 数 名： DrvBaroMS5611Init
* @函数描述： MS5611 初始化
* @参    数： 
* @返 回 值： TRUE - 正常
              FALSE - 不正常
* @备    注：
******************************************************************************/
u8 DrvBaroMaxInit(void)
{
//  if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, 0x09, 0x0b) == FALSE)
//  {
//    return FALSE;
//  }
//  if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, 0x01, 0xF0) == FALSE)
//  {
//    return FALSE;
//  }
//  if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, 0x00, 0x00) == FALSE)
//  {
//    return FALSE;
//  }
//  if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, 0x03, 0x02) == FALSE)
//  {
//    return FALSE;
//  }
//  if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, 0x21, 0x01) == FALSE)
//  {
//    return FALSE;
//  }
//  if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, 0x0a, 0x47) == FALSE)
//  {
//    return FALSE;
//  }
//  if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, 0x0c, 0x47) == FALSE)
//  {
//    return FALSE;
//  }
//  if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, 0x0d, 0x47) == FALSE)
//  {
//    return FALSE;
//  }
    
    
    if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, REG_INTR_ENABLE_1, 0xc0) == FALSE)   // INTR setting1     add = 0x02
    {
      return FALSE;
    }
    if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, REG_INTR_ENABLE_2, 0x02) == FALSE)   // INTR setting2    add = 0x03
    {
      return FALSE;
    }
    if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, REG_FIFO_WR_PTR, 0x00) == FALSE)    //FIFO_WR_PTR[4:0]    add = 0x04
    {
      return FALSE;
    }
    if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, REG_OVF_COUNTER, 0x00) == FALSE)   //OVF_COUNTER[4:0]   add = 0x05
    {
      return FALSE;
    }
    if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, REG_FIFO_RD_PTR, 0x00) == FALSE)   //FIFO_RD_PTR[4:0]   add = 0x06
    {
      return FALSE;
    }
    if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, REG_FIFO_CONFIG, 0x0f) == FALSE)   //sample avg = 1, fifo rollover=false, fifo almost full = 17   add = 0x08
    {
      return FALSE;
    }
    if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, REG_MODE_CONFIG, 0x03) == FALSE)  //0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED    add = 0x09
    {
      return FALSE;
    }
    if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, REG_SPO2_CONFIG, 0x27) == FALSE)  // SPO2_ADC range = 4096nA, SPO2 sample rate (100 Hz), LED pulseWidth (400uS)   add = 0x0A
    {
      return FALSE;
    }
/*----------------------------------------------------------------------------------------------------------------------------*/    
    if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, REG_LED1_PA, 0x24) == FALSE)   //Choose value for ~ 7mA for LED1    add = 0x0C
    {
      return FALSE;
    }
    if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, REG_LED2_PA, 0x24) == FALSE)   // Choose value for ~ 7mA for LED2   add = 0x0D
    {
      return FALSE;
    }
    if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, REG_PILOT_PA, 0x7f) == FALSE)  // Choose value for ~ 25mA for Pilot LED   add = 0x10
    {
      return FALSE;
    }
    if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, REG_TEMP_CONFIG, 0x01) == FALSE)  // En temp    add = 0x21
    {
      return FALSE;
    }
  return TRUE;
}


/******************************************************************************
* @函 数 名： DrvBaroReset
* @函数描述： 对气压计进行reset
* @参    数： 无
* @返 回 值： 无
* @备    注：
******************************************************************************/ 
//void DrvBaroReset(BSP_I2C_MASTER_PORT_ENUM port)
//{
//	DrvBaroWriteCmd(port, DRV_MS5611_RESET);
//}


/******************************************************************************
* @函 数 名： DrvPressureReadPerssure
* @函数描述： 
* @参    数： 
* @返 回 值： 
* @备    注：
******************************************************************************/
//static u8 DrvBaroReadPerssure(BSP_I2C_MASTER_PORT_ENUM port, u32* Pressure)
//{
//  // D1
//  u8 PressureTemp[3];
//  if(DrvBaroWriteCmd(port,D1C4096) == FALSE)
//  {
//    //osDelay(10);
//    return FALSE;
//  }
//  osDelay(10);
//  
//  // read ADC of D1
//  if(BspI2CMasterMemRead(port, DRV_MS5611_ADDR, ADCREAD, MEM_ADD_SIZE_8BIT,PressureTemp, 3) == FALSE)
//  {
//    //osDelay(10);
//    return FALSE;
//  }
//  
//  *Pressure = (PressureTemp[0] << 16) | (PressureTemp[1] << 8) | PressureTemp[2];
//  osDelay(10);
//  return TRUE;
//}


/******************************************************************************
* @函 数 名： u8 DrvBaroReadTemp(u8* Temperature)
* @函数描述： 
* @参    数： 
* @返 回 值： 
* @备    注：
******************************************************************************/
u8 DrvBaroReadTemp(u8* Temperature)
{
  u8 temp_int = 0, temp_fraction = 0;
  if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, REG_TEMP_CONFIG, 0x01) == FALSE)
  {
    return FALSE;
  }
  if(DrvBaroMaxReadData(BSP_I2C_COM_MAX, REG_TEMP_INTR, Temperature,1) == FALSE)
  {
    return FALSE;
  }
//  if(DrvDrvMaxWriteCmd(BSP_I2C_COM_MAX, REG_TEMP_CONFIG, 0x01) == FALSE)
//  {
//    return FALSE;
//  }
//  if(DrvBaroMaxReadData(BSP_I2C_COM_MAX, REG_TEMP_FRAC, &temp_fraction,1) == FALSE)
//  {
//    return FALSE;
//  }
//  *Temperature = temp_int + temp_fraction*0.0625;
  return TRUE;
}






/*********END OF FILE****/

