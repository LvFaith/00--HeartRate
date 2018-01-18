/*
******************************************************************************
*@文件名字  : DrvRotateSpeed.h
*@描述	    : 缸温传感器驱动
*       
******************************************************************************
*@作者		: 张文君
*@强制性规定: 
*@版本		: 
*             
******************************************************************************
*/

#ifndef DRV_ROTATE_SPEED_H
#define DRV_ROTATE_SPEED_H


#ifdef __cplusplus
 extern "C" {
#endif 
   
#include "config.h"
   
extern uint32_t  RotateSpeed[2];
   
extern void RotateSpeedInit(void);
   
#ifdef __cplusplus
}
#endif
 

#endif





