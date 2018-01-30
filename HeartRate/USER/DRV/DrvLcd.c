/******************************************************************************

*重要说明！
在.h文件中，#define Immediately时是立即显示当前画面
而如果#define Delay，则只有在执行了LCD_WR_REG(0x0007,0x0173);
之后才会显示，执行一次LCD_WR_REG(0x0007,0x0173)后，所有写入数
据都立即显示。
#define Delay一般用在开机画面的显示，防止显示出全屏图像的刷新过程
******************************************************************************/
#include "DrvLcd.h"
#include <stdio.h>
#include "8x16.h"
#include "GB1616.h"	//16*16汉字字模
#include "spi.h"


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
void Write_Data_U16(unsigned int Color_U16)
{
  WriteData(Color_U16>>8);
	WriteData(Color_U16);
}

void BlockWrite_V(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend) 
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
Lcd初始化函数
Initial condition  (DB0-15,RS,CSB,WRD,RDB,RESETB="L") 
***********************************************/
u8 lcd_data_receive[10];
void DrvLcdInit(void)
{	
#if 1
SPI_CS(1);
////////////////////HX8357C+TM3.2////////////
WriteComm(0x0001);
Delay(20);
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
WriteData(0x68); //横屏，从左下角开始，从左到右，从下到上
#endif

//LQH TEST
//u8 reg_test = 0x0C;
//Delay(10);
//HAL_SPI_TransmitReceive_DMA(&hspi2, &reg_test, lcd_data_receive, 2);
//Delay(10);

WriteComm(0x36);//竖屏
WriteData(0x48);//
InitLcdPic();     //刷屏初始化原始界面
      
WriteComm(0x36); //Set_address_mode
WriteData(0x6A); //横屏，从左下角开始，从左到右，从下到上

LCD_PutString(10, 13, "测量者编号:", FONT_SURFACE_COLOR, FONT_BKG_COLOR,1);
print_num(90,13,01);
LCD_PutString(157, 13, "心率:", FONT_SURFACE_COLOR, FONT_BKG_COLOR,1);
print_num(190,13,0);
LCD_PutString(255, 13, "血氧浓度:", FONT_SURFACE_COLOR, FONT_BKG_COLOR,1);
print_num(318,13,0);
LCD_PutString(380, 13, "保存状态:否", FONT_SURFACE_COLOR, FONT_BKG_COLOR,1);

Line(0,39,479,39,LINE_COLOR);   //第一行的线
Line(0,40,479,40,LINE_COLOR);

Line(0,180,479,180,LINE_COLOR);  //中间的线
Line(0,179,479,179,LINE_COLOR);  

Line(39,39,39,319,LINE_COLOR);   //竖线
Line(40,39,40,319,LINE_COLOR);

LCD_PutString(10, 80, "心", FONT_SURFACE_COLOR, FONT_BKG_COLOR,1);//竖向的字
LCD_PutString(10, 110, "率", FONT_SURFACE_COLOR, FONT_BKG_COLOR,1);
LCD_PutString(10, 220, "血", FONT_SURFACE_COLOR, FONT_BKG_COLOR,1);
LCD_PutString(10, 250, "氧", FONT_SURFACE_COLOR, FONT_BKG_COLOR,1);

}

void InitLcdPic(void)
{
	int a,i,j;
  
  BlockWrite_V(0,319,0,479);
  for(i=0;i<480;i++)
  {	
    for(j=0;j<320;j++)
		{
			if(j<40)
			{
				WriteData(WHITE>>8);
				WriteData(WHITE); 
			}
      else
      {
        if(i>440)
        {
            WriteData(WHITE>>8);
            WriteData(WHITE); 
        }
        else
        {
            WriteData(GRAY>>8);
            WriteData(GRAY); 
        }
      }
      
        
    }
  }
}



/**********************************************
函数名：Lcd块选函数
功能：选定Lcd上指定的矩形区域

注意：xStart、yStart、Xend、Yend随着屏幕的旋转而改变，位置是矩形框的四个角

入口参数：xStart x方向的起始点
          ySrart y方向的起始点
          Xend   y方向的终止点
          Yend   y方向的终止点
返回值：无
***********************************************/
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend) 
{
	//HX8357-C
   
   Xstart = 479 - Xstart;
   Xend = 479 - Xend;
  
   Ystart = 319 - Ystart;
   Yend = 319 -Yend;
	
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
函数名：Lcd块选函数
功能：选定Lcd上指定的矩形区域

注意：xStart和 yStart随着屏幕的旋转而改变，位置是矩形框的四个角

入口参数：xStart x方向的起始点
          ySrart y方向的终止点
          xLong 要选定矩形的x方向长度
          yLong  要选定矩形的y方向长度
返回值：无
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
函数名：Lcd图像填充100*100
功能：向Lcd指定位置填充图像
入口参数：Index 要寻址的寄存器地址
          ConfigTemp 写入的数据或命令值
******************************************/
void LCD_Fill_Pic(u16 x, u16 y,u16 pic_H, u16 pic_V, const unsigned char* pic)
{
  unsigned long i;
	unsigned int j;

// 	WriteComm(0x36); //Set_address_mode
// 	WriteData(0x0a); //横屏，从左下角开始，从左到右，从下到上
	BlockWrite(x,x+pic_H-1,y,y+pic_V-1);
	
	for (i = 0; i < pic_H*pic_V*2; i++)
	{
// 		WriteData(pic[i]);
	LCD_RS(1);	
	WriteData(pic[i]);
	}
}

void DrawPixel(u16 x, u16 y, u16 Color)
{
	BlockWrite(x,x,y,y);
	WriteData(Color>>8);
	WriteData(Color);
}


void LCD_Fill_Block(unsigned int X0,unsigned int Y0,unsigned int X1,unsigned int Y1,unsigned int color)
{
	unsigned short i,j;
  if(X0 >479 || Y0>319 || X1 >479 || Y1>319)
    return;
//	BlockWrite(0,479,0,319);
	for(i=0;i<X1-X0;i++)
	{	for(j=0;j<Y1-Y0;j++)
		{
				DrawPixel(X0+i,Y0+j,color);
    }
  }
}


void PutGB1616(unsigned short x, unsigned short  y, unsigned char c[2], unsigned int fColor,unsigned int bColor,unsigned char flag)
{
  BlockWrite(x,x+15,y,y+15);
	unsigned char i,j,k,m;
	for (k=0;k<64;k++) { //64标示自建汉字库中的个数，循环查询内码
	  if ((codeGB_16[k].Index[0]==c[0])&&(codeGB_16[k].Index[1]==c[1]))
			{ 
    	for(i=0;i<32;i++) 
			{
				m=codeGB_16[k].Msk[i];
				for(j=0;j<8;j++) 
				{		
					if((m&0x80)==0x80) {
						DrawPixel(x+j,y,fColor);
      //      Write_Data_U16(fColor);
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
void LCD_PutChar8x16(unsigned short x, unsigned short y, char c, unsigned int fColor, unsigned int bColor)
{
	unsigned char i,j;
	BlockWrite(x,x+7,y,y+15);
	for (i=0;i<16;i++)
	{
      unsigned char m=Font8x16[c*16+i];
      for (j=0;j<8;j++)
      {
        if((m&0x80)==0x80)
        {
          DrawPixel(x+j,y,fColor);
        }
        else 
        {	
          DrawPixel(x+j,y,bColor);
        }
        m<<=1;
      }	
      y++;		
	}
}
void LCD_PutString(unsigned short x, unsigned short y, unsigned char *s, unsigned int fColor, unsigned int bColor,unsigned char flag) 
{
    unsigned char l=0;
    while(*s) 
    {
        if( *s < 0x80) 
        {
          LCD_PutChar8x16(x+l*8,y,*s,fColor,bColor);
          s++;l++;
        }
        else
        {
          PutGB1616(x+l*8,y,(unsigned char*)s,fColor,bColor,flag);
          s+=2;l+=2;
        }
		}
}

void Line(unsigned int X0,unsigned int Y0,unsigned int X1,unsigned int Y1,unsigned int color)
{
	int dx = X1 - X0;
	int dy = Y1 - Y0;
	//int P  = 2 * dy - dx;
	//int dobDy = 2 * dy;
	//int dobD = 2 * (dy - dx);
	int PointX = 0,PointY = 0;
	int incx = 0,incy = 0;
	int distance = 0,xerr = 0,yerr = 0;
	unsigned int i = 0;
	if(dx == 0)
	{
		PointX = X0;
		if(Y0 < Y1)
		{
			PointY = Y0;
		}
		else{
			PointY = Y1;
		}
		for(i = 0;i <= ((Y0<Y1) ? (Y1-Y0) : (Y0-Y1));i++)
		{
			DrawPixel(PointX,PointY,color);
			PointY++;
		}
		return;
	}
	if(dy == 0)	
	{
		PointY = Y0;
		if(X0 < X1)
		{
			PointX = X0;
		}
		else{
			PointX = X1;
		}
		for(i = 0;i <= ((X0<X1) ? (X1-X0) : (X0-X1));i++)
		{
			DrawPixel(PointX,PointY,color);
			PointX++;
		}
		return;
	}

	if(dx > 0)
		incx = 1;
	else if(dx == 0)
		incx = 0;
	else
		incx = -1;

	if(dy > 0)
		incy = 1;
	else if(dy == 0)
		incy = 0;
	else
		incy = -1;

    dx = ((X0>X1) ? (X0-X1) : (X1-X0));
    dy = ((Y0>Y1) ? (Y0-Y1) : (Y1-Y0));

    if(dx>dy) distance=dx;
    else distance=dy;

    PointX = X0;
    PointY = Y0;
    for(i=0;i<=distance+1;i++){
    	DrawPixel(PointX,PointY,color);
        xerr+=dx;
        yerr+=dy;
        if(xerr>distance){
            xerr-=distance;
            PointX+=incx;
        }
        if(yerr>distance){
            yerr-=distance;
            PointY+=incy;
        }
    }
}


void print_num(u16 x,u16 y,u16 num)
{
	LCD_PutChar8x16(x+8,y,num%1000/100+48,Black,White);
	LCD_PutChar8x16(x+16,y,num%100/10+48,Black,White);
	LCD_PutChar8x16(x+24,y,num%10+48,Black,White);
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

//任意屏大小范围内显示图片
/*
x，y像素起点坐标
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

  res = f_open(&fsrc, filename, FA_OPEN_EXISTING | FA_READ);	 //打上图片文件名
  if(res==FR_NO_FILE||res==FR_INVALID_NAME){
     f_mount(0, NULL);
	 return 0;
  }

  if(res!=FR_OK){
     f_mount(0, NULL);
	 SD_Init();//重新初始化SD卡 
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
 	WriteData(0x38); //横屏，从左下角开始，从左到右，从下到上
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
		WriteData(0x08); //竖屏，从左下角开始，从左到右，从下到上
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
//  	WriteData(0x08); //竖屏，从左上角开始，从左到右，从上到下
  return 1;
}
#endif

