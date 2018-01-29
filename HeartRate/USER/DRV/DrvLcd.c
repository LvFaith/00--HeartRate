/******************************************************************************

*��Ҫ˵����
��.h�ļ��У�#define Immediatelyʱ��������ʾ��ǰ����
�����#define Delay����ֻ����ִ����LCD_WR_REG(0x0007,0x0173);
֮��Ż���ʾ��ִ��һ��LCD_WR_REG(0x0007,0x0173)������д����
�ݶ�������ʾ��
#define Delayһ�����ڿ����������ʾ����ֹ��ʾ��ȫ��ͼ���ˢ�¹���
******************************************************************************/
#include "DrvLcd.h"
#include <stdio.h>
#include "FONT.h"
#include "GB1616.h"	//16*16������ģ
//#include "sheep.h"
#include "spi.h"
/*
 * ��������LCD_GPIO_Config
 * ����  ������FSMC����LCD��I/O
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����        
 */
// void LCD_GPIO_Config(void)
//{
//    GPIO_InitTypeDef GPIO_InitStructure;
//    
//    /* Enable the FSMC Clock */
//    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
//    
//    /* config lcd gpio clock base on FSMC */
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE);
//    
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    
//    /* config tft back_light gpio base on the PT4101 */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;		
//    GPIO_Init(GPIOD, &GPIO_InitStructure);
//    
//    /* config tft rst gpio */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_13 ; 	 
//    GPIO_Init(GPIOC, &GPIO_InitStructure);  		   
//    
//    /* config tft data lines base on FSMC
//	 * data lines,FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10
//	 */	
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//    
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | 
//                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
//    GPIO_Init(GPIOD, &GPIO_InitStructure);
//    
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
//                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
//                                  GPIO_Pin_15;
//    GPIO_Init(GPIOE, &GPIO_InitStructure); 
//    
//    /* config tft control lines base on FSMC
//	 * PD4-FSMC_NOE  :LCD-RD
//   * PD5-FSMC_NWE  :LCD-WR
//	 * PD7-FSMC_NE1  :LCD-CS
//   * PD11-FSMC_A16 :LCD-DC
//	 */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
//    GPIO_Init(GPIOD, &GPIO_InitStructure);
//    
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
//    GPIO_Init(GPIOD, &GPIO_InitStructure);
//    
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
//    GPIO_Init(GPIOD, &GPIO_InitStructure);  
//    
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 
//    GPIO_Init(GPIOD, &GPIO_InitStructure);  
//    
//    /* tft control gpio init */	 
//    GPIO_SetBits(GPIOD, GPIO_Pin_13);		 // RST = 1 	
//    GPIO_SetBits(GPIOC, GPIO_Pin_5);	 	 //	LIGHT
//    GPIO_SetBits(GPIOD, GPIO_Pin_4);		 // RD = 1  
//    GPIO_SetBits(GPIOD, GPIO_Pin_5);		 // WR = 1 
//    GPIO_SetBits(GPIOD, GPIO_Pin_7);		 //	CS = 1 
//		
//																	//   BL      ����RS      CS
//  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;	
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//	GPIO_ResetBits(GPIOA, GPIO_Pin_4);

//	GPIO_SetBits(GPIOC, GPIO_Pin_13);	 	 //	CS2
//	
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7 | GPIO_Pin_5|GPIO_Pin_11;	
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOD, &GPIO_InitStructure);
//	GPIO_ResetBits(GPIOD, GPIO_Pin_7);
//}

static void Delay(__IO u32 nCount)
{	
	volatile int i;
	for(i=0;i<720;i++)
    for(; nCount != 0; nCount--);
}  
  
void WriteComm(u8 CMD)
{			
  u8 rx_data_buf;
	LCD_RS(0);
//	HAL_SPI_Transmit(&hspi2, (u8*)&CMD, 2,1000);
//  HAL_SPI_Transmit_DMA(&hspi2, &CMD, 1);
  HAL_SPI_TransmitReceive_DMA(&hspi2, &CMD, &rx_data_buf, 1);
}
void WriteData(u8 tem_data)
{			
  u8 rx_data_buf;
	LCD_RS(1);
//  HAL_SPI_Transmit(&hspi2, (u8*)&tem_data, 1,1000);
//  HAL_SPI_Transmit_DMA(&hspi2, (u8*)&tem_data, 1);
  HAL_SPI_TransmitReceive_DMA(&hspi2, &tem_data, &rx_data_buf, 1);
}

//void WriteComm(u16 CMD)
//{			
//	LCD_RS(0);
//  hspi2.Instance->DR = CMD;
//}
//void WriteData(u16 tem_data)
//{			
//	LCD_RS(1);
//  hspi2.Instance->DR = tem_data;
//}

/**********************************************
Lcd��ʼ������
Initial condition  (DB0-15,RS,CSB,WRD,RDB,RESETB="L") 
***********************************************/
u8 lcd_data_receive[10];
void DrvLcdInit(void)
{	
	int a,i;
  
// LCD_FSMC_Config();
// LCD_Rst();
  
#if 1
SPI_CS(1);
// 
	
		////////////////////HX8357C+TM3.2////////////
WriteComm(0x0001);
Delay(100);
SPI_CS(0);
WriteComm(0x0011);
Delay(20);

//************* Start Initial Sequence **********//
WriteComm(0xC0);
WriteData(0x0A); // P-Gamma level
WriteData(0x0A); // N-Gamma level
WriteComm(0xC1); // BT & VC Setting
WriteData(0x41);
WriteData(0x07); // VCI1 = 2.5V
WriteComm(0xC2); // DC1.DC0 Setting
WriteData(0x33);
WriteComm(0xC5);
WriteData(0x00);
WriteData(0x42); // VCM Setting
WriteData(0x80); // VCM Register Enable
WriteComm(0xB1);
WriteData(0xB0); // Frame Rate Setting
WriteData(0x11);
WriteComm(0xB4);
WriteData(0x00); // Frame Rate Setting
WriteComm(0xB6); // RM.DM Setting
WriteData(0x00);
WriteData(0x02);
WriteData(0x3B);
WriteComm(0xB7); // Entry Mode
WriteData(0x07);
WriteComm(0xF0); // Enter ENG , must be set before gamma setting
WriteData(0x36);
WriteData(0xA5);
WriteData(0xD3);
WriteComm(0xE5); // Open gamma function , must be set before gamma setting
WriteData(0x80);
WriteComm(0xE5); // Page 1
WriteData(0x01);
WriteComm(0XB3); // WEMODE=0(Page 1) , pixels over window setting will be ignored.
WriteData(0x00);
WriteComm(0xE5); // Page 0
WriteData(0x00);
WriteComm(0xF0); // Exit ENG , must be set before gamma setting
WriteData(0x36);
WriteData(0xA5);
WriteData(0x53);
WriteComm(0xE0); // Gamma setting
WriteData(0x00);
WriteData(0x35);
WriteData(0x33);
WriteData(0x00);
WriteData(0x00);
WriteData(0x00);
WriteData(0x00);
WriteData(0x35);
WriteData(0x33);
WriteData(0x00);
WriteData(0x00);
WriteData(0x00);
WriteComm(0x36); // Color filter setting
WriteData(0x08);
WriteComm(0xEE);
WriteData(0x00);
WriteComm(0x3A); // interface setting
WriteData(0x55);
WriteComm(0x11); // Exit sleep mode
WriteComm(0x20);
WriteComm(0x29); // Display on 

Delay(10);


WriteComm(0x36); //Set_address_mode
WriteData(0x68); //�����������½ǿ�ʼ�������ң����µ���
#endif

#if 0
SPI_CS(1);

WriteComm(0x01);
Delay(100);
SPI_CS(0);

WriteComm(0x11); 	//t ver address
Delay(20);	


//LQH TEST
//u8 reg_test = 0x0C;
//Delay(10);
//HAL_SPI_TransmitReceive_DMA(&hspi2, &reg_test, lcd_data_receive, 2);
//Delay(10);

WriteComm(0xc0); 	
WriteData(0x0A); // VRH1 01~1F
WriteData(0x0A); // VRH2 01~1F

WriteComm(0xC1); 	//BT AND VC	
WriteData(0x41);
WriteData(0x07);

WriteComm(0xC5); 	//VCM
WriteData(0x00);
WriteData(0x42);	   //3D
WriteData(0x80);

WriteComm(0xC2); 	//DC1,DC0	
WriteData(0x33);

WriteComm(0x35); 	//TE 	
WriteData(0x00);

WriteComm(0xB1); // frmae rate	
WriteData(0xB0);    // 70hz // c0 81hz // d0 96hz
WriteData(0x11);

WriteComm(0xB4); // display inversion
WriteData(0x02);  // 0 column 1 1dot  2 2dot

WriteComm(0xB6); 	//t ver address	
WriteData(0x00);
WriteData(0x00);//02  GS SS SM
WriteData(0x3B);

WriteComm(0xB7); 	//t ver address	
WriteData(0x07);

WriteComm(0xF0); 	//Enter ENG	
WriteData(0x36);
WriteData(0xA5);
WriteData(0xD3);

WriteComm(0xE5); 	//Open gamma function	
WriteData(0x80);

WriteComm(0xF0); 	//	Exit ENG	
WriteData(0x36);
WriteData(0xA5);
WriteData(0x53);

WriteComm(0xE0); 	//Gamma setting	
WriteData(0x00);
WriteData(0x35);
WriteData(0x33);
WriteData(0x00);
WriteData(0x00);
WriteData(0x00);
WriteData(0x00);
WriteData(0x35);
WriteData(0x33);
WriteData(0x00);
WriteData(0x00);
WriteData(0x00);

WriteComm(0x3a); 	//t ver address	
WriteData(0x55);//

WriteComm(0x29); 	//t ver address
Delay(10);

WriteComm(0x36); //Set_address_mode
WriteData(0x68); //�����������½ǿ�ʼ�������ң����µ���
#endif



////Lcd_Light_ON;//�����ƹ�û�з���IO********************************************************************************************************************************

// while(1)
//{
WriteComm(0x36);//����
WriteData(0x48);//

//test_color();

BlockWrite(0,319,0,479);
for(i=0;i<320*480;i++)
{
	LCD_RS(1);
	WriteData(BLUE>>8);
	LCD_RS(1);
	WriteData(BLUE);
}
WriteComm(0x36); //Set_address_mode
WriteData(0x68); //�����������½ǿ�ʼ�������ң����µ���
LCD_PutString(10, 10, "֧������jfkalgjlasghkjlagjkaslhuig", BRED, BLACK,1); 

}

void test_color(void)
{
	unsigned short i,j;
	BlockWrite(0,319,0,479);
	for(i=0;i<480;i++)
	{	for(j=0;j<320;j++)
		{
			if(j<40)
			{
				WriteData(BLACK>>8);
				WriteData(BLACK); 
			}
			else if(j<80)
			{
				WriteData(BLUE>>8);
				WriteData(BLUE);
			}
			else if(j<120)
			{
				WriteData(BRED>>8);
				WriteData(BRED);
			}
			else if(j<160)
			{
				WriteData(GRED>>8);
				WriteData(GRED);
			}
			else if(j<200)
			{
				WriteData(RED>>8);
				WriteData(RED);
			}
			else if(j<240)
			{
				WriteData(GREEN>>8);
				WriteData(GREEN);
			}
			else if(j<280)
			{
				WriteData(YELLOW>>8);
				WriteData(YELLOW);
			}
			else if(j<320)
			{
				WriteData(BROWN>>8);
				WriteData(BROWN);
			}
		}
	}
}

/**********************************************
��������Lcd��ѡ����
���ܣ�ѡ��Lcd��ָ���ľ�������

ע�⣺xStart��yStart��Xend��Yend������Ļ����ת���ı䣬λ���Ǿ��ο���ĸ���

��ڲ�����xStart x�������ʼ��
          ySrart y�������ʼ��
          Xend   y�������ֹ��
          Yend   y�������ֹ��
����ֵ����
***********************************************/
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend) 
{
	//HX8357-C
	
	 WriteComm(0x2A);
   WriteData(Xstart>>8);
   WriteData(Xstart);
   WriteData(Xend>>8);
   WriteData(Xend);
  
   WriteComm(0x2B);
   WriteData(Ystart>>8);
   WriteData(Ystart);
   WriteData(Yend>>8);
	 WriteData(Yend);
	 
	 WriteComm(0x2C);
}
/**********************************************
��������Lcd��ѡ����
���ܣ�ѡ��Lcd��ָ���ľ�������

ע�⣺xStart�� yStart������Ļ����ת���ı䣬λ���Ǿ��ο���ĸ���

��ڲ�����xStart x�������ʼ��
          ySrart y�������ֹ��
          xLong Ҫѡ�����ε�x���򳤶�
          yLong  Ҫѡ�����ε�y���򳤶�
����ֵ����
***********************************************/
void Lcd_ColorBox(u16 xStart,u16 yStart,u16 xLong,u16 yLong,u16 Color)
{
	u32 temp;

	BlockWrite(xStart,xStart+xLong-1,yStart,yStart+yLong-1);
	for (temp=0; temp<xLong*yLong; temp++)
	{
		WriteData(Color>>8);
		WriteData(Color);
	}
}

/******************************************
��������Lcdͼ�����100*100
���ܣ���Lcdָ��λ�����ͼ��
��ڲ�����Index ҪѰַ�ļĴ�����ַ
          ConfigTemp д������ݻ�����ֵ
******************************************/
void LCD_Fill_Pic(u16 x, u16 y,u16 pic_H, u16 pic_V, const unsigned char* pic)
{
  unsigned long i;
	unsigned int j;

// 	WriteComm(0x36); //Set_address_mode
// 	WriteData(0x0a); //�����������½ǿ�ʼ�������ң����µ���
	BlockWrite(x,x+pic_H-1,y,y+pic_V-1);
	
	for (i = 0; i < pic_H*pic_V*2; i++)
	{
// 		WriteData(pic[i]);
	LCD_RS(1);	
	WriteData(pic[i]);
	}
// 	WriteComm(0x36); //Set_address_mode
// 	WriteData(0xaa);
}

void DrawPixel(u16 x, u16 y, u16 Color)
{
	BlockWrite(x,x,y,y);
// 	WriteComm(0x200);   
// 	WriteData(x);
// 	WriteComm(0x201);   
// 	WriteData(y);
// 	WriteComm(0x202);
	WriteData(Color>>8);
	WriteData(Color);
// 	*(__IO u16 *) (Bank1_LCD_D) = Color;
}

void PutGB1616(unsigned short x, unsigned short  y, unsigned char c[2], unsigned int fColor,unsigned int bColor,unsigned char flag)
{
	unsigned char i,j,k,m;
	for (k=0;k<64;k++) { //64��ʾ�Խ����ֿ��еĸ�����ѭ����ѯ����
	  if ((codeGB_16[k].Index[0]==c[0])&&(codeGB_16[k].Index[1]==c[1]))
			{ 
    	for(i=0;i<32;i++) 
			{
				m=codeGB_16[k].Msk[i];
				for(j=0;j<8;j++) 
				{		
					if((m&0x80)==0x80) {
						DrawPixel(x+j,y,fColor);
						}
					else {
						if(flag) DrawPixel(x+j,y,bColor);
						}
					m=m<<1;
				} 
				if(i%2){y++;x=x-8;}
				else x=x+8;
		  }
		}  
	  }	
	}
void LCD_PutString(unsigned short x, unsigned short y, unsigned char *s, unsigned int fColor, unsigned int bColor,unsigned char flag) 
	{
	 unsigned char l=0;
	while(*s) {
		PutGB1616(x+l*8,y,(unsigned char*)s,fColor,bColor,flag);
		s+=2;l+=2;
// 		if( *s < 0x80) 
// 		    {
// 			LCD_PutChar(x+l*8,y,*s,fColor,bColor,flag);
// 			s++;l++;
// 			}
// 		else
// 		    {
// 			PutGB1616(x+l*8,y,(unsigned char*)s,fColor,bColor,flag);
// 			s+=2;l+=2;
// 			}
		}
	}
typedef __packed struct
{
	u8  pic_head[2];				//1
	u16 pic_size_l;			    //2
	u16 pic_size_h;			    //3
	u16 pic_nc1;				    //4
	u16 pic_nc2;				    //5
	u16 pic_data_address_l;	    //6
	u16 pic_data_address_h;		//7	
	u16 pic_message_head_len_l;	//8
	u16 pic_message_head_len_h;	//9
	u16 pic_w_l;					//10
	u16 pic_w_h;				    //11
	u16 pic_h_l;				    //12
	u16 pic_h_h;				    //13	
	u16 pic_bit;				    //14
	u16 pic_dip;				    //15
	u16 pic_zip_l;			    //16
	u16 pic_zip_h;			    //17
	u16 pic_data_size_l;		    //18
	u16 pic_data_size_h;		    //19
	u16 pic_dipx_l;			    //20
	u16 pic_dipx_h;			    //21	
	u16 pic_dipy_l;			    //22
	u16 pic_dipy_h;			    //23
	u16 pic_color_index_l;	    //24
	u16 pic_color_index_h;	    //25
	u16 pic_other_l;			    //26
	u16 pic_other_h;			    //27
	u16 pic_color_p01;		    //28
	u16 pic_color_p02;		    //29
	u16 pic_color_p03;		    //30
	u16 pic_color_p04;		    //31
	u16 pic_color_p05;		    //32
	u16 pic_color_p06;		    //33
	u16 pic_color_p07;		    //34
	u16 pic_color_p08;			//35			
}BMP_HEAD;

BMP_HEAD bmp;

 
#if 0

//��������С��Χ����ʾͼƬ
/*
x��y�����������
*/
char display_picture(char *filename)
{
	u16 ReadValue;
	FATFS fs;            // Work area (file system object) for logical drive
	FIL fsrc;      			// file objects
	u8 buffer[2048]; 		// file copy buffer
	FRESULT res;         // FatFs function common result code
	UINT br;         		// File R/W count
	u16 r_data,g_data,b_data;	
	u32	 tx,ty,temp;
	
	
  f_mount(0, &fs);

  res = f_open(&fsrc, filename, FA_OPEN_EXISTING | FA_READ);	 //����ͼƬ�ļ���
  if(res==FR_NO_FILE||res==FR_INVALID_NAME){
     f_mount(0, NULL);
	 return 0;
  }

  if(res!=FR_OK){
     f_mount(0, NULL);
	 SD_Init();//���³�ʼ��SD�� 
	 return 0;
  }

  res = f_read(&fsrc, &bmp, sizeof(bmp), &br);

  if(br!=sizeof(bmp))
	{
		f_close(&fsrc);
		f_mount(0, NULL);
		return 0;
  }

  if((bmp.pic_head[0]=='B')&&(bmp.pic_head[1]=='M'))
  {
	res = f_lseek(&fsrc, ((bmp.pic_data_address_h<<16)|bmp.pic_data_address_l));
	if(res!=FR_OK){
     f_close(&fsrc);
     f_mount(0, NULL);
	 return 0;
    }
	
	temp=bmp.pic_w_l*3;	
	if(bmp.pic_w_l>bmp.pic_h_l)
	{
 	WriteComm(0x36); //Set_address_mode
 	WriteData(0x38); //�����������½ǿ�ʼ�������ң����µ���
	if(bmp.pic_w_l<480||bmp.pic_h_l<320)
		{
 			Lcd_ColorBox(0,0,480,320,0x0000);
 			BlockWrite((480-bmp.pic_w_l)/2,(480-bmp.pic_w_l)/2+bmp.pic_w_l-1,(320-bmp.pic_h_l)/2,(320-bmp.pic_h_l)/2+bmp.pic_h_l-1);
		}
		else 	BlockWrite(0,479,0,319);
	}
	else
	{
		WriteComm(0x36); //Set_address_mode
		WriteData(0x08); //�����������½ǿ�ʼ�������ң����µ���
		if(bmp.pic_w_l<320||bmp.pic_h_l<480)
			{
				Lcd_ColorBox(0,0,320,480,0x0000);
				BlockWrite((320-bmp.pic_w_l)/2,(320-bmp.pic_w_l)/2+bmp.pic_w_l-1,(480-bmp.pic_h_l)/2,(480-bmp.pic_h_l)/2+bmp.pic_h_l-1);
			}
			else BlockWrite(0,319,0,479);	
	}
	for (tx = 0; tx < bmp.pic_h_l; tx++)
	{
	f_read(&fsrc, buffer, (bmp.pic_w_l)*3, &br);
	for(ty=0;ty<temp;ty+=3)
		{
			r_data = *(ty +2+buffer);
			g_data = *(ty +1+buffer);
			b_data = *(ty +  buffer);			
			ReadValue=(r_data & 0xF8) << 8 | (g_data & 0xFC) << 3 | b_data >> 3;	
			WriteData(ReadValue>>8);
			WriteData(ReadValue);
			
// 			WriteData(r_data);
// 			WriteData(g_data);
// 			WriteData(b_data);
// 			*(__IO u16 *) (Bank1_LCD_D) = ((*(ty +2+buffer)<<8))|(*(ty +1+buffer)); 
// 			*(__IO u16 *) (Bank1_LCD_D) = ((*(ty +0+buffer)<<8))|(*(ty +5+buffer));  
// 			*(__IO u16 *) (Bank1_LCD_D) = ((*(ty +4+buffer)<<8))|(*(ty +3+buffer)); 
		}
	}	
		f_close(&fsrc);
   }
  f_mount(0, NULL);
// 	WriteComm(0x36); //Set_address_mode
//  	WriteData(0x08); //�����������Ͻǿ�ʼ�������ң����ϵ���
  return 1;
}
#endif

