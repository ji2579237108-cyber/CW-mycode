#include "Appconf.h"
#include "kfifo_sp.h"
#include "can_ep_handler.h"
#include "can_ip_handler.h"
#include "MidTimer.h"
#include "fault.h"	
#include "selfdiag.h"
#include "pfh_pump.h"
#include "fd.h"
#include "para.h"
#include "valvectl.h"
#include "auto_addressing_token_master.h"
#include "systick.h"
#include "message.h"
#include "DiCtrl.h"
#include "DoCtrl.h"
#include "LEDCtrl.h"
#include "algo.h"
#include "can_ep_handler.h"


DECLARE_KFIFO(canout_fifo, STR_CAN_STD_TYPE, 64);
const T_CW_VERSION tCwAppVersion = {1, 3, 1, 1}; // software versi    


UINT8 MessageConf(void)
{
    BOOL ucRet;
    ucRet = MessageConfigTicks(10);
    if (ucRet == FALSE) 
    {
        return 1;    
    }
    else
    {
		gucWorkMode = 0;
		(void)MessageRegister(SelfDiagMainProcess, 		50);    //自检 
		(void)MessageRegister(DiagnosticMainProcess, 	1000);  //自检结果诊断 
		(void)MessageRegister(DiCtrl_Scan, 				20);    //输入接口扫描 
		(void)MessageRegister(LedCtrl_ConditionTask, 	100);  	//LED
		(void)MessageRegister(DoCtrl_Update, 	        100);   //输出接口     
        (void)MessageRegister(ValveCtrl_Process, 		1000); 	//阀门主进程
		(void)MessageRegister(MsgSendHandle, 			1000); 	//主动上送报文
		(void)MessageRegister(doOneSecStak, 			1000); 	//1s主进程 
		
	}
	return 0;
}

void doOneSecStak(void)
{
	GetSensorValue();	//获取传感器数据
    SysFaultDiag();		//故障检测和设置
    AlgoProcessing();	//报警判断
	static int aaa = 0;
	aaa++;
	SEGGER_RTT_printf(0,"%d\r\n",aaa);
}

void CanOutFifoInit()
{
	INIT_KFIFO(canout_fifo);
}

unsigned int CanInPutTask(STR_CAN_STD_TYPE *canf)
{
	canf->Channel = CAN1_CHANEL; //CAN1
	kfifo_in(&canout_fifo, canf, 1);
    return 1;
}


#if 0
void repost()  
{
	STR_CAN_STD_TYPE TxCanf;
	if (kfifo_out(&canout_fifo, &TxCanf, 1) != 0) {
		if(TxCanf.Channel == 0)
		{
			ExCan0Send(&TxCanf);
		} 
		else 
		{
			InCan1Send(&TxCanf);
		}
	}
}


void BroadCast(void){
    
      if(BROADCAST == 0x01)
      {
            static STR_CAN_STD_TYPE ExstIpCanf;
            ExstIpCanf.canid.exdid = 0x18BCE900 + pSysPara->ucDeviceID;
            ExstIpCanf.len = 8;
            ExstIpCanf.exd = 1;
            ExstIpCanf.data[0] = Fdid+j;
            ExstIpCanf.data[1] = gstFdDataBuffer[Fdid+j].ucTemp1;
            ExstIpCanf.data[2] = gstFdDataBuffer[Fdid+j].ucTemp2;
            ExstIpCanf.data[3] = gstFdDataBuffer[Fdid+j].ucCo >> 8;
            ExstIpCanf.data[4] = gstFdDataBuffer[Fdid+j].ucCo & 0xFF;
            ExstIpCanf.data[5] = gstFdDataBuffer[Fdid+j].ucVoc >> 8 ;
            ExstIpCanf.data[6] = gstFdDataBuffer[Fdid+j].ucVoc & 0xFF;
            ExstIpCanf.data[7] = (gstFdDataBuffer[Fdid+j].ucAlarm & 0x0F) << 4|(gstFdDataBuffer[Fdid+j].ucFault & 0x0F);
            ExCan0Send(&ExstIpCanf);      //外Can发送s 
              
      }
}
#endif


