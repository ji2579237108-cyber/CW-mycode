#include "can_ep_01h.h"
#include "can_ep_handler.h"
#include "algo.h"
#include "Appconf.h"
#include "para.h"
#include "valvectl.h"
#include "para.h"
#include "MidDio.h"
#include "MidCan.h"

static STR_CAN_STD_TYPE stRspCanf;
static STR_CAN_STD_TYPE* const pstRspCanf = &stRspCanf;


static void Can_Ep_TmrSend(UINT8 ucCommand, STR_CAN_STD_TYPE *canmsg)
{ 
	canmsg->CANEXDID = 0x1800F600 + ((UINT32)ucCommand << 16) + (UINT32)pSysPara->ucDeviceID;
    canmsg->rtr = 0;
    canmsg->exd = 1;
	if (0xB0 == ucCommand) 
    {
        canmsg->len = 8; 
		canmsg->data[0] = (uint8_t)(SensorValue.ntc1 + 40);
        canmsg->data[1] = (uint8_t)(SensorValue.ntc2 + 40);
		canmsg->data[2] = (uint8_t)(SensorValue.SmokeFlag);
		canmsg->data[3] = (uint8_t)((SensorValue.SmokeValueA >> 8) & 0xFF);
		canmsg->data[4] = (uint8_t)(SensorValue.SmokeValueA);
		canmsg->data[5] = (uint8_t)((SensorValue.SmokeValueB >> 8) & 0xFF);
		canmsg->data[6] = (uint8_t)(SensorValue.SmokeValueB);
		canmsg->data[7] = (uint8_t)(FaultResult.all | (SysLevel << 5));//高3位为报警状态
	}
}
/********************************************
函数功能：组帧发送报文
********************************************/
static void EpTypeSendFrame(STR_CAN_STD_TYPE *canmsg) 
{    
    canmsg->rtr = 0;  
    canmsg->exd = 1;
    switch (canmsg->J1939PF) {      
    	case 0xCA:
			canmsg->len = 1;
		break;
	}
	ExCan0Send(canmsg);
	delay_ms(3);
}

static void Can_Ep_CAh_Handler (STR_CAN_STD_TYPE *canf)
{
	switch(canf->J1939PF)
	{
		//CA启阀
		case 0xCA: 
			if((1 == canf->len) && (0x01 == canf->data[0]))
			{
				ValveCtrl_SetValveType(VALVE_TYPE_CAN);
				pstRspCanf->data[0] = canf->data[0];
				EpTypeSendFrame(pstRspCanf);
			}
			else if(1 == canf->len && (0x00 == canf->data[0]))
			{
				DioWriteChannel(HSD_IN, FALSE); // 关闭灭火器
				pstRspCanf->data[0] = canf->data[0];
				EpTypeSendFrame(pstRspCanf);
			} else
			{
				/* No Action */
				;
			}
			
			break;

	}

}

static void Can_Ep_01h_Handler(STR_CAN_STD_TYPE *canf)  
{                  
    if (canf->J1939PRDP == 0x18 && canf->J1939SA == 0xF6 && \
		(canf->J1939PF == pSysPara->ucDeviceID || canf->J1939PF == 0xFF)) //支持广播
    {
		pstRspCanf->CANEXDID = canf->CANEXDID;
		pstRspCanf->J1939PS = canf->J1939SA;
		pstRspCanf->J1939SA = pSysPara->ucDeviceID;
		//拟定输出18XXF6ID
       (void)Can_Ep_CAh_Handler(canf); 
    }
}

void Can_Ep_01h_Init(void)
{   
	UINT8 i; 
    for (i = 0; i < CAN_SEND_MAX; i++)
        TmrSnd[i].cmd = -1; /* 首先设置其为无效 */  
	
    TmrSnd[CAN_SEND_B0_MSG].sndflg = CAN_MSG_SEND_ENABLE;
    TmrSnd[CAN_SEND_B0_MSG].cmd = 0xB0;
    TmrSnd[CAN_SEND_B0_MSG].interval = 10; 	//10*100ms 发送间隔
    TmrSnd[CAN_SEND_B0_MSG].ttl = 10;		//10*100ms，初始延时
    
    stCanEpHandler.pCanEpTmrSend = Can_Ep_TmrSend;  //发送外部报文
    stCanEpHandler.pCanEpHandler = Can_Ep_01h_Handler;  //接受外部报文  

}
