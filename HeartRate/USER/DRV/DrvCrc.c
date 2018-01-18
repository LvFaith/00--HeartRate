
/*============================ INCLUDES ======================================*/
#include "DrvCrc.h"
#include <stdint.h>
#include "crc.h"
#include "CMSIS_OS.h"
#include <string.h>

/*============================ MACROS ========================================*/

/*============================ TYPES =========================================*/

/*============================ GLOBAL VARIABLES ==============================*/

/*============================ IMPLEMENTATION ================================*/

/*============================ INCLUDES ======================================*/

/*============================ IMPLEMENTATION ================================*/
#define _STM1

uint32_t GetCrcCheck(uint8_t const *buf, uint32_t len)//线程安全函数，中断里不要用
{
		uint32_t temp = 0;
		uint32_t returnCrc = 0xFFFFFFFF;
		uint8_t const *ptr = buf;
		uint8_t i = 0;
		
#ifdef _STM1

		extern osMutexId CrcMutexHandle;

		osMutexWait( CrcMutexHandle, osWaitForever );
		
		CRC->CR = 1;
		while(len >= 4)
		{
				memcpy( &temp, ptr, 4 );
				CRC->DR = temp;
				len -= 4;
				ptr += 4;
		}
		if(len > 0)
		{
				temp = 0;
				for(i = 0; i < len; i++)
				{
						temp |= (*ptr++) << (8 * i);
				}
				CRC->DR = temp;
		}

		returnCrc = CRC->DR;
		
		osMutexRelease(CrcMutexHandle);
		
		return returnCrc;
		
#else
		const uint32_t xCode = 0x04C11DB7;
		uint32_t xbit = 0x80000000; 	//1 << 31
		returnCrc = 0xFFFFFFFF;
		uint32_t cnt = 0;
		while(len >= 4)
		{
				xbit = 0x80000000;
				memcpy( &temp, ptr, 4 );
				cnt++;
				if(cnt == 0x4000)
				{
						memcpy( &temp, ptr, 4 );
				}
				for(int bit = 0; bit < 32; bit++)
				{
						if((returnCrc & 0x80000000) != 0)
						{
								returnCrc <<= 1;
								returnCrc ^= xCode;
						}
						else
						{
								returnCrc <<= 1;
						}
						if((temp & xbit) != 0)
						{
								returnCrc ^= xCode;
						}
						xbit >>= 1;
				}
				ptr += 4;
				len -= 4;
		}
		if(len > 0)
		{
				uint8_t *pT = (uint8_t *) ptr;
				temp = 0;
				for(i = 0; i < len; i++)
				{
						temp |= (*pT++) << (8 * i);
				}
				xbit = 0x80000000;
				for(int bit = 0; bit < 32; bit++)
				{
						if((returnCrc & 0x80000000) != 0)
						{
								returnCrc <<= 1;
								returnCrc ^= xCode;
						}
						else
						{
								returnCrc <<= 1;
						}
						if((temp & xbit) != 0)
						{
								returnCrc ^= xCode;
						}
						xbit >>= 1;
				}
		}
		return returnCrc;
#endif
}




