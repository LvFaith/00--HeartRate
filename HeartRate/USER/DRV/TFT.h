#ifndef _TFT_H_
#define _TFT_H_

#define LCD_R *(volatile u16 *)((uint32_t)0x6C000000) //向fsmc总线资写入地址
#define LCD_D *(volatile u16 *)((uint32_t)0x6C000800) //向fsmc总线写入数据

#define HANZIGESHU 31

#define Write_Data_U16(y) LCD_D = y	   //向tft写入十六位数据
#define Write_Data(x) LCD_D = x			//同上
#define Write_Cmd(LCD_Reg) LCD_R= LCD_Reg  //向tft写入命令

#define WINDOW_XADDR_START	0x0050 // Horizontal Start Address Set
#define WINDOW_XADDR_END	0x0051 // Horizontal End Address Set
#define WINDOW_YADDR_START	0x0052 // Vertical Start Address Set
#define WINDOW_YADDR_END	0x0053 // Vertical End Address Set
#define GRAM_XADDR		    0x0020 // GRAM Horizontal Address Set
#define GRAM_YADDR		    0x0021 // GRAM Vertical Address Set
#define GRAMWR 			    0x0022 // memory write

#define White          0xFFFF
#define Black          0x0000
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#define Bred         	0xfc07
#define LGRAY          0xc618
#define Brown          0xbc40
#define Blue3          0x7d7c

#define BColor Blue2
#define FColor Red

#define RESET(x) x ? GPIO_SetBits(GPIOB,GPIO_Pin_0) : GPIO_ResetBits(GPIOB,GPIO_Pin_0)

//声明函数
void ILI9325_Initial(void);	  //液晶初始化函数
void delayms(unsigned int count);	 //不精确延时1ms
void Write_Cmd_Data (unsigned int x,unsigned int y);			  //液晶的写16位数据
void LCD_SetPos(unsigned int x0,unsigned int x1,unsigned int y0,unsigned int y1);  //设置一个现实窗口
void ClearScreen(unsigned int bColor);		//清屏
void LCD_PutChar8x16(unsigned short x, unsigned short y, char c, unsigned int fColor, unsigned int bColor);	 //显示一个英文字符
void LCD_PutChar(unsigned short x, unsigned short y, char c, unsigned int fColor, unsigned int bColor);		 //同上
void LCD_PutString(unsigned short x, unsigned short y, unsigned char *s, unsigned int fColor, unsigned int bColor);	  //显示一个字符串
void PutGB1616(unsigned short x, unsigned short  y, unsigned char c[2], unsigned int fColor,unsigned int bColor);	  //现实一个汉字
void Line(unsigned int X0,unsigned int Y0,unsigned int X1,unsigned int Y1,unsigned int color);		 // 画一条线
void Rectangle(unsigned int left,unsigned int top,unsigned int right,unsigned int bottom,unsigned int color);  //画一个空心矩形框
void Put_pixel(unsigned int x,unsigned int y,unsigned int color);	  //画一个像素点儿  通常被其它函数调用
void Fill_Rectangle(uint x,uint y,uint x1,uint y1,uint color);		  //画一个实心矩形
void Rectangle(unsigned int left,unsigned int top,unsigned int right,unsigned int bottom,unsigned int color);
void Show_Touch(unsigned int x,unsigned int y,unsigned int Color);
void showimage(void);
void print_num(uint x,uint y,uint num);

#endif




