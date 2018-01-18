//adc补充
#include "DrvAdcEx.h"
#include "publicvar.h"
#include "AppInfo.h"


AdcStruct g_AdcData;

//0:飞控电压 1:舵机电压 2:点火电压 3:旋翼动力电压 4:固定翼动力电压 5:启动电压  6：电流
__IO float g_Voltage[7] = {0.0f};
float g_VoltageTemp[ADC_NUM] = {0.0f};

float  Ic;


static HAL_StatusTypeDef ConfigAdcChannel(u32 channel )
{
  ADC_ChannelConfTypeDef sConfig;
	
	sConfig.Channel = channel;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		return HAL_ERROR;
	}

	return HAL_OK;
}


static void AdcAver(void)
{
	u32 AdcProTemp[ADC_SOURCE_NUM][ADC_NUM];
	u32 ValueTemp;
	s16 i,j,k;
	
	memcpy( AdcProTemp, g_AdcData.raw, sizeof(AdcProTemp) );

	for(j = 0; j < ADC_NUM; j++)//排序，找出最大和最小的几个，以便扔掉
	{
		for(k = 0; k < (ADC_SOURCE_NUM - ADC_DELETE_MIN_NUM); k++)//找出最大的SOURCE_NUM -DELETE_MIN_NUM 个
		{
			for(i = 0; i < (ADC_SOURCE_NUM-1-k); i++)
			{
				if(AdcProTemp[i][j] > AdcProTemp[i+1][j])
				{
					ValueTemp 			= AdcProTemp[i][j];
					AdcProTemp[i][j] 	= AdcProTemp[i+1][j];
					AdcProTemp[i+1][j] 	= ValueTemp;
				}
			}
		}
	}
	

	for(j = 0; j < ADC_NUM; j++)
	{
		ValueTemp = 0;
		for(i = ADC_DELETE_MIN_NUM; i < (ADC_DELETE_MIN_NUM + ADC_USEFUL_NUM); i++)
		{
			ValueTemp += AdcProTemp[i][j];//g_AdcDmaStatus.Value[i][j];
		}
		
		g_AdcData.useful[j] = (ValueTemp/ADC_USEFUL_NUM);
	}

  if ( BOARD_VERSION_VA == GetBoardVersion() )
  {
  	for(int index = 0; index < ADC_NUM-3; index++)
  	{
  	  g_VoltageTemp[index] = ( g_AdcData.useful[index] * 3.3f/4095.0f - 1.65f ) * 40.0f;
  	}
    
    for(int index = ADC_NUM-3; index < ADC_NUM-1; index++)
  	{
  	  g_VoltageTemp[index] = ( g_AdcData.useful[index] * 3.3f/4095.0f ) * 4.0f;
  	}
  
  	g_VoltageTemp[ADC_NUM-1] = g_AdcData.useful[ADC_NUM-1] * 3.3f * 11.0f /4095.0f;
  }
  else if( BOARD_VERSION_VB == GetBoardVersion() )
  {
    g_VoltageTemp[4] = g_AdcData.useful[4] * 33.15f/4095.0f;
    for(int index = 0; index < 2; index++)
  	{
  	  g_VoltageTemp[index] = g_AdcData.useful[index] * 331.5f/4095.0f/4.7f;
  	}

    for(int index = 2; index < 4; index++)
  	{
  	  g_VoltageTemp[index] = g_AdcData.useful[index] * 331.5f/4095.0f/22.1f;
  	}
  }
  else
  {
    memset( g_VoltageTemp, 0, sizeof(g_VoltageTemp) );
  }
  
  g_Voltage[0] = g_VoltageTemp[4];
	g_Voltage[1] = g_VoltageTemp[3];
	if( (UAV_TYPE >= 100) && (UAV_TYPE < 150) )//固定翼电动
	{
		g_Voltage[2] = 0;
		g_Voltage[3] = 0;
		g_Voltage[4] = g_VoltageTemp[1];
		g_Voltage[5] = 0;
		g_Voltage[6] = g_VoltageTemp[2];
	}
	else if( (UAV_TYPE >= 150) && (UAV_TYPE < 200) )//固定翼油动
	{
		g_Voltage[2] = g_VoltageTemp[1];
		g_Voltage[3] = 0;
		g_Voltage[4] = 0;
		g_Voltage[5] = g_VoltageTemp[0];
		g_Voltage[6] = 0;
	}
	else if( (UAV_TYPE >= 200) && (UAV_TYPE < 230) )//垂直起降油动
	{
		g_Voltage[2] = g_VoltageTemp[1];
		g_Voltage[3] = g_VoltageTemp[0];
		g_Voltage[4] = 0;
		g_Voltage[5] = 0;
		g_Voltage[6] = 0;
	}
	else if( UAV_TYPE >= 230 )                      //垂直起降电动
	{
		g_Voltage[2] = 0;
		g_Voltage[3] = g_VoltageTemp[0];
		g_Voltage[4] = g_VoltageTemp[1];
		g_Voltage[5] = 0;
		g_Voltage[6] = g_VoltageTemp[2];
	}
	
}

static void  I_calculateProcess(void)
{
    float tmp_UI;
    tmp_UI = g_Voltage[6]- U_ACS0A;                      //减去0A时的Viout基础电压
    if (NOTEMPERATURE_IMPACT)                                 // 电流传感器忽略温度影响
    {
      Ic = (tmp_UI*1000.0f/40.0f)/0.9f;
    }

    if (Ic<1e-3f)   
    {
      Ic = 1e-6;
    }    
}

void AdcProcess(void)
{
  static u8 i = 0, j = 0;
	static u32 channel;
  switch(i)
  {
    case 0:
			channel = ADC_CHANNEL_8;
			break;

		case 1:
			channel = ADC_CHANNEL_9;
			break;

		case 2:
			channel = ADC_CHANNEL_14;
			break;

		case 3:
			channel = ADC_CHANNEL_15;
			break;

		case 4:
			channel = ADC_CHANNEL_13;
			break;
  }
	
  ConfigAdcChannel(channel);
	HAL_ADC_Start(&hadc1);
  osDelay(2);
	g_AdcData.raw[j][i] = (HAL_ADC_GetValue(&hadc1) & 0x0FFF);
	//g_Vol[i] = (s_AdcRawData[i] * 3.3f / 4095.0f - 1.65f) * 40.0f;

	if( ++i >= ADC_NUM)
	{
	  i = 0;
		if(++j >= ADC_SOURCE_NUM )
		{
		  j = 0;
			AdcAver();
      I_calculateProcess();
		}
	}
}

