
/**
* @file     
* @brief    
* @version  
* @author   
* @date     2016-12-7
* @note     
*/

#ifndef DRV_COM_PC_H_
#define DRV_COM_PC_H_

/*============================ INCLUDES ======================================*/

#include "BspUsb.h" 

/*============================ MACROS ========================================*/
/* USER CODE BEGIN 0 */

#define DEVICE_TYPE               ( 100 )
#define DEVICE_ID                 ( 1 )

#define SLAVER_DEVICE_ID          ( 241 )


/* USER CODE END 0 */

//协议中的偏移位置
#define LEN_INDEX                 ( 1 )
#define FLAG_INDEX                ( 3 )
#define MSG_SEQ_INDEX             ( 4 )
#define DEVICE_TYPE_INDEX         ( 5 )
#define DEVICE_ID_INDEX           ( 6 )
#define MAS_CMD_INDEX             ( 7 )
#define SUB_CMD_INDEX             ( 8 )
#define SOURCE_TYPE_INDEX         ( 9 )
#define SOURCE_ID_INDEX           ( 10 )
#define CRC16_INDEX               ( 11 )
#define BODY_INDEX                ( 13 )
#define PC_DATA_INDEX             ( BODY_INDEX )
#define RETURN_ERROR_CODE_INDEX   ( BODY_INDEX )
#define BODY_CMD_INDEX            ( BODY_INDEX + 1 )
#define RETURN_DATA_INDEX         ( RETURN_ERROR_CODE_INDEX + 1 )
#define UPDATE_PACK_ID_INDEX      ( PC_DATA_INDEX )


//协议中相关区域数据大小
#define LEN_SIZE                  ( 2 )
#define PART_OF_HEAD_SIZE         ( 3 )//从DeviceType到 subCmd之前 的大小
#define MSG_SEQ_SIZE              ( 1 )
#define HEAD_LEN                  ( 13 )

#define UPDATE_PACK_ID_SIZE       ( 4 )

#define BODY_SIZE_FOR_FC          ( 2 )//飞控SD卡私有相关协议占用Data域的大小


//错误码
#define NO_ERR                    ( 0x00 )
#define NORMAL_ERR                ( 0x01 )
#define ERASE_COUNT_ERR           ( 0x0F )
#define BIN_INFO_ERR              ( 0x10 )
#define FLASH_OVER_FLOW           ( 0x0C )//flash超出范围
#define REV_PACK_ID_ERR           ( 0x09 )//发过来的包号大于等待的包号


//MasCmd
#define MAS_CMD_UPDATE            ( 0x0C )
#define MAS_CMD_MEMORIZER         ( 0x0E )//存储器相关
#define MAS_CMD_FC                ( 0xFC )//飞控相关


#define SUB_CMD_START_BOOT        ( 0x01 )
#define SUB_CMD_UPDATA            ( 0x02 )
#define SUB_CMD_UPDATA_END        ( 0x03 )//没有这条语句了
#define SUB_CMD_JUMP_APP          ( 0x04 )
#define SUB_CMD_NO_APP            ( 0x05 )
#define SUB_CMD_VERIFY            ( 0x06 )

#define SUB_CMD_MEM_FORMAT        ( 0x01 )// 存储类管理协议中的格式化subcmd
#define SUB_CMD_MEM_RELEASE       ( 0x03 )// 存储类管理协议中的释放管理的subcmd

#define SUB_CMD_PRODUCION_TEST    ( 0xF0 )//产测相关的
#define SUB_FC                    ( 0xFC )//飞控相关的私有存储类管理协议

#define BODY_CMD_NORMAL_TRACK           ( 0x01 )
#define BODY_CMD_SPECIAL_TRACK          ( 0x02 )
#define BODY_CMD_POINT_SIZE             ( 0x03 )
#define BODY_CMD_POINT_INFO             ( 0x04 )
#define BODY_CMD_POINT_NUM              ( 0x05 )//航点总数
#define BODY_CMD_PHOTO_SIZE             ( 0x06 )
#define BODY_CMD_PHOTO_INFO             ( 0x07 )
#define BODY_CMD_GET_POINT_TOTAL_NUM    ( 0x08 )//获取航点总数
#define BODY_CMD_GET_POINT_INFO         ( 0x09 )//获取航点信息
#define BODY_CMD_CLEAR_POINT            ( 0x0A )//清除航点信息
#define BODY_CMD_CLEAR_PHOTO            ( 0x0B )//清除pos信息
#define BODY_CMD_GET_PHOTO_TOTAL_NUM    ( 0x0C )//获取pos点总数
#define BODY_CMD_GET_PHOTO_INFO         ( 0x0D )//获取pos点信息
#define BODY_CMD_SET_ONE_POINT          ( 0x0E )//设置单个航点

#define BODY_CMD_GET_FILE_TOTAL_NUM     ( 0x21 )//读取文件总数，一般上位机操作
#define BODY_CMD_GET_ONE_FILE_HEAD      ( 0x22 )//读取文件头信息
#define BODY_CMD_GET_TRACK_FILE_HEAD    ( 0x23 )//读取航迹文件头信息，高速和普通的一起

#define BODY_CMD_SAVE_PRODUCTION_TEST   ( 0x0F )//保存产测数据
#define BODY_CMD_GET_FILE_PACK          ( 0x31 )//读取文件内容



#define FLAG_ACK       (0x08)//命令包，需要应答
#define FLAG_NACK      (0x00)//命令包，不需要应答
#define FLAG_RETURN    (0x10)//应答包







/*============================ TYPES =========================================*/


typedef struct
{
	u8 DeviceType;
	u8 DeviceId;
	u8 MasCmd;
}PartOfHeadStruct;

typedef enum
{
  USB_PASS_THROUGH               = 0,    //透传
  BODY_CMD_GET_UID               = 0x80, //获取UID
  BODY_CMD_START_INTERFACE_TEST  = 0x81, //开始电气测试
  BODY_CMD_GET_DEVICE_INFO       = 0x84, //获取EEPROM里的设备信息
  BODY_CMD_GET_PRODUCTION_RESULT = 0x85, //获取产测结果数据
  BODY_CMD_START_BURN_IN_TEST    = 0x86, //开始拷机
  BODY_CMD_STOP_BURN_IN_TEST     = 0x87, //停止拷机
  BODY_CMD_SET_SN                = 0x90, //写SN号
  BODY_CMD_SET_PRODUCTION_TIME   = 0x91, //写生产日期
  BODY_CMD_GET_RSA               = 0x92, //获取授权信息
  BODY_CMD_SET_RSA               = 0x93, //设置授权信息

  BODY_CMD_RETURN_INTERFACE_TEST = 0xA0,
}ComPcModeEnum;

typedef struct
{
  ComPcModeEnum mode;
  u8 type;
  u8 id;
  u8 errCode;
}ComPcModeStruct;


/*============================ GLOBAL VARIABLES ==============================*/
extern u8 g_DeviceInfo[176];
extern u8 g_DeviceInfoSet[176];
extern u8 g_RsaInfo[256];
extern u8 g_RsaInfoSet[256];
extern u8 g_EnableFunc[4];
extern u8 g_Sn[64];
extern u8 g_SetSnFlag;
extern u8 g_ProductionTime[16];
extern u8 g_SetProductionTimeFlag;

extern u8 g_ProductionResult[226];
extern u8 g_SetProductionResultFlag;

extern u8 g_WriteDeviceAndRsaInfo;
/*============================ PROTOTYPES ====================================*/
extern void RsaDecrypt( int * enableFunc, u8 * uidBuf );
extern void SendToPc(ComPcModeStruct mode, u8 const * data, u16 size);
  
/*============================ TEST ==========================================*/



#endif

