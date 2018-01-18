//adc ����

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
#define   UBASIC		            (3.3)      //��Ƭ����׼��ѹ;
#define   U_ACS0A                0.55f         // ����������ACS758����Ϊ0Aʱ��Viout��ѹ
#define   NOTEMPERATURE_IMPACT    1           //  �����������Ƿ����¶�Ӱ�죬1���¶��޹ء�0���¶��й�
   
typedef struct
{
	u32 raw[ADC_SOURCE_NUM][ADC_NUM];
	u32 useful[ADC_NUM];
}AdcStruct;

extern __IO float g_Voltage[7];
extern float g_VoltageTemp[ADC_NUM];//������õ�
extern float  Ic;
extern void AdcProcess(void);

#ifdef __cplusplus
}
#endif

#endif

/*********END OF FILE****/


