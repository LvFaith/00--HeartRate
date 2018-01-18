/*
******************************************************************************
*@�ļ�����  : DrvLed.h
*@����	    : Led������
******************************************************************************
*@����		: ��־��
*@ǿ���Թ涨: ��ֲʱֻ������ USER CODE BEGIN  USER CODE END  ֮��ɾ�Ĵ���!!!
*@�汾		: V1.00      2017/6/28
******************************************************************************
*/


#ifndef _DRV_LED_H_
#define _DRV_LED_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "config.h"
   
typedef enum
{
	DRV_LED_OFF		=	0,		//���
	DRV_LED_RED			,		//��ɫ
	DRV_LED_BLUE		,		//��ɫ
	DRV_LED_PURPLE		,		//��ɫ
	DRV_LED_GREEN		,		//��ɫ
	DRV_LED_YELLOW		,		//��ɫ
	DRV_LED_LIGHT_BLUE	,		//ǳ��
	DRV_LED_WHITE		,		//��ɫ
	DRV_LED_COLOR_NUM	,		
}DRV_LED_COLOR_ENUM;

typedef enum
{
	DRV_LED_MODE_OFF		=	0,	//���
  DRV_LED_MODE_ON          ,      //����
  DRV_LED_MODE_BLINK_QUICK ,      //������˸
  DRV_LED_MODE_BLINK_NORMAL,      //����˸
}DRV_LED_MODE_ENUM;

typedef enum
{
	DRV_LED_BLINK_MODE_OFF		=	0,	//���
  DRV_LED_BLINK_MODE_FIRST_ON   =   1,   //
  DRV_LED_BLINK_MODE_SECOND_ON  =   3,   //
  DRV_LED_BLINK_MODE_THIRD_ON   =   5,   //
  DRV_LED_BLINK_MODE_FOURTH_ON  =   7,   //
}DRV_LED_BLINK_MODE_ENUM;


typedef struct
{
	DRV_LED_COLOR_ENUM  First;
  DRV_LED_COLOR_ENUM  Second;
  DRV_LED_COLOR_ENUM  Third;
  DRV_LED_COLOR_ENUM  Fourth;
}DRV_LED_GROUP_STRUCT;

typedef struct
{
  u16 Coefficient;//����ϵ��
	DRV_LED_GROUP_STRUCT Group;
  DRV_LED_MODE_ENUM Mode;
}DRV_LED_STRUCT;

/*************************����ӿڱ���********************************************/

extern DRV_LED_STRUCT g_Led;

/*************************����ӿں���********************************************/

extern void DrvLedInit(void);
extern void DrvLedPro(void);


#ifdef __cplusplus
}
#endif

#endif

