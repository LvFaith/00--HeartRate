
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

//Э���е�ƫ��λ��
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


//Э��������������ݴ�С
#define LEN_SIZE                  ( 2 )
#define PART_OF_HEAD_SIZE         ( 3 )//��DeviceType�� subCmd֮ǰ �Ĵ�С
#define MSG_SEQ_SIZE              ( 1 )
#define HEAD_LEN                  ( 13 )

#define UPDATE_PACK_ID_SIZE       ( 4 )

#define BODY_SIZE_FOR_FC          ( 2 )//�ɿ�SD��˽�����Э��ռ��Data��Ĵ�С


//������
#define NO_ERR                    ( 0x00 )
#define NORMAL_ERR                ( 0x01 )
#define ERASE_COUNT_ERR           ( 0x0F )
#define BIN_INFO_ERR              ( 0x10 )
#define FLASH_OVER_FLOW           ( 0x0C )//flash������Χ
#define REV_PACK_ID_ERR           ( 0x09 )//�������İ��Ŵ��ڵȴ��İ���


//MasCmd
#define MAS_CMD_UPDATE            ( 0x0C )
#define MAS_CMD_MEMORIZER         ( 0x0E )//�洢�����
#define MAS_CMD_FC                ( 0xFC )//�ɿ����


#define SUB_CMD_START_BOOT        ( 0x01 )
#define SUB_CMD_UPDATA            ( 0x02 )
#define SUB_CMD_UPDATA_END        ( 0x03 )//û�����������
#define SUB_CMD_JUMP_APP          ( 0x04 )
#define SUB_CMD_NO_APP            ( 0x05 )
#define SUB_CMD_VERIFY            ( 0x06 )

#define SUB_CMD_MEM_FORMAT        ( 0x01 )// �洢�����Э���еĸ�ʽ��subcmd
#define SUB_CMD_MEM_RELEASE       ( 0x03 )// �洢�����Э���е��ͷŹ����subcmd

#define SUB_CMD_PRODUCION_TEST    ( 0xF0 )//������ص�
#define SUB_FC                    ( 0xFC )//�ɿ���ص�˽�д洢�����Э��

#define BODY_CMD_NORMAL_TRACK           ( 0x01 )
#define BODY_CMD_SPECIAL_TRACK          ( 0x02 )
#define BODY_CMD_POINT_SIZE             ( 0x03 )
#define BODY_CMD_POINT_INFO             ( 0x04 )
#define BODY_CMD_POINT_NUM              ( 0x05 )//��������
#define BODY_CMD_PHOTO_SIZE             ( 0x06 )
#define BODY_CMD_PHOTO_INFO             ( 0x07 )
#define BODY_CMD_GET_POINT_TOTAL_NUM    ( 0x08 )//��ȡ��������
#define BODY_CMD_GET_POINT_INFO         ( 0x09 )//��ȡ������Ϣ
#define BODY_CMD_CLEAR_POINT            ( 0x0A )//���������Ϣ
#define BODY_CMD_CLEAR_PHOTO            ( 0x0B )//���pos��Ϣ
#define BODY_CMD_GET_PHOTO_TOTAL_NUM    ( 0x0C )//��ȡpos������
#define BODY_CMD_GET_PHOTO_INFO         ( 0x0D )//��ȡpos����Ϣ
#define BODY_CMD_SET_ONE_POINT          ( 0x0E )//���õ�������

#define BODY_CMD_GET_FILE_TOTAL_NUM     ( 0x21 )//��ȡ�ļ�������һ����λ������
#define BODY_CMD_GET_ONE_FILE_HEAD      ( 0x22 )//��ȡ�ļ�ͷ��Ϣ
#define BODY_CMD_GET_TRACK_FILE_HEAD    ( 0x23 )//��ȡ�����ļ�ͷ��Ϣ�����ٺ���ͨ��һ��

#define BODY_CMD_SAVE_PRODUCTION_TEST   ( 0x0F )//�����������
#define BODY_CMD_GET_FILE_PACK          ( 0x31 )//��ȡ�ļ�����



#define FLAG_ACK       (0x08)//���������ҪӦ��
#define FLAG_NACK      (0x00)//�����������ҪӦ��
#define FLAG_RETURN    (0x10)//Ӧ���







/*============================ TYPES =========================================*/


typedef struct
{
	u8 DeviceType;
	u8 DeviceId;
	u8 MasCmd;
}PartOfHeadStruct;

typedef enum
{
  USB_PASS_THROUGH               = 0,    //͸��
  BODY_CMD_GET_UID               = 0x80, //��ȡUID
  BODY_CMD_START_INTERFACE_TEST  = 0x81, //��ʼ��������
  BODY_CMD_GET_DEVICE_INFO       = 0x84, //��ȡEEPROM����豸��Ϣ
  BODY_CMD_GET_PRODUCTION_RESULT = 0x85, //��ȡ����������
  BODY_CMD_START_BURN_IN_TEST    = 0x86, //��ʼ����
  BODY_CMD_STOP_BURN_IN_TEST     = 0x87, //ֹͣ����
  BODY_CMD_SET_SN                = 0x90, //дSN��
  BODY_CMD_SET_PRODUCTION_TIME   = 0x91, //д��������
  BODY_CMD_GET_RSA               = 0x92, //��ȡ��Ȩ��Ϣ
  BODY_CMD_SET_RSA               = 0x93, //������Ȩ��Ϣ

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

