
/*============================ INCLUDES ======================================*/
#include "DrvComPc.h"
#include "CMSIS_OS.h"
#include "commonstream.h"
#include "DrvCrc.h"
#include "SEGGER_RTT.h"
#include "AppInfo.h"
#include "DrvComSlaver.h"
#include "DrvProductionTest.h"
#include "rsa_wrap.h"
#include <string.h>
#ifdef NUAA_SIMULATION
#include "dlink.h"
#endif
/*============================ MACROS ========================================*/

/*============================ TYPES =========================================*/



/*============================ GLOBAL VARIABLES ==============================*/
u8 g_DeviceInfo[176] = {0};//设备相关信息
u8 g_DeviceInfoSet[176] = {0};//设备相关信息
u8 g_RsaInfo[256] = {0};//读取到的Rsa信息
u8 g_RsaInfoSet[256] = {0};//要设置的Rsa信息
u8 g_EnableFunc[4] = {0};
u8 g_Sn[64] = {0};
u8 g_SetSnFlag = 0;
u8 g_ProductionTime[16] = {0};
u8 g_SetProductionTimeFlag = 0;

u8 g_ProductionResult[226] = {0};//只存16个数据
u8 g_SetProductionResultFlag = 0;

u8 g_WriteDeviceAndRsaInfo = 0;


extern osMutexId PcTxMutexHandle;

const PartOfHeadStruct partOfReturnHead = 
{
	.DeviceType  = DEVICE_TYPE,
	.DeviceId    = DEVICE_ID,
	.MasCmd      = MAS_CMD_UPDATE,
};

const PartOfHeadStruct partOfBootHead = 
{
	.DeviceType  = DEVICE_TYPE,
	.DeviceId    = DEVICE_ID,
	.MasCmd      = MAS_CMD_UPDATE,
};


/*============================ IMPLEMENTATION ================================*/


/*============================ INCLUDES ======================================*/

/*========================= tx stream to slaver ==================================*/
#define COM_PC_TX_QUEUE_BUF_SIZE   	(2) //发送消息队列大小
#define COM_PC_TX_BUF_SIZE_MAX      (1024+UPDATE_PACK_ID_SIZE+18+8)//目前一次最多发多少字节，目前最长的是对从机的升级数据

static CommonStreamTxStruct s_ComPcTxStream;

static CommonStreamTxQueueBufStruct s_ComPcTxQueueBuf[COM_PC_TX_QUEUE_BUF_SIZE];
static u8 s_ComPcTxBuf[COM_PC_TX_QUEUE_BUF_SIZE][COM_PC_TX_BUF_SIZE_MAX];
/*==================================== end ======================================*/


/*========================= rx stream from slaver ================================*/
#define COM_PC_RX_FIFO_SIZE           ( 2048 )
#define COM_PC_RX_FRAME_LEN_MAX       ( 1024+UPDATE_PACK_ID_SIZE+17 )
#define COM_PC_RX_FRAME_LEN_MIN       ( 17 )
#define COM_PC_RX_WAIT_TIME           ( 1000 )


static CommonStreamRxFifoStruct s_ComPcRxFifo;
static u8 s_ComPcRxFifoBuf[COM_PC_RX_FIFO_SIZE];
static u8 s_ComPcRxFrameBuf[COM_PC_RX_FRAME_LEN_MAX];

static osSemaphoreId s_ComPcRxSem;

static void            ComPcRxProcessFrame(CommonStreamRxFifoStruct const * const fifo);
static FrameStatusEnum ComPcRxSearchFrame(CommonStreamRxFifoStruct * const fifo);


/*============================ IMPLEMENTATION ================================*/

#define ReadIndex(ReadPos) ((ReadPos) % (COM_PC_RX_FIFO_SIZE))


static FrameStatusEnum ComPcRxSearchFrame(CommonStreamRxFifoStruct * const fifo)
{
  u16 i, readPos;
	u16 crc16;
	u32 crc32;
  u8 tempBuf[16];
	static u16 frameLen = 0;
	static u8 s_HeadOk = 0;//避免重复找头
  /* check whether data enough */
  if (fifo->CurFifoLen < COM_PC_RX_FRAME_LEN_MIN) /* make an error: "buffer_length() > MIN_UBLOX_MSG_LENGTH" */
  {
    //trace(TINFO,"too few data, fifolen =%d",FifoLen);
    return RX_FRAME_LESS;
  }

  readPos = fifo->ReadPos;

  /* check header 0xA5 */
	if( 0 == s_HeadOk )
	{
		if ( 0xA5 != fifo->FifoBuf[ReadIndex(readPos)] )
		{
			return RX_FRAME_ERR;
		}

	  /* check length */
		for( i = 0; i < 2; i++ )
		{
			tempBuf[i] = fifo->FifoBuf[ReadIndex(readPos+i+1)];
		}

	  memcpy( &frameLen, tempBuf, 2 );

		if( (frameLen > COM_PC_RX_FRAME_LEN_MAX ) || (frameLen < COM_PC_RX_FRAME_LEN_MIN ) )
		{
			return RX_FRAME_ERR;
		}

	  /* check crc16 */
		/* copy header bytes from fifo to frame[] */
		CommonStreamRxFifoRead(fifo, fifo->FrameBuf, HEAD_LEN );

		memcpy( &crc16, &(fifo->FrameBuf[HEAD_LEN-2]), 2 );
	  if(crc16 != ( (u16)GetCrcCheck( fifo->FrameBuf, HEAD_LEN-2 ) ) )
	  {
	    return RX_FRAME_ERR;
	  }

		s_HeadOk = 1;//头校验成功
	}

  /* check data enough or not */
  if (fifo->CurFifoLen < frameLen)
  {
    return RX_FRAME_LESS;
  }

  /* copy bytes from fifo to frame[] */
  CommonStreamRxFifoRead(fifo, fifo->FrameBuf, frameLen );

	memcpy( &crc32, &(fifo->FrameBuf[frameLen-4]), 4 );
  if( crc32 !=  GetCrcCheck( fifo->FrameBuf, frameLen - 4 ) )
  {
    s_HeadOk = 0;
		frameLen = 0;
    return RX_FRAME_ERR;
  }

	
  /* return frame length */
  fifo->CurFrameLen = frameLen;

	s_HeadOk = 0;
  frameLen = 0;
  return RX_FRAME_OK;
}


static inline void ProcessStartBootCmd( u8 const * buf, u16 size )
{
  if( size < 27 )
  {
    SEGGER_RTT_printf(0,"startBootLenErr %d\n",size);
    return;
  }
	
   
  SEGGER_RTT_printf(0,"resetToBoot\n");
  //需要加入保护，在空中收到此命令不能reset
  vPortSetBASEPRI(0);
  NVIC_SystemReset();
}

void RsaDecrypt( int * enableFunc, u8 * uidBuf )
{
   static u8 buf[256] = {0};
   size_t outLen;
   mbedtls_rsa_process(PUB_DECRYPT, DECODE_SHOUQUAN_KEY, 256, g_RsaInfo, &outLen, buf);

   memcpy( uidBuf, &buf[176+8], 12 );
   memcpy( enableFunc, &buf[176+8+12], 4 );
}

static inline void GetUid( u8 const * buf, u16 size )
{
  ComPcModeStruct dst;

  union 
  {
    u8 buf[12];
    u32 uid[3];
  }UidUnion;
  
  if( 18 <= size )
  {
    UidUnion.uid[0] = UID1;
    UidUnion.uid[1] = UID2;
    UidUnion.uid[2] = UID3;
    
    dst.mode = BODY_CMD_GET_UID;
    dst.type = buf[SOURCE_TYPE_INDEX];
    dst.id   = buf[SOURCE_ID_INDEX];
    dst.errCode = NO_ERR;
    SendToPc( dst, UidUnion.buf, 12 );
  }
}


static inline void SetSn( u8 const * buf, u16 size )
{
  int i = 0;
  ComPcModeStruct dst;
  SEGGER_RTT_printf(0,"snLen %d\n",size-18);
  if( size > (64+18) )
  {
    return;
  }

  for( i = 0; i < size-18; i++ )
  {
    if( '\0' == buf[BODY_INDEX+1+i] )
    {
      break;
    }
  }
  
  if( i >= (size-18) )
  {
    dst.errCode = NORMAL_ERR;
  }
  else
  {
    dst.errCode = NO_ERR;
    memset( g_Sn, 0, sizeof(g_Sn) );
    memcpy( g_Sn, &buf[BODY_INDEX+1], size-18 );
    g_SetSnFlag = 1;
  }

  dst.mode = BODY_CMD_SET_SN;
  dst.type = buf[SOURCE_TYPE_INDEX];
  dst.id   = buf[SOURCE_ID_INDEX];
  SendToPc( dst, NULL, 0 ); 
  
}


static inline void SetProductionTime( u8 const * buf, u16 size )
{
  int i = 0;
  ComPcModeStruct dst;
  SEGGER_RTT_printf(0,"ProductionLen %d\n",size-18);
  if( size > (16+18) )
  {
    return;
  }

  for( i = 0; i < size-18; i++ )
  {
    if( '\0' == buf[BODY_INDEX+1+i] )
    {
      break;
    }
  }
  
  if( i >= (size-18) )
  {
    dst.errCode = NORMAL_ERR;
  }
  else
  {
    dst.errCode = NO_ERR;
    memset( g_ProductionTime, 0, sizeof(g_ProductionTime) );
    memcpy( g_ProductionTime, &buf[BODY_INDEX+1], size-18 );
    g_SetProductionTimeFlag = 1;
  }

  dst.mode = BODY_CMD_SET_PRODUCTION_TIME;
  dst.type = buf[SOURCE_TYPE_INDEX];
  dst.id   = buf[SOURCE_ID_INDEX];
  SendToPc( dst, NULL, 0 ); 
  
}

static inline void GetRsa( u8 const * buf, u16 size )
{
//  extern unsigned long StackUavCanHandle;
//  extern unsigned long StackComPcRxHandle;
//  extern unsigned long StackProcessQEkfHandle;
  
  ComPcModeStruct dst;

  //SEGGER_RTT_printf( 0,"uavStack %d comPcRxStack %d StackProcessQEkfHandle %d\n", StackUavCanHandle,StackComPcRxHandle,StackProcessQEkfHandle);
  if( ( (3+18) == size ) &&
      ( 0 == memcmp( &buf[BODY_INDEX+1], "GET", 3) )
    )
  {
    static u8 inBuf[256] = {0};
    static u8 outBuf[512] = {0};
    size_t outLen = 0;
    int inLen = 0;
     
    memcpy( inBuf+inLen, "ZEROTECH",   8   ); inLen += 8;
    memcpy( inBuf+inLen, g_DeviceInfo, 176 ); inLen += 176;

    u32 u32Temp = UID1;
    memcpy( inBuf+inLen, &u32Temp,     4   ); inLen += 4;

    u32Temp = UID2;
    memcpy( inBuf+inLen, &u32Temp,     4   ); inLen += 4;

    u32Temp = UID3;
    memcpy( inBuf+inLen, &u32Temp,     4   ); inLen += 4;
    memcpy( inBuf+inLen, g_EnableFunc, 4   ); inLen += 4;
    memcpy( inBuf+inLen, "EFLY",       4   ); inLen += 4;
    
    mbedtls_rsa_process( PUB_ENCRYPT, COM_PC_KEY, inLen, inBuf, &outLen, outBuf );
    if( outLen != 256 )
    {
      SEGGER_RTT_printf( 0, "RsaOutlenErr %d   %d\n", inLen, outLen );
      return;
    }

    memcpy( &outBuf[256], g_RsaInfo, 256 );


    dst.mode = BODY_CMD_GET_RSA;
    dst.type = buf[SOURCE_TYPE_INDEX];
    dst.id   = buf[SOURCE_ID_INDEX];
    dst.errCode = NO_ERR;
    SendToPc( dst, outBuf, 512 );

    #if 0
    SEGGER_RTT_printf(0, "PUB  ENC\n");
    for (int i=0; i<16; i++)
    {
      for (int j=0; j<16; j++)
      {
        SEGGER_RTT_printf(0, "0x%02X, ", outBuf[i*16 + j]);    
      }
      SEGGER_RTT_printf(0, "\n");
    }
    SEGGER_RTT_printf(0, "\n");


     int inlen = 256;
     static unsigned char tmp[256] = {0};
     size_t tmp_olen = 0;
     mbedtls_rsa_process(PRI_DECRYPT, TEST_KEY, inlen, outBuf, &tmp_olen, tmp);
     SEGGER_RTT_printf(0, "PRI  DEC\n");
    
     for (int i=0; i<tmp_olen; i++)
     {
        SEGGER_RTT_printf(0, "%c",tmp[i]); 
      }
      SEGGER_RTT_printf(0, "\n"); 
    #endif
  }
}

static inline void SetRsa( u8 const * buf, u16 size )
{
  #define SET_RSA_ERR (1)
  
//  extern unsigned long StackUavCanHandle;
//  extern unsigned long StackComPcRxHandle;
//  extern unsigned long StackProcessQEkfHandle;

  ComPcModeStruct dst;
  static u8 testBuf[256] = {0};
  size_t outLen = 0;

  union 
  {
    u8 buf[12];
    u32 uid[3];
  }UidUnion;

  //SEGGER_RTT_printf( 0,"SetRsa uavStack %d comPcRxStack %d StackProcessQEkfHandle %d\n", StackUavCanHandle,StackComPcRxHandle,StackProcessQEkfHandle);
  if( (256+18) != size )
  {
    SEGGER_RTT_printf( 0, "setRsaLenErr %d\n",size);
    return;
  }

  dst.errCode = NO_ERR;
  memcpy( g_RsaInfoSet, &buf[BODY_INDEX+1], 256 );

  #if 0
  for (int j=0; j< 256; j++)
  {
    SEGGER_RTT_printf(0, "%02X ", g_RsaInfoSet[j]);    
  }
  SEGGER_RTT_printf(0, "rsaDataSetFromPc\n");
  osDelay(100);
  #endif
  
  memset( testBuf, 0, 256 );
  mbedtls_rsa_process(PUB_DECRYPT, DECODE_SHOUQUAN_KEY, 256, g_RsaInfoSet, &outLen, testBuf);

  if( 0 != memcmp( testBuf, "ZEROTECH", 8 ) )
  {
    for (int i=0; i < 8; i++)
    {
       SEGGER_RTT_printf(0, "%c",testBuf[i]); 
    }
    SEGGER_RTT_printf(0, "hErr\n");
    dst.errCode = SET_RSA_ERR;
  }

  if( 0 != memcmp( &testBuf[200], "EFLY", 4 ) )
  {
    for (int i=0; i < 4; i++)
    {
       SEGGER_RTT_printf(0, "%c",testBuf[i+200]); 
    }
    SEGGER_RTT_printf(0, "eErr\n");
    dst.errCode = SET_RSA_ERR;
  }

  UidUnion.uid[0] = UID1;
  UidUnion.uid[1] = UID2;
  UidUnion.uid[2] = UID3;

  if( 0 != memcmp( &testBuf[176+8], UidUnion.buf, 12 ) )
  {
    for (int i=0; i < 12; i++)
    {
       SEGGER_RTT_printf(0, "%02x ",testBuf[i+176+8]); 
    }
    SEGGER_RTT_printf(0, "rsaSetUIDErr\n");
    dst.errCode = SET_RSA_ERR;
  }

  if( SET_RSA_ERR != dst.errCode )
  {
    SEGGER_RTT_printf(0, "rsaSetOk\n");
  
    //设置设备名称 //设置硬件版本号 等。。
    memcpy( g_DeviceInfoSet, &testBuf[8], 176 ); 
  
    g_WriteDeviceAndRsaInfo = 1;
  }

  dst.mode = BODY_CMD_SET_RSA;
  dst.type = buf[SOURCE_TYPE_INDEX];
  dst.id   = buf[SOURCE_ID_INDEX];
  SendToPc(dst, NULL, 0 );
   
#if 0 
  mbedtls_rsa_process( PUB_ENCRYPT, TEST_KEY, inLen, inBuf, &outLen, outBuf );
  if( outLen != 256 )
  {
    SEGGER_RTT_printf( 0, "RsaOutlenErr %d   %d\n", inLen, outLen );
    return;
  }

  memcpy( &outBuf[256], g_RsaInfo, 256 );

  dst.mode = BODY_CMD_SET_RSA;
  dst.type = buf[SOURCE_TYPE_INDEX];
  dst.id   = buf[SOURCE_ID_INDEX];
  dst.errCode = NO_ERR;
  SendToPc( dst, outBuf, 512 );
  #endif
  
}


static inline void GetDeviceInfoFormEeprom( u8 const * buf, u16 size )
{
  ComPcModeStruct dst;
  
  if( 18 <= size )
  {
    dst.mode = BODY_CMD_GET_DEVICE_INFO;
    dst.type = buf[SOURCE_TYPE_INDEX];
    dst.id   = buf[SOURCE_ID_INDEX];
    dst.errCode = NO_ERR;
    SendToPc( dst, g_DeviceInfo, 176 );
  }
}


static inline void GetProductionResultFormEeprom( u8 const * buf, u16 size )
{
  ComPcModeStruct dst;
  
  if( 18 <= size )
  {
    dst.mode = BODY_CMD_GET_PRODUCTION_RESULT;
    dst.type = buf[SOURCE_TYPE_INDEX];
    dst.id   = buf[SOURCE_ID_INDEX];
    dst.errCode = NO_ERR;
    SendToPc( dst, g_ProductionResult, 224 );
  }
}


static inline void ProcessStartBurnInTest( u8 const * buf, u16 size )
{
  ComPcModeStruct dst;
  
  if( 18 <= size )
  {
    StartBurnInTest();
    dst.mode = BODY_CMD_START_BURN_IN_TEST;
    dst.type = buf[SOURCE_TYPE_INDEX];
    dst.id   = buf[SOURCE_ID_INDEX];
    dst.errCode = NO_ERR;
    SendToPc( dst, NULL, 0 );
  }
}

static inline void ProcessStartInterfaceTest( u8 const * buf, u16 size )
{
  ComPcModeStruct dst;
  
  if( ( 34 <= size ) &&
      ( 3 == buf[BODY_INDEX+1] ) &&
      ( (2 <= g_ProductionResult[0]) && (3 >= g_ProductionResult[0])  )
    )
  {
    dst.errCode = NO_ERR;
    memcpy( g_ProductionResult, &buf[BODY_INDEX+1], 16 );
    StartInterfaceTest();
  }
  else
  {
    SEGGER_RTT_printf( 0, "StartInterFaceTestErr\n" );
    dst.errCode = NORMAL_ERR;
  }

  dst.mode = BODY_CMD_START_INTERFACE_TEST;
  dst.type = buf[SOURCE_TYPE_INDEX];
  dst.id   = buf[SOURCE_ID_INDEX];
  
  SendToPc( dst, NULL, 0 );
}


static inline void ProcessStopBurnInTest( u8 const * buf, u16 size )
{
  ComPcModeStruct dst;
  
  if( 18 <= size )
  {
    StopBurnInTest();
    dst.mode = BODY_CMD_STOP_BURN_IN_TEST;
    dst.type = buf[SOURCE_TYPE_INDEX];
    dst.id   = buf[SOURCE_ID_INDEX];
    dst.errCode = NO_ERR;
    SendToPc( dst, NULL, 0 );
  }
}




static inline void ProcessProductionTest( u8 const * buf, u16 size )
{
  if( size < 18 )
  {
    SEGGER_RTT_printf(0,"ProductionTestLenErr %d\n",size);
    return;
  }
  
  switch( buf[BODY_INDEX] )
  {
    case BODY_CMD_GET_UID:
      GetUid( buf, size );
      break;

    case BODY_CMD_GET_DEVICE_INFO:
      GetDeviceInfoFormEeprom( buf, size );
      break;

    case BODY_CMD_GET_PRODUCTION_RESULT:
      GetProductionResultFormEeprom( buf, size );
      break;

    case BODY_CMD_SET_SN:
      SetSn( buf, size );
      break;

    case BODY_CMD_SET_PRODUCTION_TIME:
      SetProductionTime( buf, size );
      break;

    case BODY_CMD_GET_RSA:
      GetRsa( buf, size );
      break;

    case BODY_CMD_SET_RSA:
      SetRsa( buf, size );
      break;

    case BODY_CMD_START_INTERFACE_TEST:
      ProcessStartInterfaceTest( buf, size );
      break;
  
    case BODY_CMD_START_BURN_IN_TEST://开始拷机
      ProcessStartBurnInTest(buf,size);
      break;
    
    case BODY_CMD_STOP_BURN_IN_TEST://停止拷机
      ProcessStopBurnInTest(buf,size);
      break;
    
    default:
      SEGGER_RTT_printf( 0, "Unknown body cmd 0x%02x\n", buf[BODY_INDEX]  );
      break;
  }

  SEGGER_RTT_printf( 0, " cmd 0x%02x\n", buf[BODY_INDEX]  );
}



static void ComPcRxProcessFrame(CommonStreamRxFifoStruct const * const fifo)
{
	#define IS_BOOT_CMD   ( (0x08 == fifo->FrameBuf[FLAG_INDEX]) && (0 == memcmp( &partOfBootHead, &(fifo->FrameBuf[DEVICE_TYPE_INDEX]), PART_OF_HEAD_SIZE)) )
	//#define IS_PC_SEND_BACK_CMD ( 0 == memcmp( &partOfReturnHead, &(fifo->FrameBuf[FLAG_INDEX]), PART_OF_HEAD_SIZE) )
  #define IS_COM_SLAVER ( (SLAVER_DEVICE_ID == fifo->FrameBuf[DEVICE_ID_INDEX]) )
	#define IS_COM_MASTER ( (DEVICE_ID == fifo->FrameBuf[DEVICE_ID_INDEX]) )
  #define IS_PRODUCTION_TEST ( (MAS_CMD_FC == fifo->FrameBuf[MAS_CMD_INDEX]) && (SUB_CMD_PRODUCION_TEST == fifo->FrameBuf[SUB_CMD_INDEX]) )
  u16 len = 0;
  
	memcpy( &len, &(fifo->FrameBuf[LEN_INDEX]) , 2 );

	
  if( IS_COM_MASTER )
  {
    if( IS_BOOT_CMD )
  	{
  	  switch( fifo->FrameBuf[SUB_CMD_INDEX] )
  	  {
  	    case SUB_CMD_START_BOOT:
  				ProcessStartBootCmd( fifo->FrameBuf, len ); 
  				break;
  	  }
		//SEGGER_RTT_printf(0,"cmd %02x\n",fifo->FrameBuf[SUB_CMD_INDEX]);
	  }
    else if( IS_PRODUCTION_TEST )
    {
      ProcessProductionTest( fifo->FrameBuf, fifo->CurFrameLen );
    }
  }
	else if( IS_COM_SLAVER )
	{

        switch( fifo->FrameBuf[MAS_CMD_INDEX] )
        {
          case MAS_CMD_UPDATE: //收到从机升级指令
            if( SUB_CMD_START_BOOT == fifo->FrameBuf[SUB_CMD_INDEX] )
            {
                      if(STATS.battery_remaining != 101)
                      STATS.battery_remaining = 101;
                      
                      if(STATS.cpu_load != 100)
                      STATS.cpu_load = 100;
            }
            break;
        }

	  SendToSlaver( PASS_THROUGH, fifo->FrameBuf, fifo->CurFrameLen );

    #if 0
		for( int i = 0; i < fifo->CurFrameLen; i++ )
		{
		  SEGGER_RTT_printf( 0, "%02x ", fifo->FrameBuf[i] );
		}
		SEGGER_RTT_printf( 0, "ThroughSlaver\n");
    #endif
//	  DrvTxToSlaver( fifo->FrameBuf, fifo->CurFrameLen );
//	  SEGGER_RTT_printf(0,"cmd %02x bodyCmd %02x \n",fifo->FrameBuf[SUB_CMD_INDEX], fifo->FrameBuf[BODY_CMD_INDEX] );
		return;
	}
	
}



/*============================ IMPLEMENTATION ================================*/

static void ComPcSaveRxData(u8 const * buf, u16 const size)
{
  CommonStreamRxFifoWriteIn(&s_ComPcRxFifo, buf, size);\
  
	#if 0
	SEGGER_RTT_printf(0,"size %d\n",size);
	
	for(int i = 0; i < size; i++)
	{
		SEGGER_RTT_printf(0,"%.2x ",buf[i]);
	}
	SEGGER_RTT_printf(0,"com pc\n");
	#endif
}

static u8 ComPcTxEnterCritical(void)
{
	if( osOK != osMutexWait(PcTxMutexHandle,osWaitForever) )
	{
		return FALSE;
	}
	
	return TRUE;
}

static u8 ComPcTxExitCritical(void)
{
	if( osOK != osMutexRelease(PcTxMutexHandle) )
	{
		return FALSE;
	}
	
	return TRUE;
}


static inline void ComPcSendBlock( u8 * buf, u32 size )
{
  BspUSBSendData( buf, size );
}

static void DrvComPcInit(void)
{
  //tx stream init
  memset( s_ComPcTxQueueBuf, 0, sizeof(s_ComPcTxQueueBuf) );
  memset( s_ComPcTxBuf,      0, sizeof(s_ComPcTxBuf)      );
  memset( &s_ComPcTxStream,  0, sizeof(CommonStreamTxStruct) );
  
  osSemaphoreDef(ComPcCountingSemTx);
  s_ComPcTxStream.CountingSemEmptyQueueSize = osUserSemaphoreCreate(osSemaphore(ComPcCountingSemTx), COM_PC_TX_QUEUE_BUF_SIZE, COM_PC_TX_QUEUE_BUF_SIZE);

  osMessageQDef(ComPcQueueTx, COM_PC_TX_QUEUE_BUF_SIZE, u32);
  s_ComPcTxStream.Queue = osMessageCreate(osMessageQ(ComPcQueueTx), NULL);

  CommonStreamTxInit( &s_ComPcTxStream,
                   		s_ComPcTxQueueBuf,
                   		&(s_ComPcTxBuf[0][0]),
                   		(u32)COM_PC_TX_QUEUE_BUF_SIZE,
                   		(u32)COM_PC_TX_BUF_SIZE_MAX,
                   		osWaitForever,
                   		0,
                   		NULL,//(void*)ComPcTxEnterCritical,
                   		NULL,//(void*)ComPcTxExitCritical,
                   		(void*)ComPcSendBlock,
                   		NULL);
      
	
  //rx stream init
  memset( s_ComPcRxFifoBuf,  0, sizeof(s_ComPcRxFifoBuf)  );
  memset( s_ComPcRxFrameBuf, 0, sizeof(s_ComPcRxFrameBuf) );

  osSemaphoreDef(ComPcBinarySemRx);
  s_ComPcRxSem = osUserSemaphoreCreate(osSemaphore(ComPcBinarySemRx), 1, 0);
	s_ComPcRxFifo.SemRxRdy = &s_ComPcRxSem;
	
	CommonStreamRxFifoInit(	&s_ComPcRxFifo,
													"ComPcRxFifo",
													s_ComPcRxFifoBuf,
													COM_PC_RX_FIFO_SIZE,
													s_ComPcRxFrameBuf,
													COM_PC_RX_FRAME_LEN_MAX,
													s_ComPcRxFifo.SemRxRdy,
													COM_PC_RX_WAIT_TIME,
													NULL,
													(void *)ComPcRxSearchFrame,
													(void *)ComPcRxProcessFrame);

  g_BspUSBReceiveProcess = &ComPcSaveRxData;//注册实际的底层接收函数

}


void ComPcRxDaemon(void const * argument)
{
  osDelay(20);
  DrvComPcInit();

  while(1)//{osDelay(1000);}
  {
    CommonStreamRxFifoProcess(&s_ComPcRxFifo);
  }
}

void ComPcTxDaemon(void const * argument)
{
	while(1)
  {
    osDelay(20);
    CommonStreamTxTask(&s_ComPcTxStream);
  }
}


static void DrvTxToPc(u8 const * buf, u16 size)
{
  CommonStreamTxPutDataToQueue(&s_ComPcTxStream, buf, size);
}


void SendToPc(ComPcModeStruct dst, u8 const * data, u16 size)
{
	#define DATA_INDEX     (BODY_CMD_INDEX+1)
  static u8 s_Buf[COM_PC_TX_BUF_SIZE_MAX] = {0xA5,
                                             [MSG_SEQ_INDEX]     = 0,
                                             [SOURCE_TYPE_INDEX] = DEVICE_TYPE,
                                             [SOURCE_ID_INDEX]   = DEVICE_ID,
                                             [MAS_CMD_INDEX]     = MAS_CMD_FC,
                                            };
	u16 len;
//	u16 u16Temp;
	u16 crc16;
	u32 crc32;
	u8 flag = TRUE;//是否需要继续发送数据
	
  ComPcTxEnterCritical();

  s_Buf[DEVICE_TYPE_INDEX] = dst.type,
  s_Buf[DEVICE_ID_INDEX]   = dst.id,
  s_Buf[RETURN_ERROR_CODE_INDEX] = dst.errCode;

  SEGGER_RTT_printf( 0,"dst.mode %02x size %d\n",dst.mode,size );
  
	switch(dst.mode)
  {
    case USB_PASS_THROUGH:
			
			if( NULL != data )
			{
			  DrvTxToPc(data, size);
			}
			flag = FALSE;
			
			break;

    case BODY_CMD_GET_UID:
    case BODY_CMD_START_INTERFACE_TEST:
    case BODY_CMD_GET_DEVICE_INFO:
    case BODY_CMD_GET_PRODUCTION_RESULT:
    case BODY_CMD_START_BURN_IN_TEST: //开始拷机
    case BODY_CMD_STOP_BURN_IN_TEST: //停止拷机
    case BODY_CMD_SET_SN:
    case BODY_CMD_SET_PRODUCTION_TIME:
		case BODY_CMD_GET_RSA:
    case BODY_CMD_SET_RSA:
    case BODY_CMD_RETURN_INTERFACE_TEST:    
			if (size > (COM_PC_TX_BUF_SIZE_MAX-19) ) 
			{
			  flag = FALSE;
			}
			else
			{
			  len = size + 19;
			  s_Buf[FLAG_INDEX]      = FLAG_RETURN;
        s_Buf[SUB_CMD_INDEX]   = SUB_CMD_PRODUCION_TEST;
			  s_Buf[BODY_CMD_INDEX]  = dst.mode;

        if( NULL != data )
        {
			    memcpy( &s_Buf[DATA_INDEX], data, size );
        }
			}

			break;
			
    default:
      flag = FALSE;
			break;
  }
	
  if(flag)
  {
    
    
		memcpy( &s_Buf[LEN_INDEX],    &len,  2 );

		crc16 = (u16)GetCrcCheck( s_Buf, HEAD_LEN - 2 );
	  memcpy( &s_Buf[CRC16_INDEX], &crc16, sizeof(u16) );
		
	  crc32 = GetCrcCheck( s_Buf, len - 4 );
	  memcpy( &s_Buf[len - 4], &crc32, sizeof(u32) );
		DrvTxToPc(s_Buf, len);
  }
	
	ComPcTxExitCritical();
}



