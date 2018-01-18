/*
******************************************************************************
*@�ļ�����  : DrvLed.c
*@����	    : Led������
******************************************************************************
*@����		: ��־��
*@ǿ���Թ涨: ��ֲʱֻ������ USER CODE BEGIN  USER CODE END  ֮��ɾ�Ĵ���!!!
*@�汾		: V1.00      2016/3/29
******************************************************************************
*/

#include "DrvLed.h"
#include "gpio.h"

DRV_LED_STRUCT g_Led;
//u8 test[102400] = {0};


/******************************************************************************
* @�� �� ���� DrvLedInit
* @���������� ��ʼ��LED������ݣ�����ע���
* @��    ���� 
* @�� �� ֵ��
* @��    ע��
******************************************************************************/ 
void DrvLedInit(void)
{
  memset(&g_Led, 0, sizeof(g_Led));
  g_Led.Coefficient = 1; //��ֹ�Ʋ���������û�ã���Ϊ������PWM����
}

static void DrvLedConfigSet(int TailRed,int TailGreen ,int TailBlue)
{
  //ԭ��ͼ�����ˣ�RED��GREEN����
  HAL_GPIO_WritePin(LED_RED_GPIO_OUTPUT_GPIO_Port,   LED_RED_GPIO_OUTPUT_Pin,   TailRed   ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_GREEN_GPIO_OUTPUT_GPIO_Port, LED_GREEN_GPIO_OUTPUT_Pin, TailGreen ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_BLUE_GPIO_OUTPUT_GPIO_Port,  LED_BLUE_GPIO_OUTPUT_Pin,  TailBlue  ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/******************************************************************************
* @�� �� ���� DrvLedSetColor
* @���������� ����β����ɫ
* @��    ���� 
* @�� �� ֵ��
* @��    ע��
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
        case DRV_LED_OFF:                       //��
            Green   =      0 * g_Led.Coefficient;  
            Blue    =      0 * g_Led.Coefficient;  
            Red     =      0 * g_Led.Coefficient;
            break; 
        
        case DRV_LED_RED:                       //��    
            Green   =      0 * g_Led.Coefficient;  
            Blue    =      0 * g_Led.Coefficient;
            Red     =     25 * g_Led.Coefficient;
            break; 
        
        case DRV_LED_BLUE:                      //��
            Green   =      0 * g_Led.Coefficient;  
            Blue    =     25 * g_Led.Coefficient;  
            Red     =      0 * g_Led.Coefficient;
            break;
        
        case DRV_LED_PURPLE:                    //��
            Green   =      0 * g_Led.Coefficient;  
            Blue    =     25 * g_Led.Coefficient;  
            Red     =     25 * g_Led.Coefficient;
            break;
        
        case DRV_LED_GREEN:                     //��
            Green   =     20 * g_Led.Coefficient; //2500*0.8 
            Blue    =      0 * g_Led.Coefficient; 
            Red     =      0 * g_Led.Coefficient;
            break;
        
        case DRV_LED_YELLOW:                    //��
            Green   =     15 * g_Led.Coefficient; //2500*0.6 
            Blue    =      0 * g_Led.Coefficient;  
            Red     =     25 * g_Led.Coefficient;
            break;
        
        case DRV_LED_LIGHT_BLUE:                //ǳ��
            Green   =     20 * g_Led.Coefficient; //2500*0.8  
            Blue    =     25 * g_Led.Coefficient;  
            Red     =      0 * g_Led.Coefficient;
            break;
        
        case DRV_LED_WHITE:                     //��
            Green   =    25 * g_Led.Coefficient;  
            Blue    =    25 * g_Led.Coefficient;  
            Red     =    25 * g_Led.Coefficient;
            break;
        
        default:                            //��
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
		g_Led.Coefficient = 10;//��ʱд�����ȣ��۾��ܲ�����
    DrvLedTailSet(g_Led.Mode, g_Led.Group);
}




/*********END OF FILE****/

