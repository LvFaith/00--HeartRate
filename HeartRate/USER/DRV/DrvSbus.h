/*
******************************************************************************
*@�ļ�����  : DrvSbus.h
*@����	    : futabaң����sbus�˿ڴ�����ֲ���Ǵ���
******************************************************************************
*@����		  : ��־��
*@ǿ���Թ涨: 
*@�汾		  : V1.00      2016/5/3
******************************************************************************
*/


#ifndef _DRV_SBUS_H_
#define _DRV_SBUS_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include "BspUart.h"

extern void DrvSbusInit(void);
extern void SbusSetRemoteControlData(u16 *channel, u32* timestamp);

extern u32 GetSbusTimestamp( void );

#ifdef __cplusplus
}
#endif

#endif

/*********END OF FILE****/


