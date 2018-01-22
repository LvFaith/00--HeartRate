/*
******************************************************************************
*@文件名字  : DrvMax.h
*@描述	    : MAX30102读取
******************************************************************************
*@作者		: QUANHUI.LV
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


#define DRV_MAX_ADDR     0x57 

/*********************MAX30102寄存器列表*****************************/

#define REG_INTR_STATUS_1 0x00

#define REG_INTR_STATUS_2 0x01

#define REG_INTR_ENABLE_1 0x02

#define REG_INTR_ENABLE_2 0x03

#define REG_FIFO_WR_PTR 0x04

#define REG_OVF_COUNTER 0x05

#define REG_FIFO_RD_PTR 0x06

#define REG_FIFO_DATA 0x07

#define REG_FIFO_CONFIG 0x08

#define REG_MODE_CONFIG 0x09

#define REG_SPO2_CONFIG 0x0A

#define REG_LED1_PA 0x0C

#define REG_LED2_PA 0x0D

#define REG_PILOT_PA 0x10

#define REG_MULTI_LED_CTRL1 0x11

#define REG_MULTI_LED_CTRL2 0x12

#define REG_TEMP_INTR 0x1F

#define REG_TEMP_FRAC 0x20

#define REG_TEMP_CONFIG 0x21

#define REG_PROX_INT_THRESH 0x30

#define REG_REV_ID 0xFE

#define REG_PART_ID 0xFF



extern u8 DrvBaroMaxInit(void);
extern u8 DrvBaroMaxReadData(BSP_I2C_MASTER_PORT_ENUM port, u8 reg, u8 *rxdata, u8 num);
extern u8 DrvDrvMaxWriteCmd(BSP_I2C_MASTER_PORT_ENUM port, u8 reg, u8 data);
extern u8 DrvBaroReadTemp(u8* Temperature);
extern void DrvMaxFifoReadBytes(u8* Data);


#ifdef __cplusplus
}
#endif

#endif

