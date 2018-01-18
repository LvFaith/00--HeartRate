#ifndef _DRV_MEMORY_H_
#define _DRV_MEMORY_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include "BspI2CSoftDriver.h"


typedef enum
{
	DRV_MEM_ERR = 0,
	DRV_MEM_OK,
}DRV_MEM_STATUS_ENUM;

extern DRV_MEM_STATUS_ENUM DrvMemRead(u16 Addr, u16 Size, u8* Buf);
extern DRV_MEM_STATUS_ENUM DrvMemWrite(u16 Addr, u16 Size, u8* Buf);
extern DRV_MEM_STATUS_ENUM DrvMemClear(u32 sector);

#ifdef __cplusplus
}
#endif

#endif

