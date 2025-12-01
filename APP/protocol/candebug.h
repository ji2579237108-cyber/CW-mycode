#ifndef _CAN_DEBUG_H_
#define _CAN_DEBUG_H_

#include "stdtype.h"
#include "stdint.h"


#define DEBUG_ID 0xE9
#define DEBUG_MSG_ID 0xFF


#define EXCAN_MODE 0xA
#define INCAN_MODE 0xB

//参数配置错误码
#define PARA_CONFIG_OK 				0x00U
#define PARA_CONFIG_LEN_ERROR		0x13U
#define PARA_CONFIG_DID_ERROR   	0x11U
#define PARA_CONFIG_VALUE_WRONGFUL	0x31U

#define PARA_CFG_DID_ID					0xFF1EU
#define PARA_CFG_DID_CAN_BAUD			0xFF00U
#define PARA_CFG_DID_BAT_NUM			0x0195U


typedef enum{
	DID_ID = 0,			
    DID_CAN_BAUD	,	
	DID_BAT_NUM,		
	
}ENU_PARA_CFG;


typedef struct PARA_RETRUN
{
	UINT8  	Result;
	UINT32	ResDat;
	UINT8 	ResLen;
}PARA_RETURN_STR;

typedef struct {
    UINT16 	ParaCfgDid; //参数DID
	UINT8 	DataLen; //数据长度
	PARA_RETURN_STR (*pCfgHandle)(UINT8 cmd, UINT8 Datelen, STR_CAN_STD_TYPE *ptr);
} STR_DEBUG_PARA_CFG; //参数配置	

extern UINT8 g_ucCanChannel;


extern BOOL CanDebugMode(void);
extern BOOL CanDebugHandler(STR_CAN_STD_TYPE *ptr);
extern BOOL isDebugTable(uint8_t value);

void _doD5SetHandle(STR_CAN_STD_TYPE *pCanf);
/*SWP-S2 End*/
   

#endif
