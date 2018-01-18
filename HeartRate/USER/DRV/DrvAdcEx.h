//adc 补充

#ifndef _DRV_ADC_EX_H_
#define _DRV_ADC_EX_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "config.h"
#include "adc.h"
#define ADC_SOURCE_NUM (10)
#define ADC_NUM        (5)

#define ADC_DELETE_MIN_NUM (2)
#define ADC_DELETE_MAX_NUM (2)
#define ADC_USEFUL_NUM  	(ADC_SOURCE_NUM - ADC_DELETE_MAX_NUM - ADC_DELETE_MIN_NUM)
#define   UBASIC		            (3.3)      //单片机基准电压;
#define   U_ACS0A                0.55f         // 电流传感器ACS758电流为0A时的Viout电压
#define   NOTEMPERATURE_IMPACT    1           //  电流传感器是否受温度影响，1与温度无关、0与温度有关
   
typedef struct
{
	u32 raw[ADC_SOURCE_NUM][ADC_NUM];
	u32 useful[ADC_NUM];
}AdcStruct;

extern __IO float g_Voltage[7];
extern float g_VoltageTemp[ADC_NUM];//产测会用到
extern float  Ic;
extern void AdcProcess(void);

#ifdef __cplusplus
}
#endif

#endif

/*********END OF FILE****/


