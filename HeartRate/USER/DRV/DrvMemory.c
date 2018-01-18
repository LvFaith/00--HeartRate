#include "DrvMemory.h"

#define DRV_MEM_ADDR (0x50) //0xA0>>1

#define DRV_MEM_SECTOR (128)

#define WRITE_NEED_TIME (7)//�ֲ�д����5ms������ϵͳ�£�osDelay(7) ����ʱ��ms���� (6,8)������

/*********************�洢ƫ���б�*****************************/
#define  DRV_MEM_START     (0x00)  //


/******************************************************************************
* @�� �� ���� DrvMemRead
* @���������� ���ⲿ�洢����ַAddr��ʼ������������Size���ֽ����ݣ��ŵ�Buf��
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע��
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

	if( SectorStart == SectorEnd )//��һ������
	{
		if(BspI2CMasterMemRead(BSP_I2C_COM_EEPROM, DRV_MEM_ADDR, AddrStart, MEM_ADD_SIZE_16BIT, BufPtr, SizeRead)!=BSP_I2C_OK)
		{
			return DRV_MEM_ERR;
		}
	}
	else//�������
	{
		//�������һ������
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

		//���һ������
		SizeRead = AddrEnd - AddrStart + 1;
		if(BspI2CMasterMemRead(BSP_I2C_COM_EEPROM, DRV_MEM_ADDR, AddrStart, MEM_ADD_SIZE_16BIT, BufPtr, SizeRead)!=BSP_I2C_OK)
		{
			return DRV_MEM_ERR;
		}
	}
	return DRV_MEM_OK;
}

/******************************************************************************
* @�� �� ���� DrvMemWrite
* @���������� ���ⲿ�洢����ַAddr��ʼ��������д��Size���ֽ�����
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע��
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

	if( SectorStart == SectorEnd )//��һ������
	{
		if(BspI2CMasterMemWrite(BSP_I2C_COM_EEPROM, DRV_MEM_ADDR, AddrStart, MEM_ADD_SIZE_16BIT, BufPtr, SizeWrite)!=BSP_I2C_OK)
		{
			return DRV_MEM_ERR;
		}
	}
	else//�������
	{
		//�������һ������
		for( SectorTemp = SectorStart; SectorTemp < SectorEnd; SectorTemp++ )
		{
			SizeWrite = (SectorTemp+1)*DRV_MEM_SECTOR - AddrStart;
			if(BspI2CMasterMemWrite(BSP_I2C_COM_EEPROM, DRV_MEM_ADDR, AddrStart, MEM_ADD_SIZE_16BIT, BufPtr, SizeWrite)!=BSP_I2C_OK)
			{
				return DRV_MEM_ERR;
			}
			BufPtr += SizeWrite;
			AddrStart += SizeWrite;
			osDelay(WRITE_NEED_TIME);//DrvMemStopTime(60000);�ֲ���д������д���ټ��5ms
		}

		//���һ������
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
* @�� �� ���� DrvMemClear
* @���������� ����EEPROM����
* @��    ���� 
* @�� �� ֵ�� 
* @��    ע��
******************************************************************************/ 
DRV_MEM_STATUS_ENUM DrvMemClear(u32 sector)
{
	u8 ClearBuf[DRV_MEM_SECTOR] = {0xff};
	u16 AddrStart 	= 0;
	
	memset(ClearBuf, 0 ,DRV_MEM_SECTOR);
  
  for(u16 i = 0; i < sector; i++)//512 ����
  {
    if(BspI2CMasterMemWrite(BSP_I2C_COM_EEPROM, DRV_MEM_ADDR, AddrStart, MEM_ADD_SIZE_16BIT, ClearBuf, 128)!=BSP_I2C_OK)
    {
      return DRV_MEM_ERR;
    }
    AddrStart += DRV_MEM_SECTOR;
    osDelay(WRITE_NEED_TIME);//DrvMemStopTime(60000);�ֲ���д������д���ټ��5ms
  }

	return DRV_MEM_OK;
	
}





/*********END OF FILE****/

