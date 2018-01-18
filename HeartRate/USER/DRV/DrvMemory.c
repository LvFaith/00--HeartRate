#include "DrvMemory.h"

#define DRV_MEM_ADDR (0x50) //0xA0>>1

#define DRV_MEM_SECTOR (128)

#define WRITE_NEED_TIME (7)//手册写的是5ms。操作系统下，osDelay(7) 会延时的ms数是 (6,8)开区间

/*********************存储偏移列表*****************************/
#define  DRV_MEM_START     (0x00)  //


/******************************************************************************
* @函 数 名： DrvMemRead
* @函数描述： 向外部存储器地址Addr开始处，连续读出Size个字节数据，放到Buf里
* @参    数： 
* @返 回 值： 
* @备    注：
******************************************************************************/ 
DRV_MEM_STATUS_ENUM DrvMemRead(u16 Addr, u16 Size, u8* Buf)
{
	u16 AddrStart 	= Addr;
	u16 AddrEnd		= 0;

	u16 SectorTemp	= 0;
	u16 SectorStart	= 0;
	u16 SectorEnd	= 0;

	u8* BufPtr = Buf;
	u16 SizeRead = Size;
	
	if(0 == Size)
		return DRV_MEM_ERR;

	AddrEnd = AddrStart + Size - 1;
	SectorStart = AddrStart/DRV_MEM_SECTOR;
	SectorEnd = AddrEnd/DRV_MEM_SECTOR;

	if( SectorStart == SectorEnd )//在一个扇区
	{
		if(BspI2CMasterMemRead(BSP_I2C_COM_EEPROM, DRV_MEM_ADDR, AddrStart, MEM_ADD_SIZE_16BIT, BufPtr, SizeRead)!=BSP_I2C_OK)
		{
			return DRV_MEM_ERR;
		}
	}
	else//多个扇区
	{
		//除了最后一个扇区
		for( SectorTemp = SectorStart; SectorTemp < SectorEnd; SectorTemp++ )
		{
			SizeRead = (SectorTemp+1)*DRV_MEM_SECTOR - AddrStart;
			if(BspI2CMasterMemRead(BSP_I2C_COM_EEPROM, DRV_MEM_ADDR, AddrStart, MEM_ADD_SIZE_16BIT, BufPtr, SizeRead)!=BSP_I2C_OK)
			{
				return DRV_MEM_ERR;
			}
			BufPtr += SizeRead;
			AddrStart += SizeRead;
		}

		//最后一个扇区
		SizeRead = AddrEnd - AddrStart + 1;
		if(BspI2CMasterMemRead(BSP_I2C_COM_EEPROM, DRV_MEM_ADDR, AddrStart, MEM_ADD_SIZE_16BIT, BufPtr, SizeRead)!=BSP_I2C_OK)
		{
			return DRV_MEM_ERR;
		}
	}
	return DRV_MEM_OK;
}

/******************************************************************************
* @函 数 名： DrvMemWrite
* @函数描述： 向外部存储器地址Addr开始处，连续写入Size个字节数据
* @参    数： 
* @返 回 值： 
* @备    注：
******************************************************************************/ 
DRV_MEM_STATUS_ENUM DrvMemWrite(u16 Addr, u16 Size, u8* Buf)
{
	
	u16 AddrStart 	= Addr;
	u16 AddrEnd		= 0;

	u16 SectorTemp	= 0;
	u16 SectorStart	= 0;
	u16 SectorEnd	= 0;

	u8* BufPtr = Buf;
	u16 SizeWrite = Size;
	
	if(0 == Size)
		return DRV_MEM_ERR;

	AddrEnd = AddrStart + Size - 1;
	SectorStart = AddrStart/DRV_MEM_SECTOR;
	SectorEnd = AddrEnd/DRV_MEM_SECTOR;

	if( SectorStart == SectorEnd )//在一个扇区
	{
		if(BspI2CMasterMemWrite(BSP_I2C_COM_EEPROM, DRV_MEM_ADDR, AddrStart, MEM_ADD_SIZE_16BIT, BufPtr, SizeWrite)!=BSP_I2C_OK)
		{
			return DRV_MEM_ERR;
		}
	}
	else//多个扇区
	{
		//除了最后一个扇区
		for( SectorTemp = SectorStart; SectorTemp < SectorEnd; SectorTemp++ )
		{
			SizeWrite = (SectorTemp+1)*DRV_MEM_SECTOR - AddrStart;
			if(BspI2CMasterMemWrite(BSP_I2C_COM_EEPROM, DRV_MEM_ADDR, AddrStart, MEM_ADD_SIZE_16BIT, BufPtr, SizeWrite)!=BSP_I2C_OK)
			{
				return DRV_MEM_ERR;
			}
			BufPtr += SizeWrite;
			AddrStart += SizeWrite;
			osDelay(WRITE_NEED_TIME);//DrvMemStopTime(60000);手册中写的连续写至少间隔5ms
		}

		//最后一个扇区
		SizeWrite = AddrEnd - AddrStart + 1;
		if(BspI2CMasterMemWrite(BSP_I2C_COM_EEPROM, DRV_MEM_ADDR, AddrStart, MEM_ADD_SIZE_16BIT, BufPtr, SizeWrite)!=BSP_I2C_OK)
		{
			return DRV_MEM_ERR;
		}
	}
	osDelay(WRITE_NEED_TIME);
	return DRV_MEM_OK;
	
}

/******************************************************************************
* @函 数 名： DrvMemClear
* @函数描述： 整个EEPROM清零
* @参    数： 
* @返 回 值： 
* @备    注：
******************************************************************************/ 
DRV_MEM_STATUS_ENUM DrvMemClear(u32 sector)
{
	u8 ClearBuf[DRV_MEM_SECTOR] = {0xff};
	u16 AddrStart 	= 0;
	
	memset(ClearBuf, 0 ,DRV_MEM_SECTOR);
  
  for(u16 i = 0; i < sector; i++)//512 扇区
  {
    if(BspI2CMasterMemWrite(BSP_I2C_COM_EEPROM, DRV_MEM_ADDR, AddrStart, MEM_ADD_SIZE_16BIT, ClearBuf, 128)!=BSP_I2C_OK)
    {
      return DRV_MEM_ERR;
    }
    AddrStart += DRV_MEM_SECTOR;
    osDelay(WRITE_NEED_TIME);//DrvMemStopTime(60000);手册中写的连续写至少间隔5ms
  }

	return DRV_MEM_OK;
	
}





/*********END OF FILE****/

