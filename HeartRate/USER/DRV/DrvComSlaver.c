
/*============================ INCLUDES ======================================*/
#include "DrvComSlaver.h"
#include "publicvar.h"
#include "Public_Function.h"
#include "AppSkyWay.h"
#include "DrvGcs.h"
#include "DrvComPc.h"
#include "DrvCrc.h"
#include "fuckvar.h"
#include "DrvComVio.h"

/*============================ MACROS ========================================*/

#define SLAVER_SAVE_SKYWAY_POINT  (0x00E2)//从机发送存储航点
#define SAVE_SKYWAY_TOTAL_NUM     (0x00E3)//
#define GET_PHOTO_INFO            (0x00E4)//
#define GET_PHOTO_NUM             (0x00E5)//

#define SLAVER_DTU_UPDATA         (0x0000)//从机透传升级回复内容到PC



/*============================ TYPES =========================================*/



/*============================ GLOBAL VARIABLES ==============================*/
extern osMutexId ComSlaverTxMutexHandle;
u8 s_PcLockSd = 0;
/*============================ IMPLEMENTATION ================================*/


/*============================ INCLUDES ======================================*/

/*========================= tx stream to slaver ==================================*/
#define COM_SLAVER_TX_QUEUE_BUF_SIZE   	(5) //发送消息队列大小
#define COM_SLAVER_TX_BUF_SIZE_MAX      (1024+UPDATE_PACK_ID_SIZE+17)//目前一次最多发多少字节，目前最长的是对从机的升级数据

static CommonStreamTxStruct s_ComSlaverTxStream;

static CommonStreamTxQueueBufStruct s_ComSlaverTxQueueBuf[COM_SLAVER_TX_QUEUE_BUF_SIZE];
static u8 s_ComSlaverTxBuf[COM_SLAVER_TX_QUEUE_BUF_SIZE][COM_SLAVER_TX_BUF_SIZE_MAX];
/*==================================== end ======================================*/


/*========================= rx stream from slaver ================================*/
#define COM_SLAVER_RX_FIFO_SIZE         (2048)
#define COM_SLAVER_RX_FRAME_LEN_MAX     (1024+18+8+BODY_SIZE_FOR_FC)//(1024+UPDATE_PACK_ID_SIZE+18)//( 10 + ( 2 + sizeof(SkyWayPointStruct) ) * PING_PANG_BUF_SIZE )//pingpangbuf中的一个
#define COM_SLAVER_RX_FRAME_LEN_MIN     (17)
#define COM_SLAVER_RX_WAIT_TIME         (100)


static CommonStreamRxFifoStruct s_ComSlaverRxFifo;
static u8 s_ComSlaverRxFifoBuf[COM_SLAVER_RX_FIFO_SIZE];
static u8 s_ComSlaverRxFrameBuf[COM_SLAVER_RX_FRAME_LEN_MAX];

static osSemaphoreId s_ComSlaverRxSem;

static void            ComSlaverRxProcessFrame(CommonStreamRxFifoStruct const * const fifo);
static FrameStatusEnum ComSlaverRxSearchFrame(CommonStreamRxFifoStruct * const fifo);


/*============================ IMPLEMENTATION ================================*/


#define ReadIndex(ReadPos) ((ReadPos) % (COM_SLAVER_RX_FIFO_SIZE))

static FrameStatusEnum ComSlaverRxSearchFrame(CommonStreamRxFifoStruct * const fifo)
{
  u16 i, readPos;
	u16 crc16;
	u32 crc32;
  u8 tempBuf[16];
	static u16 frameLen = 0;
	static u8 s_HeadOk = 0;//避免重复找头
  /* check whether data enough */
  if (fifo->CurFifoLen < COM_SLAVER_RX_FRAME_LEN_MIN) /* make an error: "buffer_length() > MIN_UBLOX_MSG_LENGTH" */
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

		if( (frameLen > COM_SLAVER_RX_FRAME_LEN_MAX ) || (frameLen < COM_SLAVER_RX_FRAME_LEN_MIN ) )
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




static inline void ProcessSlaverReturn( u8 const * buf, u16 len )
{
  static u8 s_Buf[64] = {0};
	u16 numTemp = 0;
  PhotoDataStruct photoTemp;
	
  if( (len < 20) || (NULL == buf) )
  {
    return;
  }

  switch( buf[RETURN_DATA_INDEX + 1] )
  {
    case BODY_CMD_POINT_SIZE:
			SEGGER_RTT_printf( 0, "returnPointSize %d\n", buf[RETURN_DATA_INDEX+2] );
			break;

		case BODY_CMD_PHOTO_SIZE:
			SEGGER_RTT_printf( 0, "returnPhotoSize %d\n", buf[RETURN_DATA_INDEX+2] );
			break;

		case BODY_CMD_POINT_INFO:
		case BODY_CMD_SET_ONE_POINT:
			if( BODY_CMD_SET_ONE_POINT == buf[RETURN_DATA_INDEX + 1] )
			{
			  memcpy( s_Buf, "$SETD", 5 );//地面站只认这个回复。。。不能用SETO原样返回
				SEGGER_RTT_printf( 0, "SendOnePointReturnGcs\n" );
			}
			else
			{
			  memcpy( s_Buf, "$SETD", 5 );
				SEGGER_RTT_printf( 0, "SendNormalPointReturnGcs\n" );
			}
			
			memcpy( &s_Buf[5], &buf[RETURN_DATA_INDEX+2], 26 );
			s_Buf[31] = CalcSum(s_Buf,31);
//			DrvGcsSendData( s_Buf, 32 );
			break;

		case BODY_CMD_POINT_NUM:
			//SEGGER_RTT_printf(0,"receive Slaver TotalNum\n");
			memcpy( s_Buf,     "$SEM",                   4  );
			memcpy( &s_Buf[4], &buf[RETURN_DATA_INDEX+2], 2 );
			memcpy( &s_Buf[6], &buf[RETURN_DATA_INDEX+2], 2 );
//			DrvGcsSendData( s_Buf, 8 );
			break;

		case BODY_CMD_GET_POINT_TOTAL_NUM:
			memcpy( &numTemp, &buf[RETURN_DATA_INDEX+2], 2 );

			if(numTemp <= SKYWAY_POINT_NUM_MAX)
			{
			  tgtnum = numTemp;
				SEGGER_RTT_printf(0,"GetTgtNum %d\n",tgtnum);
			}
			break;

		case BODY_CMD_GET_POINT_INFO:
			SaveSkywayPoint( &buf[RETURN_DATA_INDEX+2], len-20 );
			break;

		case BODY_CMD_GET_PHOTO_INFO:

			memcpy( s_Buf, "$COMM", 5 );
      memcpy( &photoTemp, &buf[RETURN_DATA_INDEX+2+4], sizeof(PhotoDataStruct) );//+4去除开始的 $POS
      s_Buf[5] = 38;//len
      s_Buf[6] = 208;//cmd id
      memcpy( &s_Buf[7], &(photoTemp.id), 2 );
      memcpy( &s_Buf[9], &(photoTemp.photoCurLatti), 28 );
      s_Buf[37] = CalcSum( s_Buf, 37 );
//      DrvGcsSendData( s_Buf, 38 );
			break;
			
    case BODY_CMD_GET_PHOTO_TOTAL_NUM:
			memcpy( &numTemp, &buf[RETURN_DATA_INDEX+2], 2 );
			g_PosTotalNum = numTemp;
		  SEGGER_RTT_printf(0,"GetPhotoNum %d\n",g_PosTotalNum);
			break;
			
  }
}

static void ComSlaverRxProcessFrame(CommonStreamRxFifoStruct const * const fifo)
{
  //#define IS_SLAVER_BOOT_CMD (MAS_CMD_UPDATE == fifo->FrameBuf[MAS_CMD_INDEX])
  #define IS_COM_PC     ( (DEVICE_TYPE != fifo->FrameBuf[DEVICE_TYPE_INDEX]) || (DEVICE_ID != fifo->FrameBuf[DEVICE_ID_INDEX]) )
  #define IS_COM_MASTER ( (DEVICE_TYPE == fifo->FrameBuf[DEVICE_TYPE_INDEX]) && (DEVICE_ID == fifo->FrameBuf[DEVICE_ID_INDEX]) )

//	u16 len = 0;
  ComPcModeStruct comPcMode;
	if( IS_COM_PC )
	{
	  comPcMode.mode = USB_PASS_THROUGH;
	  SendToPc( comPcMode, fifo->FrameBuf, fifo->CurFrameLen );
		if( fifo->FrameBuf[SUB_CMD_INDEX] == SUB_CMD_MEM_RELEASE )
    {
      s_PcLockSd = 0;
		}
		return;
	}

	if( IS_COM_MASTER )
	{
	  switch( fifo->FrameBuf[FLAG_INDEX] )
	  {
	    case FLAG_RETURN:
				ProcessSlaverReturn( fifo->FrameBuf, fifo->CurFrameLen );
				break;
	  }

		return;
	}
}



/*============================ IMPLEMENTATION ================================*/

static void ComSlaverSaveRxData(u8 const * buf, u16 const size)
{
  CommonStreamRxFifoWriteIn(&s_ComSlaverRxFifo, buf, size);\
  
	#if 0
	SEGGER_RTT_printf(0,"size %d\n",size);
	
	for(int i = 0; i < size; i++)
	{
		SEGGER_RTT_printf(0,"%.2x ",buf[i]);
	}
	SEGGER_RTT_printf(0,"com Slaver\n");
	#endif
}

static u8 ComSlaverTxEnterCritical(void)
{
	if( osOK != osMutexWait(ComSlaverTxMutexHandle,osWaitForever) )
	{
		return FALSE;
	}
	
	return TRUE;
}

static u8 ComSlaverTxExitCritical(void)
{
	if( osOK != osMutexRelease(ComSlaverTxMutexHandle) )
	{
		return FALSE;
	}
	
	return TRUE;
}


static inline void ComSlaverSendBlock( u8 * const buf, u32 const size )
{
  BspUartTransmitDataBlock( &(g_BspUartTxGroup.uart5), buf, size);
}

static void DrvComSlaverInit(void)
{
  //tx stream init
  memset( s_ComSlaverTxQueueBuf, 0, sizeof(s_ComSlaverTxQueueBuf) );
  memset( s_ComSlaverTxBuf,      0, sizeof(s_ComSlaverTxBuf)      );
  memset( &s_ComSlaverTxStream,  0, sizeof(CommonStreamTxStruct) );
  
  osSemaphoreDef(ComSlaverCountingSemTx);
  s_ComSlaverTxStream.CountingSemEmptyQueueSize = osUserSemaphoreCreate(osSemaphore(ComSlaverCountingSemTx), COM_SLAVER_TX_QUEUE_BUF_SIZE, COM_SLAVER_TX_QUEUE_BUF_SIZE);

  osMessageQDef(ComSlaverQueueTx, COM_SLAVER_TX_QUEUE_BUF_SIZE, u32);
  s_ComSlaverTxStream.Queue = osMessageCreate(osMessageQ(ComSlaverQueueTx), NULL);

  CommonStreamTxInit( &s_ComSlaverTxStream,
                   		s_ComSlaverTxQueueBuf,
                   		&(s_ComSlaverTxBuf[0][0]),
                   		(u32)COM_SLAVER_TX_QUEUE_BUF_SIZE,
                   		(u32)COM_SLAVER_TX_BUF_SIZE_MAX,
                   		osWaitForever,
                   		0,
                   		NULL,//(void*)ComSlaverTxEnterCritical,
                   		NULL,//(void*)ComSlaverTxExitCritical,
                   		(void*)ComSlaverSendBlock,
                   		NULL);
      
	
  //rx stream init
  memset( s_ComSlaverRxFifoBuf,  0, sizeof(s_ComSlaverRxFifoBuf)  );
  memset( s_ComSlaverRxFrameBuf, 0, sizeof(s_ComSlaverRxFrameBuf) );

  osSemaphoreDef(ComSlaverBinarySemRx);
  s_ComSlaverRxSem = osUserSemaphoreCreate(osSemaphore(ComSlaverBinarySemRx), 1, 0);
	s_ComSlaverRxFifo.SemRxRdy = &s_ComSlaverRxSem;
	
	CommonStreamRxFifoInit(	&s_ComSlaverRxFifo,
													"ComSlaverRxFifo",
													s_ComSlaverRxFifoBuf,
													COM_SLAVER_RX_FIFO_SIZE,
													s_ComSlaverRxFrameBuf,
													COM_SLAVER_RX_FRAME_LEN_MAX,
													s_ComSlaverRxFifo.SemRxRdy,
													COM_SLAVER_RX_WAIT_TIME,
													NULL,
													(void *)ComSlaverRxSearchFrame,
													(void *)ComSlaverRxProcessFrame);

  g_BspUartProRxFunc.uart5 = &ComSlaverSaveRxData;//注册实际的底层接收函数

}


void ComSlaverRxDaemon(void const * argument)
{
  osDelay(20);
  DrvComSlaverInit();

  while(1)//{osDelay(1000);}
  {
    CommonStreamRxFifoProcess(&s_ComSlaverRxFifo);
  }
}

void ComSlaverTxDaemon(void const * argument)
{
	while(1)
  {
    osDelay(20);
    CommonStreamTxTask(&s_ComSlaverTxStream);
  }
}


void DrvTxToSlaver(u8 const * buf, u16 size)
{
  CommonStreamTxPutDataToQueue(&s_ComSlaverTxStream, buf, size);
}


void SendToSlaver(DataModeEnum mode, u8 const * data, u16 size)
{
	#define DATA_INDEX     (BODY_CMD_INDEX+1)
  static u8 s_Buf[COM_SLAVER_TX_BUF_SIZE_MAX] = {0xA5,
                                                [MSG_SEQ_INDEX]     = 0,
		                                            [DEVICE_TYPE_INDEX] = DEVICE_TYPE,
		                                            [DEVICE_ID_INDEX]   = SLAVER_DEVICE_ID,
		                                            [MAS_CMD_INDEX]     = MAS_CMD_MEMORIZER,
		                                            [SOURCE_TYPE_INDEX] = DEVICE_TYPE,
		                                            [SOURCE_ID_INDEX]   = DEVICE_ID,
		                                            [BODY_INDEX]        = 0,//SD卡
		                                           };
  SkyWayPointStruct tempPoint;
	u16 len;
	u16 u16Temp;
	u16 crc16;
	u32 crc32;
	u8 flag = TRUE;//是否需要继续发送数据
	
  ComSlaverTxEnterCritical();

	switch(mode)
  {
    case PASS_THROUGH:
			
			if( NULL != data )
			{
			  DrvTxToSlaver(data, size);
				s_PcLockSd = 1;
			}
			flag = FALSE;
			
			break;

		case ONE_POINT_SIZE:

			if( size != sizeof(SkyWayPointStruct) )
			{
			  flag = FALSE;
			}
			else
			{
  			len = 17+4;
  			memcpy( &s_Buf[LEN_INDEX],    &len,  2 );
  
  			u16Temp = size+4;//+crc32的空间
  			memcpy( &s_Buf[DATA_INDEX], &u16Temp, 2 );
  			
  			s_Buf[FLAG_INDEX]      = FLAG_ACK;
  			s_Buf[SUB_CMD_INDEX]   = SUB_FC;
  			s_Buf[BODY_CMD_INDEX]  = BODY_CMD_POINT_SIZE;
			}
			break;
			
		case SKYWAY_POINT:
			if( ( size != sizeof(SkyWayPointStruct) ) || (NULL == data) )
			{
			  flag = FALSE;
			}
			else
			{
				len = size + BODY_SIZE_FOR_FC + 17 + 4;
				s_Buf[FLAG_INDEX]      = FLAG_ACK;
				
				s_Buf[BODY_CMD_INDEX]  = mode;
				memcpy( &s_Buf[DATA_INDEX], data, size );
				crc32 = GetCrcCheck( &s_Buf[DATA_INDEX], size );
				memcpy( &s_Buf[DATA_INDEX+size], &crc32, 4 );
			}
			break;

	  case SET_ONE_POINT:
			if( ( size != sizeof(SkyWayPointStruct) ) || (NULL == data) )
			{
			  flag = FALSE;
			}
			else
			{
			  memcpy( &tempPoint, data, size );
				len = size + BODY_SIZE_FOR_FC + 17 + 4 + 2;//+2 存储航点编号
				s_Buf[FLAG_INDEX]      = FLAG_ACK;
				
				s_Buf[BODY_CMD_INDEX]  = mode;
				memcpy( &s_Buf[DATA_INDEX], &tempPoint.id, 2);
				memcpy( &s_Buf[DATA_INDEX+2], data, size );
				crc32 = GetCrcCheck( &s_Buf[DATA_INDEX+2], size );
				memcpy( &s_Buf[DATA_INDEX+size+2], &crc32, 4 );
			}
			break;

		case POINT_TOTAL_NUM:
		case GET_POS_INFO:
		case NORMAL_TRACK:
		case SPECIAL_TRACK:
        case TESTDATA_SAVE:
			if( (NULL == data) || (size > (COM_SLAVER_TX_BUF_SIZE_MAX-BODY_SIZE_FOR_FC - 17) ) )
			{
			  flag = FALSE;
			}
			else
			{
			  len = size + BODY_SIZE_FOR_FC + 17;
				if( (SPECIAL_TRACK == mode) || (NORMAL_TRACK == mode) )
				{
				  s_Buf[FLAG_INDEX]      = FLAG_NACK;
				}
				else
				{
			    s_Buf[FLAG_INDEX]      = FLAG_ACK;
				}
			  s_Buf[BODY_CMD_INDEX]  = mode;
			  memcpy( &s_Buf[DATA_INDEX], data, size );
			}
			break;

	  case GET_POINT_TOTAL_NUM:
	  case GET_POINT_INFO:
		case GET_POS_TOTAL_NUM:
			if(size > (COM_SLAVER_TX_BUF_SIZE_MAX-BODY_SIZE_FOR_FC - 17) )
			{
			  flag = FALSE;
			}
			else
			{
   			len = size + BODY_SIZE_FOR_FC + 17;
   			s_Buf[FLAG_INDEX]      = FLAG_ACK;
   			s_Buf[BODY_CMD_INDEX]  = mode;
   			if( size && (NULL != data))
   			{
   			  memcpy( &s_Buf[DATA_INDEX], data, size );
   			}
			}
			break;

		case ONE_POS_DATA_SIZE:
			len = 17+4;
			memcpy( &s_Buf[LEN_INDEX],    &len,  2 );

			u16Temp = size+4+4;//$POS + crc32 的空间
			memcpy( &s_Buf[DATA_INDEX], &u16Temp, 2 );
			
			s_Buf[FLAG_INDEX]      = FLAG_ACK;
			s_Buf[SUB_CMD_INDEX]   = SUB_FC;
			s_Buf[BODY_CMD_INDEX]  = BODY_CMD_PHOTO_SIZE;
			break;
			
		case POS_DATA:
      if( ( size != sizeof(PhotoDataStruct) ) || (NULL == data) )
			{
			  flag = FALSE;
			}
			else
			{
				len = size + BODY_SIZE_FOR_FC + 17 + 8;
				s_Buf[FLAG_INDEX]      = FLAG_ACK;
				
				s_Buf[BODY_CMD_INDEX]  = BODY_CMD_PHOTO_INFO;
				memcpy( &s_Buf[DATA_INDEX],   "$POS", 4    );
				memcpy( &s_Buf[DATA_INDEX+4], data,   size );
				crc32 = GetCrcCheck( &s_Buf[DATA_INDEX], size+4 );
				memcpy( &s_Buf[DATA_INDEX+size+4], &crc32, 4 );
			}
			break;

		case CLEAR_SKYWAY_POINT:
		case CLEAR_PHOTO_INFO:
			len = BODY_SIZE_FOR_FC + 17;
   		s_Buf[FLAG_INDEX]      = FLAG_ACK;
   		s_Buf[BODY_CMD_INDEX]  = mode;
			break;

    case HEART_BEAT:
      len = BODY_SIZE_FOR_FC + 17;
   		s_Buf[FLAG_INDEX]      = FLAG_NACK;
   		s_Buf[BODY_CMD_INDEX]  = mode;
      break;
			
    default:
      flag = FALSE;
			break;
  }

	if( s_PcLockSd && (mode != PASS_THROUGH) )
  {
    flag = FALSE;
  }
	
  if(flag)
  {
    
    s_Buf[SUB_CMD_INDEX]   = SUB_FC;
		memcpy( &s_Buf[LEN_INDEX],    &len,  2 );

		crc16 = (u16)GetCrcCheck( s_Buf, HEAD_LEN - 2 );
	  memcpy( &s_Buf[CRC16_INDEX], &crc16, sizeof(u16) );
		
	  crc32 = GetCrcCheck( s_Buf, len - 4 );
	  memcpy( &s_Buf[len - 4], &crc32, sizeof(u32) );
		DrvTxToSlaver(s_Buf, len);
  }
	
	ComSlaverTxExitCritical();
}


