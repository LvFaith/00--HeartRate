/*
******************************************************************************
*@文件名字  : DrvLed.c
*@描述	    : Led驱动层
******************************************************************************
*@作者		: 丁志铖
*@强制性规定: 移植时只可以在 USER CODE BEGIN  USER CODE END  之间删改代码!!!
*@版本		: V1.00      2016/3/29
******************************************************************************
*/

#include "DrvLed.h"
#include "gpio.h"

DRV_LED_STRUCT g_Led;
//u8 test[102400] = {0};


/******************************************************************************
* @函 数 名： DrvLedInit
* @函数描述： 初始化LED相关数据，函数注册等
* @参    数： 
* @返 回 值：
* @备    注：
******************************************************************************/ 
void DrvLedInit(void)
{
  memset(&g_Led, 0, sizeof(g_Led));
  g_Led.Coefficient = 1; //防止灯不亮，亮度没用，因为不采用PWM控制
}

static void DrvLedConfigSet(int TailRed,int TailGreen ,int TailBlue)
{
  //原理图画错了，RED和GREEN反了
  HAL_GPIO_WritePin(LED_RED_GPIO_OUTPUT_GPIO_Port,   LED_RED_GPIO_OUTPUT_Pin,   TailRed   ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_GREEN_GPIO_OUTPUT_GPIO_Port, LED_GREEN_GPIO_OUTPUT_Pin, TailGreen ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_BLUE_GPIO_OUTPUT_GPIO_Port,  LED_BLUE_GPIO_OUTPUT_Pin,  TailBlue  ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/******************************************************************************
* @函 数 名： DrvLedSetColor
* @函数描述： 设置尾灯颜色
* @参    数： 
* @返 回 值：
* @备    注：
******************************************************************************/ 
static void DrvLedSetColor(DRV_LED_COLOR_ENUM Color )
{
    u16 Red,Green,Blue;
    
    /*if(PowerOff>0)
    {
        HalLedLight=0;
    }*/
    
    switch(Color)
    {
        case DRV_LED_OFF:                       //灭
            Green   =      0 * g_Led.Coefficient;  
            Blue    =      0 * g_Led.Coefficient;  
            Red     =      0 * g_Led.Coefficient;
            break; 
        
        case DRV_LED_RED:                       //红    
            Green   =      0 * g_Led.Coefficient;  
            Blue    =      0 * g_Led.Coefficient;
            Red     =     25 * g_Led.Coefficient;
            break; 
        
        case DRV_LED_BLUE:                      //蓝
            Green   =      0 * g_Led.Coefficient;  
            Blue    =     25 * g_Led.Coefficient;  
            Red     =      0 * g_Led.Coefficient;
            break;
        
        case DRV_LED_PURPLE:                    //紫
            Green   =      0 * g_Led.Coefficient;  
            Blue    =     25 * g_Led.Coefficient;  
            Red     =     25 * g_Led.Coefficient;
            break;
        
        case DRV_LED_GREEN:                     //绿
            Green   =     20 * g_Led.Coefficient; //2500*0.8 
            Blue    =      0 * g_Led.Coefficient; 
            Red     =      0 * g_Led.Coefficient;
            break;
        
        case DRV_LED_YELLOW:                    //黄
            Green   =     15 * g_Led.Coefficient; //2500*0.6 
            Blue    =      0 * g_Led.Coefficient;  
            Red     =     25 * g_Led.Coefficient;
            break;
        
        case DRV_LED_LIGHT_BLUE:                //浅蓝
            Green   =     20 * g_Led.Coefficient; //2500*0.8  
            Blue    =     25 * g_Led.Coefficient;  
            Red     =      0 * g_Led.Coefficient;
            break;
        
        case DRV_LED_WHITE:                     //白
            Green   =    25 * g_Led.Coefficient;  
            Blue    =    25 * g_Led.Coefficient;  
            Red     =    25 * g_Led.Coefficient;
            break;
        
        default:                            //灭
            Green   =      0 * g_Led.Coefficient;  
            Blue    =      0 * g_Led.Coefficient;  
            Red     =      0 * g_Led.Coefficient;
            break;
    }
    
    
    DrvLedConfigSet(Red, Green, Blue);
        
     
   
}

static void DrvLedBlinkQuick(DRV_LED_GROUP_STRUCT Group)//100hz
{
  static u8 i = 0,j = 0;
  static DRV_LED_BLINK_MODE_ENUM Step = DRV_LED_BLINK_MODE_OFF;

	if( (++j) > 10 )
	{
	  j = 0;
    i++;
    i = i%8;
    if(i%2)
      Step = (DRV_LED_BLINK_MODE_ENUM)i;
    else
      Step = DRV_LED_BLINK_MODE_OFF;
        
    switch(Step)
    {
      case DRV_LED_BLINK_MODE_FIRST_ON:
        DrvLedSetColor(Group.First);
        break;

      case DRV_LED_BLINK_MODE_SECOND_ON:
        DrvLedSetColor(Group.Second);
        break;

      case DRV_LED_BLINK_MODE_THIRD_ON:
        DrvLedSetColor(Group.Third);
        break;

      case DRV_LED_BLINK_MODE_FOURTH_ON:
        DrvLedSetColor(Group.Fourth);
        break;
          
      default:
        DrvLedSetColor(DRV_LED_OFF);
        break;   
    }
	}
}

static void DrvLedBlinkNormal(DRV_LED_GROUP_STRUCT Group)//100hz
{
    static u8 i = 0, j = 4, k = 0;
    static DRV_LED_BLINK_MODE_ENUM Step = DRV_LED_BLINK_MODE_OFF;

    if( (++k) > 10 )
    {
      k = 0;
      j++;
      j = j%5;
      if(0 == j)
      {
        i++;
        i = i%8;
        if(i%2)
          Step = (DRV_LED_BLINK_MODE_ENUM)i;
        else
          Step = DRV_LED_BLINK_MODE_OFF;
      }
          
      switch(Step)
      {
        case DRV_LED_BLINK_MODE_FIRST_ON:
          DrvLedSetColor(Group.First);
          break;

        case DRV_LED_BLINK_MODE_SECOND_ON:
          DrvLedSetColor(Group.Second);
          break;

        case DRV_LED_BLINK_MODE_THIRD_ON:
          DrvLedSetColor(Group.Third);
          break;

        case DRV_LED_BLINK_MODE_FOURTH_ON:
          DrvLedSetColor(Group.Fourth);
          break;
            
        default:
          DrvLedSetColor(DRV_LED_OFF);
          break;     
      }
    }
    
}


static void DrvLedTailSet(DRV_LED_MODE_ENUM Mode, DRV_LED_GROUP_STRUCT Group)//100Hz
{ 
    switch(Mode)
    {
    
        case DRV_LED_MODE_OFF:
            DrvLedSetColor(DRV_LED_OFF);
            break;
            
        case DRV_LED_MODE_ON:
            DrvLedSetColor(Group.First);
            break;

        case DRV_LED_MODE_BLINK_QUICK:
            DrvLedBlinkQuick(Group);
            break;

        case DRV_LED_MODE_BLINK_NORMAL:
            DrvLedBlinkNormal(Group);
            break;
            
    }
}


void DrvLedPro(void)
{
#if 0
    if(FALSE != DrvFileGetErrFlag())
    {
        g_Led.Coefficient = 100;
        g_Led.Mode = DRV_LED_MODE_BLINK_QUICK;
        g_Led.Group.First = DRV_LED_RED;
        g_Led.Group.Second= DRV_LED_BLUE;
        g_Led.Group.Third = g_Led.Group.First;
        g_Led.Group.Fourth= g_Led.Group.Second;
    }
#endif
		g_Led.Coefficient = 10;//临时写死亮度，眼睛受不鸟了
    DrvLedTailSet(g_Led.Mode, g_Led.Group);
}




/*********END OF FILE****/

