
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
  PASS_THROUGH = 0,    //透传
	NORMAL_TRACK,        //普通航迹
	SPECIAL_TRACK,       //高速航迹
	ONE_POINT_SIZE,    //单个航点大小
	SKYWAY_POINT,        //航点
	POINT_TOTAL_NUM,     //航点总数
	ONE_POS_DATA_SIZE,   //一个pos点的大小
  POS_DATA,            //照片信息
  GET_POINT_TOTAL_NUM, //获取总航点数
  GET_POINT_INFO,      //获取航点信息
  CLEAR_SKYWAY_POINT,  //清除航点信息
  CLEAR_PHOTO_INFO,    //清除照片信息
  GET_POS_TOTAL_NUM,   //获取pos点总数
  GET_POS_INFO,        //获取pos信息
  SET_ONE_POINT = BODY_CMD_SET_ONE_POINT,//设置单个航点,//设置一个航点，从机收到后知道是单点上传
  TESTDATA_SAVE = BODY_CMD_SAVE_PRODUCTION_TEST,

  HEART_BEAT    = 0x20,//心跳包，让从机做看门狗
	GET_FILE_NUM = BODY_CMD_GET_FILE_TOTAL_NUM, //获取总文件数
	GET_FILE_HEAD = BODY_CMD_GET_ONE_FILE_HEAD, //获取文件信息
	GET_TRACK_HEAD = BODY_CMD_GET_TRACK_FILE_HEAD,//读取航迹文件头信息，高速和普通的一起
	GET_FILE_PACK  = BODY_CMD_GET_FILE_PACK,//读取文件内容

	
}DataModeEnum;//和BODY_CMD同步值


extern void SendToSlaver(DataModeEnum mode, u8 const * data, u16 size);


#endif

