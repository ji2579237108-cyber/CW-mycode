#include "para.h"
#include "can_ep_handler.h"
#include "candebug.h"
#include "SEGGER_RTT.h"
#include "fd.h"
#include "can_ep_01h.h" 
#include "canboot.h"
#include "MidCan.h"
#include "auto_addressing_token_master.h"


//static STR_CAN_STD_TYPE stResetCanf;
//static STR_CAN_STD_TYPE* const pstResetCanf = &stResetCanf;
UINT8 BROADCAST = 0;
   
TmrSndStr TmrSnd[CAN_SEND_MAX] = { 0 };

STR_CAN_EP_HANDLER stCanEpHandler; //设置不同外部规约的发送接收命令


/********************************************
函数功能：在主函数里，调用，用于初始化外部规约
********************************************/
void CanEpHandlerInit(UINT8 ucEpProtocol) 
{
    stCanEpHandler.pCanEpIntervalSet = NULL; //20211202 统一初始化为NULL，具体协议实现该接口
    switch (ucEpProtocol) { 
        case 0x01U: Can_Ep_01h_Init(); break; 
        default   :Can_Ep_01h_Init(); break;
    }
}


/********************************************
函数功能：外部报文的接收处理
********************************************/
void CanExpHandler(STR_CAN_STD_TYPE *canf)
{
    BOOL bRet;   
    g_ucCanChannel = EXCAN_MODE;
    bRet = CanDebugHandler(canf); 		if (bRet == TRUE) return;    // DEBUG 
    bRet = CanBootHandler(canf); 		if (bRet == TRUE) return;    // BOOT
    stCanEpHandler.pCanEpHandler(canf);                              // 开80A球阀或透传07A报文
}


void MsgSendHandle(void)
{
	UINT8 i;	
	static STR_CAN_STD_TYPE canmsg = { 0 };

	for (i = 0; i < CAN_SEND_MAX; i++) {
		if (TmrSnd[i].sndflg == CAN_MSG_SEND_ENABLE){
				if(TmrSnd[i].cmd > 0 && --TmrSnd[i].ttl <= 0) {
				stCanEpHandler.pCanEpTmrSend((UINT8)TmrSnd[i].cmd, &canmsg);
				ExCan0Send(&canmsg);
	            delay_ms(3);	
				TmrSnd[i].ttl = TmrSnd[i].interval;
			}
		}
	}
}


