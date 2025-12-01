#include "can_ep_17_8fh.h"

#include "can_ep_handler.h"
#include "CanTest.h"
#include "Appconf.h"


#define EP_CMD_FF 0xFFu //实现18FF301D
#define EP_ACP_ID 0x18CAFFF6
#define EX_CAN_SEND(data)   ExCan0Send(data)
#define EP_MSK_ID 0


/********************************************
函数功能：组帧发送报文
********************************************/
static void Can_Ep_CAh_Handler (STR_CAN_STD_TYPE *canf)
{
	CanInPutTask(canf);
}
static void Can_Ep_01h_TmrSend(UINT8 ucCommand)
{
	
}


static void Can_Ep_01h_Handler(STR_CAN_STD_TYPE *canf)  
{                  
    if (canf->J1939PRDP == 0x18 && \
				canf->J1939PF == 0xCA && \
				canf->J1939SA == 0xF6) {
					
       (void)Can_Ep_CAh_Handler(canf); 
    }
		
		switch(canf->J1939PF)
		{
			case 0xCA:
				(void)Can_Ep_CAh_Handler(canf); 
			break;
			
			case 0xA9:
				(void)Can_Ep_CAh_Handler(canf); 
			break;
		}
			
}


static const STR_CAN_TMR_SEND scg_stCanTmrSendArray[] = {
	//
    { EP_CMD_FF, 1000u, 100u },         
};

void Can_Ep_17_8Fh_Init(void)
{   
    (void)CanEpTrmSendAdd(scg_stCanTmrSendArray, ARRAY_SIZE(scg_stCanTmrSendArray));
    stCanEpHandler.pCanEpTmrSend = NULL;  //发送外部报文
    stCanEpHandler.pCanEpHandler = Can_Ep_01h_Handler;  //接受外部报文  
    stCanEpHandler.uwCanAcpId   = EP_ACP_ID;                      //掩码
    stCanEpHandler.uwCanMskId   = EP_MSK_ID;
}
