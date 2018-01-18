
/**
* @file     
* @brief    
* @version  
* @author   
* @date     2016-6-22
* @note     
*/

#ifndef DRV_COM_SLAVER_H__
#define DRV_COM_SLAVER_H__
 
/*============================ INCLUDES ======================================*/
 
#include "Config.h"
#include "BspUart.h" 
#include "CommonStream.h"
#include "DrvComPc.h"

 
/*============================ MACROS ========================================*/
#define COM_SLAVER_LEN_INDEX     (5)
#define COM_SLAVER_ID_INDEX      (7)
#define COM_SLAVER_PAYLOAD_INDEX (9)


/*============================ TYPES =========================================*/
 

 
/*============================ GLOBAL VARIABLES ==============================*/


/*============================ PROTOTYPES ====================================*/
typedef enum
{
  PASS_THROUGH = 0,    //͸��
	NORMAL_TRACK,        //��ͨ����
	SPECIAL_TRACK,       //���ٺ���
	ONE_POINT_SIZE,    //���������С
	SKYWAY_POINT,        //����
	POINT_TOTAL_NUM,     //��������
	ONE_POS_DATA_SIZE,   //һ��pos��Ĵ�С
  POS_DATA,            //��Ƭ��Ϣ
  GET_POINT_TOTAL_NUM, //��ȡ�ܺ�����
  GET_POINT_INFO,      //��ȡ������Ϣ
  CLEAR_SKYWAY_POINT,  //���������Ϣ
  CLEAR_PHOTO_INFO,    //�����Ƭ��Ϣ
  GET_POS_TOTAL_NUM,   //��ȡpos������
  GET_POS_INFO,        //��ȡpos��Ϣ
  SET_ONE_POINT = BODY_CMD_SET_ONE_POINT,//���õ�������,//����һ�����㣬�ӻ��յ���֪���ǵ����ϴ�
  TESTDATA_SAVE = BODY_CMD_SAVE_PRODUCTION_TEST,

  HEART_BEAT    = 0x20,//���������ôӻ������Ź�
	GET_FILE_NUM = BODY_CMD_GET_FILE_TOTAL_NUM, //��ȡ���ļ���
	GET_FILE_HEAD = BODY_CMD_GET_ONE_FILE_HEAD, //��ȡ�ļ���Ϣ
	GET_TRACK_HEAD = BODY_CMD_GET_TRACK_FILE_HEAD,//��ȡ�����ļ�ͷ��Ϣ�����ٺ���ͨ��һ��
	GET_FILE_PACK  = BODY_CMD_GET_FILE_PACK,//��ȡ�ļ�����

	
}DataModeEnum;//��BODY_CMDͬ��ֵ


extern void SendToSlaver(DataModeEnum mode, u8 const * data, u16 size);


#endif

