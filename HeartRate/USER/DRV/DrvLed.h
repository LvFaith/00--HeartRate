/*
******************************************************************************
*@文件名字  : DrvLed.h
*@描述	    : Led驱动层
******************************************************************************
*@作者		: 丁志铖
*@强制性规定: 移植时只可以在 USER CODE BEGIN  USER CODE END  之间删改代码!!!
*@版本		: V1.00      2017/6/28
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
	DRV_LED_OFF		=	0,		//灭灯
	DRV_LED_RED			,		//红色
	DRV_LED_BLUE		,		//蓝色
	DRV_LED_PURPLE		,		//紫色
	DRV_LED_GREEN		,		//绿色
	DRV_LED_YELLOW		,		//黄色
	DRV_LED_LIGHT_BLUE	,		//浅蓝
	DRV_LED_WHITE		,		//白色
	DRV_LED_COLOR_NUM	,		
}DRV_LED_COLOR_ENUM;

typedef enum
{
	DRV_LED_MODE_OFF		=	0,	//灭灯
  DRV_LED_MODE_ON          ,      //长亮
  DRV_LED_MODE_BLINK_QUICK ,      //快速闪烁
  DRV_LED_MODE_BLINK_NORMAL,      //慢闪烁
}DRV_LED_MODE_ENUM;

typedef enum
{
	DRV_LED_BLINK_MODE_OFF		=	0,	//灭灯
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
  u16 Coefficient;//亮度系数
	DRV_LED_GROUP_STRUCT Group;
  DRV_LED_MODE_ENUM Mode;
}DRV_LED_STRUCT;

/*************************对外接口变量********************************************/

extern DRV_LED_STRUCT g_Led;

/*************************对外接口函数********************************************/

extern void DrvLedInit(void);
extern void DrvLedPro(void);


#ifdef __cplusplus
}
#endif

#endif

