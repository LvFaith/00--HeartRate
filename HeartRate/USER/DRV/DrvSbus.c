
/**
* @file     
* @brief    
* @version  
* @author   
* @date     2016-6-30
* @note     
*/

/*============================ INCLUDES ======================================*/
 
#include <string.h>
#include "DrvSbus.h"
#include "CommonStream.h"

#ifdef NUAA_SIMULATION
#include "dlink.h"
#endif
 
/*============================ MACROS ========================================*/

#define R9DS (0)//R9DS接收机最后一个字节是从0x00到0xFF一直循环!fuck
 
#define SBUS_RX_FIFO_SIZE     (64)
#define SBUS_RX_FRAME_LEN     (25)
#define SBUS_RX_SEM_WAIT_MS   (100)//100ms
 
/*============================ TYPES =========================================*/

typedef struct
{
	u16 Channel[20];
  u32 Timestamp;
} 
SbusDataStruct;

static SbusDataStruct s_SbusData;

#ifdef NUAA_SIMULATION
u8 rc_enable_cnt = 0;
#endif
 
/*============================ LOCAL VARIABLES ===============================*/
 
 
static CommonStreamRxFifoStruct s_SbusRxFifo;
static u8 s_SbusRxFifoBuf[SBUS_RX_FIFO_SIZE];
static u8 s_SbusRxFrameBuf[SBUS_RX_FRAME_LEN];


static osSemaphoreId s_SbusRxSem;

static void            SbusRxProcessFrame(CommonStreamRxFifoStruct const * const fifo);
static FrameStatusEnum SbusRxSearchFrame(CommonStreamRxFifoStruct * const fifo);
 
/*============================ IMPLEMENTATION ================================*/
 
static void SbusSaveRxData(u8 const * buf, u16 size)
{
  CommonStreamRxFifoWriteIn(&s_SbusRxFifo, buf, size);
}

u32 GetSbusTimestamp( void )
{
  return s_SbusData.Timestamp;
}

void DrvSbusInit(void)
{
  //rx stream init
  memset( s_SbusRxFifoBuf,  0, sizeof(s_SbusRxFifoBuf)  );
  memset( s_SbusRxFrameBuf, 0, sizeof(s_SbusRxFrameBuf) );

  osSemaphoreDef(SbusBinarySemRx);
  s_SbusRxSem = osUserSemaphoreCreate(osSemaphore(SbusBinarySemRx), 1, 0);
	s_SbusRxFifo.SemRxRdy = &s_SbusRxSem;
	
	CommonStreamRxFifoInit(	&s_SbusRxFifo,
													"SbusRxFifo",
													s_SbusRxFifoBuf,
													SBUS_RX_FIFO_SIZE,
													s_SbusRxFrameBuf,
													SBUS_RX_FRAME_LEN,
													s_SbusRxFifo.SemRxRdy,
													SBUS_RX_SEM_WAIT_MS,
													NULL,
													(void *)SbusRxSearchFrame,
													(void *)SbusRxProcessFrame);

  g_BspUartProRxFunc.uart2 = &SbusSaveRxData;//注册实际的底层接收函数    和地面站相比没有流传送初始化 

}

static FrameStatusEnum SbusRxSearchFrame(CommonStreamRxFifoStruct * const fifo)
{
  u16 ReadPos;

  /* check whether data enough */
  if (fifo->CurFifoLen < fifo->MaxFrameLen) /* make an error: "buffer_length() > MIN_UBX_MSG_LENGTH" */
  {
    //trace(TINFO,"too few data, fifolen =%d",fifo->CurFifoLength);
    return RX_FRAME_LESS;
  }

  ReadPos = fifo->ReadPos;
  
  /* check header 0xA0 */
  if (0x0F != fifo->FifoBuf[ReadPos])
  {
    //trace(TEROR,"header!=0x0f");
    return RX_FRAME_ERR;
  }

  
  ReadPos += 24;
  ReadPos %= fifo->MaxFifoLen;
//  SEGGER_RTT_printf(0,"[24] = %02x  ",fifo->FifoBuf[ReadPos]);
  
  #if !R9DS
  if( (0x00 != fifo->FifoBuf[ReadPos]) && (0x08 != fifo->FifoBuf[ReadPos]) &&
      (0x04 != fifo->FifoBuf[ReadPos]) && (0x14 != fifo->FifoBuf[ReadPos]) && (0x24 != fifo->FifoBuf[ReadPos]) && (0x34 != fifo->FifoBuf[ReadPos]) )//0x00: 6208  0x08 0x04 0x14 0x24 0x34 : 7008 
  {
    //trace(TEROR,"header!=0x0f");
    return RX_FRAME_ERR;
  }
  #endif

  ReadPos = fifo->ReadPos;
  /* copy bytes from fifo to frame[] */
  for (u16 i = 0; i < fifo->MaxFrameLen; i++)
  {
    fifo->FrameBuf[i] = fifo->FifoBuf[ReadPos++];
    ReadPos %= fifo->MaxFifoLen;
  }

  /* return frame length */
  
  fifo->CurFrameLen = fifo->MaxFrameLen;
  
  return RX_FRAME_OK;
}

/******************************************************************************
* @函 数 名： SbusProcessFrame
* @函数描述： 校验成功对一帧数据进行处理
* @参    数： 
* @返 回 值：
* @备    注：
******************************************************************************/ 
void  SbusRxProcessFrame(CommonStreamRxFifoStruct const * const fifo)
{
  u8 i;
  u8* Buf = fifo->FrameBuf;
  u16 ChannelTemp[17];
    
  ChannelTemp[1]  = ((Buf[2]  & 0x07) << 8) | (Buf[1]);
  ChannelTemp[2]  = ((Buf[3]  & 0x3F) << 5) | (Buf[2]  >> 3);
  ChannelTemp[3]  = ((Buf[5]  & 0x01) << 10)| (Buf[4]  << 2) | (Buf[3] >> 6);
  ChannelTemp[4]  = ((Buf[6]  & 0x0F) << 7) | (Buf[5]  >> 1);
  ChannelTemp[5]  = ((Buf[7]  & 0x7F) << 4) | (Buf[6]  >> 4);
  ChannelTemp[6]  = ((Buf[9]  & 0x03) << 9) | (Buf[8]  << 1) | (Buf[7] >> 7);
  ChannelTemp[7]  = ((Buf[10] & 0x1F) << 6) | (Buf[9]  >> 2);
  ChannelTemp[8]  = ((Buf[11] & 0xFF) << 3) | (Buf[10] >> 5);
  ChannelTemp[9]  = ((Buf[13] & 0x07) << 8) | (Buf[12]);
  ChannelTemp[10] = ((Buf[14] & 0x3F) << 5) | (Buf[13] >> 3);
  ChannelTemp[11] = ((Buf[16] & 0x01) << 10)| (Buf[15] << 2) | (Buf[14] >> 6);
  ChannelTemp[12] = ((Buf[17] & 0x0F) << 7) | (Buf[16] >> 1);
  ChannelTemp[13] = ((Buf[18] & 0x7F) << 4) | (Buf[17] >> 4);
  ChannelTemp[14] = ((Buf[20] & 0x03) << 9) | (Buf[19] << 1) | (Buf[18] >> 7);
  ChannelTemp[15] = ((Buf[21] & 0x1F) << 6) | (Buf[20] >> 2);
  ChannelTemp[16] = ((Buf[22] & 0xFF) << 3) | (Buf[21] >> 5);

  for( i = 1; i <= 16; i++ )
  {
    //Channel[i] = (u16)(-0.744f * ChannelTemp[i] + 2262);
    s_SbusData.Channel[i] = (u16)(0.606f * ChannelTemp[i] + 879.39f);
  }
//  for( i = 5; i <= 16; i++ )//T10
//  {
//    s_SbusData.Channel[i] = (u16)(0.4519774f * ChannelTemp[i] + 1037.175f);
//  }
  
  
  if(Buf[23] & (1<<1))//bit 1
  {
    s_SbusData.Channel[17] = 1900;
  }
  else
  {
    s_SbusData.Channel[17] = 1100;
  }

  //DigiChannel 2
  if(Buf[23] & (1))//bit 0
  {
    s_SbusData.Channel[18] = 1900;
  }
  else
  {
    s_SbusData.Channel[18] = 1100;
  }
 
  s_SbusData.Channel[19] = (u16)(Buf[23] & 0x0C);//signal is lost/fail/ok?
  
  s_SbusData.Timestamp = HAL_GetTick();
  
#ifdef NUAA_SIMULATION  
  
  if(HAL_IO.RCin_enabled & 0x01)
  {
    for(i = 0; i< 18 ; i++)
   HAL_IO.RCin_channels[i] = s_SbusData.Channel[i+1];
  }
  
  rc_enable_cnt = 3;
#endif  

#if 0
  for(u8 j = 0; j < 20; j++)
      SEGGER_RTT_printf(0, "CH%d:%04d  ",j,s_SbusData.Channel[j]);
   SEGGER_RTT_printf(0,"SBUS Rx Data End %d\n",HAL_GetTick() );
#endif
}

void SbusSetRemoteControlData(u16 *channel, u32* timestamp)
{
  channel[1] = s_SbusData.Channel[5];
  channel[2] = s_SbusData.Channel[8];
  channel[3] = s_SbusData.Channel[4];
  channel[4] = s_SbusData.Channel[1];
  channel[5] = s_SbusData.Channel[2];
  channel[6] = s_SbusData.Channel[3];
  
  channel[7] = s_SbusData.Channel[6];  //
  channel[8] = s_SbusData.Channel[12];  //
  channel[9] = s_SbusData.Channel[7];  //
  channel[10] = s_SbusData.Channel[9];
  channel[11] = s_SbusData.Channel[10];
  channel[12] = s_SbusData.Channel[18]; // PTZ SJ
  
  channel[16] = 1500;//s_SbusData.Channel[18]; // Emergency
  channel[17] = (s_SbusData.Channel[19] & 0x8); // Lost

  channel[19] = s_SbusData.Channel[17];
 
  *timestamp = s_SbusData.Timestamp;
}


void SbusRxDaemon(void const * argument)   // 遥控器接收线程  
{
  /* USER CODE BEGIN SbusRxDaemon */
  osDelay(2000);
	DrvSbusInit();   // 遥控器初始化 
  while(1)
  {
  	CommonStreamRxFifoProcess(&s_SbusRxFifo);  // 接收数据流处理（遥控器接收fifo）
  }
  /* USER CODE END SbusRxDaemon */
}



