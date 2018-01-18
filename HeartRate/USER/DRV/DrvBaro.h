/*
******************************************************************************
*@文件名字  : DrvBaro.h
*@描述	    : 气压计读取
******************************************************************************
*@作者		: 丁志铖
*@强制性规定: 移植时只可以在 USER CODE BEGIN  USER CODE END  之间删改代码!!!
*@版本		: V1.00      2016/3/25
******************************************************************************
*/


#ifndef _DRV_BAROMETRIC_H_
#define _DRV_BAROMETRIC_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include "config.h"
#include "BspI2CSoftDriver.h"


#define DRV_MS5611_ADDR     0x76 //#define  RDCMD   0xED  // read  cmd  //#define  WRCMD   0xEC  // write cmd

/*********************5883L寄存器列表*****************************/

#define  DRV_MS5611_RESET   0x1E 

#define  D1C256     0x40
#define  D1C512     0x42
#define  D1C1024    0x44
#define  D1C2048    0x46
#define  D1C4096    0x48

#define  D2C256     0x50
#define  D2C512     0x52
#define  D2C1024    0x54
#define  D2C2048    0x56
#define  D2C4096    0x58

#define  ADCREAD    0x00

#define  C0ADDR     0xA0 //0b1010 0000
#define  C1ADDR     0xA2 //0b1010 0010
#define  C2ADDR     0xA4 //0b1010 0100
#define  C3ADDR     0xA6 //0b1010 0110
#define  C4ADDR     0xA8 //0b1010 1000
#define  C5ADDR     0xAA //0b1010 1010
#define  C6ADDR     0xAC //0b1010 1100
#define  C7ADDR     0xAE //0b1010 1110

#define BARO_NUM  (3)

extern double sumspip_use[BARO_NUM];

extern u8 DrvBaroInit(BSP_I2C_MASTER_PORT_ENUM port);
extern u8 DrvBaroCheck(BSP_I2C_MASTER_PORT_ENUM port);
extern u8 DrvBaroRead(BSP_I2C_MASTER_PORT_ENUM port,s16 *t);



#ifdef __cplusplus
}
#endif

#endif

