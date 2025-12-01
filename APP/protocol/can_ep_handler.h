#ifndef _CAN_EP_HANDLER_H_
#define _CAN_EP_HANDLER_H_

#include "stdtype.h"

#define CAN_SEND_MS 50

#define CAN_MSG_SEND_ENABLE				1
#define CAN_MSG_SEND_DISABLE			0

typedef enum{
	CAN_SEND_B0_MSG = 0,
	CAN_SEND_MAX
}CAN_SEND_NUM;
/***********************************
 * Typedef
 ***********************************/
typedef struct {
    int  sndflg;
	int  cmd;
    int  interval;
    int  ttl;
} TmrSndStr; /* 定时发送结构体 */
extern TmrSndStr TmrSnd[];

typedef struct {
    void (*pCanEpTmrSend)(UINT8 ucCommand, STR_CAN_STD_TYPE *canmsg);
    void (*pCanEpHandler)(STR_CAN_STD_TYPE *canf);
    void (*pCanEpIntervalSet)(void); //20211202_1 外CAN发送间隔设置
    UINT32 uwCanAcpId;
    UINT32 uwCanMskId;
} STR_CAN_EP_HANDLER;

extern STR_CAN_EP_HANDLER stCanEpHandler;


extern void CanEpHandlerInit(UINT8 ucEpProtocol);
extern void CanExpHandler(STR_CAN_STD_TYPE *canf);
extern void MsgSendHandle(void);

#endif



