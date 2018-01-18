/*
******************************************************************************
*@�ļ�����  : DrvBaro.c
*@����	    : ��ѹ�ƶ�ȡ
******************************************************************************
*@����		: ��־��
*@ǿ���Թ涨: ��ֲʱֻ������ USER CODE BEGIN  USER CODE END  ֮��ɾ�Ĵ���!!!
*@�汾		: V1.00      2016/3/25
******************************************************************************
*/


#include "DrvBaro.h"

//#include "fuckvar.h"

#ifdef NUAA_SIMULATION
#include "fuckvar.h"
#include "dlink.h"
#endif

static u16 s_MS5611_PROM[BARO_NUM][8];


double sumspip_use[BARO_NUM] ={ 0.0 } ;


/******************************************************************************
* @�� �� ���� DrvBaroWriteCmd
* @���������� Baro���üĴ���
* @��    ����               
* @�� �� ֵ�� TRUE - д�ɹ�
              FALSE - дʧ��
* @��    ע��
******************************************************************************/ 
static u8 DrvBaroWriteCmd(BSP_I2C_MASTER_PORT_ENUM port, u8 reg)
{
  u8 NotUsed;
	/*//���Դ���
	static u8 Test = 0;

	if(Test == 0)
	{
    Test = 1;
    BspI2CMasterMemWrite(BSP_I2C_COM_PRESSURE1, 0xff, reg, MEM_ADD_SIZE_8BIT,&NotUsed, 0);
    trace(TEROR,"Test Send ERR Add First!!!");
    BspI2CMasterMemWrite(BSP_I2C_COM_PRESSURE1, DRV_MS5611_ADDR, reg, MEM_ADD_SIZE_8BIT,&NotUsed, 0);
    return;
	}*/
	if(BSP_I2C_ERR == BspI2CMasterMemWrite(port, DRV_MS5611_ADDR, reg, MEM_ADD_SIZE_8BIT,&NotUsed, 0))
	{
    SEGGER_RTT_printf(0,"BaroErr L %d\n",__LINE__);
    return FALSE;
	}
  
  return TRUE;
}


/******************************************************************************
* @�� �� ���� DrvBaroReadRom
* @���������� 
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע��
******************************************************************************/
static u8 DrvBaroMS5611ReadRom(BSP_I2C_MASTER_PORT_ENUM port, u16 *C)
{
	u8 RomTemp[2];

	if (BspI2CMasterMemRead(port, DRV_MS5611_ADDR, C0ADDR, MEM_ADD_SIZE_8BIT,RomTemp, 2)
    == BSP_I2C_ERR)
  {
    return FALSE;
  }
	C[0] = ((u16)RomTemp[0] << 8)|RomTemp[1];
	
	if (BspI2CMasterMemRead(port, DRV_MS5611_ADDR, C1ADDR, MEM_ADD_SIZE_8BIT,RomTemp, 2)
    == BSP_I2C_ERR)
  {
    return FALSE;
  }
	C[1] = ((u16)RomTemp[0] << 8)|RomTemp[1];

	if (BspI2CMasterMemRead(port, DRV_MS5611_ADDR, C2ADDR, MEM_ADD_SIZE_8BIT,RomTemp, 2)
    == BSP_I2C_ERR)
  {
    return FALSE;
  }
	C[2] = ((u16)RomTemp[0] << 8)|RomTemp[1];

	if (BspI2CMasterMemRead(port, DRV_MS5611_ADDR, C3ADDR, MEM_ADD_SIZE_8BIT,RomTemp, 2)
    == BSP_I2C_ERR)
  {
    return FALSE;
  }
	C[3] = ((u16)RomTemp[0] << 8)|RomTemp[1];

	if (BspI2CMasterMemRead(port, DRV_MS5611_ADDR, C4ADDR, MEM_ADD_SIZE_8BIT,RomTemp, 2)
    == BSP_I2C_ERR)
  {
    return FALSE;
  }
	C[4] = ((u16)RomTemp[0] << 8)|RomTemp[1];

	if (BspI2CMasterMemRead(port, DRV_MS5611_ADDR, C5ADDR, MEM_ADD_SIZE_8BIT,RomTemp, 2)
    == BSP_I2C_ERR)
  {
    return FALSE;
  }
	C[5] = ((u16)RomTemp[0] << 8)|RomTemp[1];

	if (BspI2CMasterMemRead(port, DRV_MS5611_ADDR, C6ADDR, MEM_ADD_SIZE_8BIT,RomTemp, 2)
    == BSP_I2C_ERR)
  {
    return FALSE;
  }
	C[6] = ((u16)RomTemp[0] << 8)|RomTemp[1]; 

	if (BspI2CMasterMemRead(port, DRV_MS5611_ADDR, C7ADDR, MEM_ADD_SIZE_8BIT,RomTemp, 2)
    == BSP_I2C_ERR)
  {
    return FALSE;
  }
	C[7] = ((u16)RomTemp[0] << 8)|RomTemp[1];

  return TRUE;
}

/******************************************************************************
* @�� �� ���� MS5611PromCrc
* @���������� ����MS5611��PROM CRCУ����
* @��    ���� 
* @�� �� ֵ�� CRCУ����
* @��    ע��
******************************************************************************/
static u8 MS5611PromCrc(u16 Prom[])
{
	s16 i;  // simple counter 
	u16 n_rem = 0; // crc reminder
	u16 Prom7 = Prom[7];  // original value of the crc
	u8 n_bit;
	
	
	Prom[7] = (0xFF00 & (Prom[7])); //CRC byte is replaced by 0
	for (i = 0; i < 16; i++) // operation is performed on bytes
	{  // choose LSB or MSB
		if (i%2==1) 
			n_rem ^= (u16) ((Prom[i>>1]) & 0x00FF);
		else 
			n_rem ^= (u16) (Prom[i>>1]>>8);
		for (n_bit = 8; n_bit > 0; n_bit--)
		{
			if (n_rem & (0x8000))
			{
				n_rem = (n_rem << 1) ^ 0x3000;
			}
			else
			{
				n_rem = (n_rem << 1);
			}
		}
	}
	n_rem= (0x000F & (n_rem >> 12)); // // final 4-bit reminder is CRC code
	Prom[7] = Prom7; // restore the crc_read to its original place
	return (n_rem ^ 0x00);
}

/******************************************************************************
* @�� �� ���� DrvBaroMS5611CheckPROM
* @���������� ��� MS5611 PROM���
* @��    ���� 
* @�� �� ֵ�� TRUE - ����
              FALSE - ������
* @��    ע��
******************************************************************************/
static u8 DrvBaroMS5611CheckPROM(BSP_I2C_MASTER_PORT_ENUM port,u16 *C)
{
  // Reset
  if (DrvBaroWriteCmd(port, DRV_MS5611_RESET) == FALSE)
  {
    return FALSE;
  }
  osDelay(5);
  
  // Read ROM
  if (DrvBaroMS5611ReadRom(port,C) == FALSE)
  {
    return FALSE;
  }
  
  // check PROM
  if (MS5611PromCrc(C) != (C[7] & 0xF))
  {
    SEGGER_RTT_printf(0,"BaroErr L %d MS5611PromCrc(C) %X C[7] %X\n ",__LINE__,MS5611PromCrc(C),C[7]);
    return FALSE;
  }
  
  return TRUE;
}

/******************************************************************************
* @�� �� ���� DrvBaroMS5611Init
* @���������� MS5611 ��ʼ��
* @��    ���� 
* @�� �� ֵ�� TRUE - ����
              FALSE - ������
* @��    ע��
******************************************************************************/
static u8 DrvBaroMS5611Init(BSP_I2C_MASTER_PORT_ENUM port, u8 id)
{
  if(DrvBaroMS5611CheckPROM(port, &s_MS5611_PROM[id][0]) == FALSE)
  {
    return FALSE;
  }
  
  return TRUE;
}


static u8 DrvBaroMS5611Check(BSP_I2C_MASTER_PORT_ENUM port,u16 prom,u8 id)
{
  u16 tmp;
	static u16 cmpBuf[8] = {0,0,0,0,0,0,0,0};//s_MS5611_PROM���ȫΪ0��˵��������
  u8 RomTemp[2];
  
  if (BspI2CMasterMemRead(port, DRV_MS5611_ADDR, C7ADDR, MEM_ADD_SIZE_8BIT,RomTemp, 2)
    == BSP_I2C_ERR)
  {
    return FALSE;
  }
	tmp = ((u16)RomTemp[0] << 8)|RomTemp[1];
  
  if ( (tmp == prom) && ( memcmp( s_MS5611_PROM[id], cmpBuf, sizeof(cmpBuf) ) ) )
  {
    return TRUE;
  }
  
  return FALSE;
}


/******************************************************************************
* @�� �� ���� DrvBaroInit
* @���������� ��ʼ����ѹ�ƣ��������reset
* @��    ���� ��
* @�� �� ֵ�� ��
* @��    ע��
******************************************************************************/ 
u8 DrvBaroInit(BSP_I2C_MASTER_PORT_ENUM port)
{
  u8 i;
  switch(port)
  {
    case BSP_I2C_COM_BARO1:
			i = 0;
			break;
		case BSP_I2C_COM_BARO2:
			i = 1;
			break;
		case BSP_I2C_COM_BARO3:
			i = 2;
			break;
		default:
			return FALSE;
  }
	return DrvBaroMS5611Init(port , i);
}


u8 DrvBaroCheck(BSP_I2C_MASTER_PORT_ENUM port)
{
  u8 i;
  switch(port)
  {
    case BSP_I2C_COM_BARO1:
			i = 0;
			break;
		case BSP_I2C_COM_BARO2:
			i = 1;
			break;
		case BSP_I2C_COM_BARO3:
			i = 2;
			break;
		default:
			return FALSE;
  }
  return DrvBaroMS5611Check(port,s_MS5611_PROM[i][7],i);
}






/******************************************************************************
* @�� �� ���� DrvBaroReset
* @���������� ����ѹ�ƽ���reset
* @��    ���� ��
* @�� �� ֵ�� ��
* @��    ע��
******************************************************************************/ 
void DrvBaroReset(BSP_I2C_MASTER_PORT_ENUM port)
{
	DrvBaroWriteCmd(port, DRV_MS5611_RESET);
}


//u8 DrvPressureNeedReset(DRV_PRESSURE_DATA_STRUCT* Pressure)
//{
//	return (Pressure->Reset != FALSE);
//}

//void DrvPressureClearReset(DRV_PRESSURE_DATA_STRUCT* Pressure)
//{
//	Pressure->Reset = FALSE;
//}




/******************************************************************************
* @�� �� ���� DrvPressureReadPerssure
* @���������� 
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע��
******************************************************************************/
static u8 DrvBaroReadPerssure(BSP_I2C_MASTER_PORT_ENUM port, u32* Pressure)
{
  // D1
  u8 PressureTemp[3];
  if(DrvBaroWriteCmd(port,D1C4096) == FALSE)
  {
    //osDelay(10);
    return FALSE;
  }
  osDelay(10);
  
  // read ADC of D1
  if(BspI2CMasterMemRead(port, DRV_MS5611_ADDR, ADCREAD, MEM_ADD_SIZE_8BIT,PressureTemp, 3) == FALSE)
  {
    //osDelay(10);
    return FALSE;
  }
  
  *Pressure = (PressureTemp[0] << 16) | (PressureTemp[1] << 8) | PressureTemp[2];
  osDelay(10);
  return TRUE;
}


/******************************************************************************
* @�� �� ���� AppDriverPressureReadTemperature
* @���������� 
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע��
******************************************************************************/
static u8 DrvBaroReadTemp(BSP_I2C_MASTER_PORT_ENUM port,u32* Temperature)
{
  // D2
  u8 TemperatureTemp[3];
  if(DrvBaroWriteCmd(port, D2C4096) == FALSE)
  {
    //osDelay(10);
    return FALSE;
  }
  osDelay(10);
  
  // read ADC of D2
  if(BspI2CMasterMemRead(port, DRV_MS5611_ADDR, ADCREAD, MEM_ADD_SIZE_8BIT,TemperatureTemp, 3) == FALSE)
  {
    //osDelay(10);
    return FALSE;
  }
  
  *Temperature = (TemperatureTemp[0] << 16) | (TemperatureTemp[1] << 8) | TemperatureTemp[2];
  osDelay(10);
  return TRUE;
}


static void DrvBaroProcess(u8 id,const u16* const C, const u32 Temperature, const u32 Pressure, s16 *t)
{
  static u32 ptr3_use[BARO_NUM] = {0,0,0};
	static double winspip_use[BARO_NUM][10] = { 0.0 };
  static double spipn[BARO_NUM]= { 0.0 };
	
  double dT;
  double TEMP;
  double OFF;
  double SENS;

  double T2 = 0;
  double Aux = 0;
  double OFF2 = 0;
  double SENS2 = 0;
	
  dT = Temperature - C[5]*256.0f;
  TEMP = 2000 + dT * C[6]/8388608.0f;//���ֲ�����δ����100�����洦��
  OFF = C[2] * 65536.0f + (C[4] * dT) / 128;//���ֲ᲻һ��
  SENS = C[1] * 32768.0f + (C[3] * dT) / 256;

  if (TEMP/100 >=20) 
  {
    T2 = 0;
    OFF2 = 0;
    SENS2 = 0;
  }
  else
  {
    T2 = (dT*dT) / 0x80000000;
    Aux = (TEMP-2000)*(TEMP-2000);
    OFF2 = 2.5f*Aux;
    SENS2 = 1.25f*Aux;

    if (TEMP/100 < -15) 
    {
      Aux = (TEMP+1500)*(TEMP+1500);
      OFF2 = OFF2+7*Aux;
      SENS2 = SENS2+5.5f*Aux;
    }
  }

  TEMP = TEMP - T2;
  OFF = OFF - OFF2;
  SENS = SENS - SENS2;

#if 1 //Z_ �����Ҫ���ݹ̶�������
  // Zero Process
  spipn[id] = (Pressure * SENS/2097152.0f - OFF)/32768.0f;//Pa
  
  *t = (s16)(TEMP/100);   // ��ֵ�¶ȣ���λ��
    
  // YANGLIN PROBLEM ��LOG�㷨�£���ʼֵ������GPS�ĸ߳���Ϣ����ѹ��
  // GPS��λ������sumspip_use��12����ʼ��

  sumspip_use[id] = sumspip_use[id] - winspip_use[id][ ptr3_use[id] ] + spipn[id];
  winspip_use[id][ ptr3_use[id]++ ] = spipn[id];		

#ifdef NUAA_SIMULATION  
  if(0 == fangzhenmode)
  {   
       HAL_IO.Baro_ps_pa = (float)(sumspip_use[0]/10);                 /* ��ǰ��ѹֵ(pa) */
//       HAL_IO.Baro_enabled = 1;               /* ��ѹ��������ݿ��� */
       HAL_IO.Baro_updated = !HAL_IO.Baro_updated;              /* ��ѹ�����ݸ����źţ���ת��ʾ�и��� */
    
       HAL_IO.Baro2_ps_pa = (float)(sumspip_use[1]/10);                 /* ��ǰ��ѹֵ(pa) */
    
       HAL_IO.Baro3_ps_pa = (float)(sumspip_use[2]/10);                 /* ��ǰ��ѹֵ(pa) */
  }	
#endif  
	ptr3_use[id] %= 10;
  		  
 #endif 
  
}




u8 DrvBaroRead(BSP_I2C_MASTER_PORT_ENUM port,s16 *t)
{
  u32 Temperature;
  u32 Pressure;

	u8 i;
  switch(port)
  {
    case BSP_I2C_COM_BARO1:
			i = 0;
			break;
		case BSP_I2C_COM_BARO2:
			i = 1;
			break;
		case BSP_I2C_COM_BARO3:
			i = 2;
			break;
		default:
			return FALSE;
  }
	
  if(DrvBaroReadTemp(port, &Temperature) == FALSE)
  {
    return FALSE;
  }
  
  if(DrvBaroReadPerssure(port,&Pressure) == FALSE)
  {
    return FALSE;
  }
	
  DrvBaroProcess(i, &s_MS5611_PROM[i][0], Temperature, Pressure, t);
  
  return TRUE;
}

/*********END OF FILE****/

