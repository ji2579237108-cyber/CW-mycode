#include "systick.h"
#include "CWDrv.h"
#include "MidWdg.h"
#include "MidAdc.h"
#include "MidUsart.h"
#include "MidTimer.h"
#include "MidDio.h"
#include "MidCan.h"
#include "canbuffer.h"
#include "can_ip_handler.h"
#include "message.h"
#include "Appconf.h"
#include "can_ep_handler.h"
#include "para.h"
#include "nvm.h"
#include "fd.h"
#include "auto_addressing_token_master.h"
#include "DiCtrl.h"
#include "DoCtrl.h"
#include "RX8010.h"
#include "time.h"
#include "BatteryManage.h"
#include "BM22S2301.h"

#define SYS_CLOCK  120
#define CAN_BAUD_250 250

int main(void)
{
	__disable_irq();
    
    static STR_CAN_STD_TYPE stCanf;
	vdelay_init(SYS_CLOCK);
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0X6000);
	DioInit();
	NvmConfig();
    Usart1_init();
    CanOutFifoInit();
    Timer1_Init();
	AdcInit();
	CanInit(CAN_BAUD_250, CAN_BAUD_250);
	ParaRead();
	DiCtrl_Init();
	DoCtrl_Init();
	RX8010_Init();
	ChargeInit();
	CanEpHandlerInit(pSysPara->ucEPType);//todo 协议，发报文
	MessageConf();
    SetTime();
	__enable_irq();
	DEBUG_LOG_INFO(2,"Verson:%d-%d-%d-%d\r\n\n",tCwAppVersion[0],tCwAppVersion[1],tCwAppVersion[2],tCwAppVersion[3])
	while(1)
	{    
		Feed_HardDog();
//		while(!CanBufferIsEmpty()) 			//内Can
//		{
//			CanBufferGetFirst(&stCanf);
//			CanIpHandler(&stCanf);  
//		}
		while (Get_Rx_Flag())
		{
			BM22S2301_Handle(); //BM22S2301处理函数
		}
		while(!OtCanBufferIsEmpty())		//外Can
		{
			OtCanBufferGetFirst(&stCanf);					
			CanExpHandler(&stCanf);  
		}
		MessageProcess();
	}			
}

