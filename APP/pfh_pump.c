#include "pfh_pump.h"
#include "appdef.h"
/*
    @Module: PfhPump
    @Verion: 0.1 
 */
static UINT8 sg_ucPvCurStatus = PV_IDLE;
static UINT8 sg_ucPvNextStatus = PV_IDLE;
static STR_PV_RUN_PARA stPvRunPara;





//液位检测函数
UINT8 LiquidLevelCheck(void)
{
 //   return (0 == DioReadChannel(LEVER_CHECK) ? 1 : 0);
}


void PvTypeSet(UINT8 ucType)
{
    switch(ucType) {
    case PV3_FIRE_TYPE:
        stPvRunPara.ucPvModel = 0;
        break;     
    case PV6_FIRE_TYPE:
        stPvRunPara.ucPvModel = 1;
        break;
    case PV8_FIRE_TYPE:
        stPvRunPara.ucPvModel = 2;
        break;
    case PVA_FIRE_TYPE:
        stPvRunPara.ucPvModel = 3;
        break;
    default :
        stPvRunPara.ucPvModel = 3;
        break;
     }
}

void PvParaRead(void)
{                
//    (void)NVMRead(DINDEX_PVPARA, &stPvRunPara, sizeof(stPvRunPara));    
}

void PvParaSave(void)
{
//    stPvRunPara.ucCrc = CjCrc32CalBuffer(&stPvRunPara,sizeof(stPvRunPara)-4); 
//    (void)NVMWrite(DINDEX_PVPARA, &stPvRunPara, sizeof(stPvRunPara));
}

void PfhPumpRequest(UINT8 ucReqType) 
{
    if (stPvRunPara.ucFireOpen == RESET) 
	{
		printf("PumpRequest ready open fire\n");
        sg_ucPvNextStatus = VALVE_OPEN;
        stPvRunPara.ucReqType = ucReqType;
        stPvRunPara.uwTimeStamp = g_uwTimeStamp; 
        stPvRunPara.ucFireOpen = SET;
        stPvRunPara.ucLevel = LiquidLevelCheck();    
        PvParaSave();
    } 
	else 
	{
        ;
    }
}

void PfhPumpParaReset(void)
{
    (void)c_memset(&stPvRunPara,0,sizeof(stPvRunPara));    
}

void PfhPumpInit(void) 
{
    UINT32 ucCrc;
    PumpInit();
    PumpValveInit();
	
//    PvParaRead();	
//    ucCrc = CjCrc32CalBuffer(&stPvRunPara,sizeof(stPvRunPara)-4);   
//    if(ucCrc != stPvRunPara.ucCrc) {
//        PfhPumpParaReset();
//        PvParaSave();
//    } 
	
    sg_ucPvCurStatus = PV_IDLE;
	
    if (stPvRunPara.ucFireOpen == SET && stPvRunPara.ucRunEnd != SET) 
	{
        sg_ucPvCurStatus = PV_IDLE;
        sg_ucPvNextStatus = PUMP_OPEN;
    } 
	else 
	{
        sg_ucPvCurStatus = PV_IDLE;
        sg_ucPvNextStatus = PV_IDLE;
    }
}

//灭火器启动状态
UINT8 PfhPumpStatus(void)
{
    if(stPvRunPara.ucFireOpen == RESET) {
        return FIRE_NOT_OPEN;   //未启动
    } else if(stPvRunPara.ucFireOpen == SET && stPvRunPara.ucRunEnd == RESET) {
        return FIRE_OPEN_RUNING;//正在启动
    } else if(stPvRunPara.ucFireOpen == SET && stPvRunPara.ucRunEnd == SET) {
        return FIRE_OPEN_SUC;   //启动成功
    } else {
        return FIRE_OPEN_FAIL;  //启动失败
    }
}



#define DURA_TIME 	60//s
#define PAUSE_TIME 	0
#define PERIOD_NUM  1


void PfhPumpRunProcess(void)
{
    static UINT8 runtime = 0;
    
	
	//在这里做一个测试限制
    
    if (stPvRunPara.ucFireOpen == 0 || stPvRunPara.ucRunEnd == 1) 
	{  //灭火器无需启动或启动结束
        //printf("fire not open\n");
		return;
    } 
	else 
	{
        if (runtime != 0) 
		{
#ifdef DBUG_PHF_PUMP
            (void)printf("\tRUNTM:%d\n", runtime);
#endif
            runtime--;
        }
        if (runtime == 0) 
		{
#ifdef DBUG_PHF_PUMP
            (void)printf("PvCur:%d,PvNex:%d\n", sg_ucPvCurStatus, sg_ucPvNextStatus); 
#endif
            sg_ucPvCurStatus = sg_ucPvNextStatus; 
        } 
		else 
		{
            return;    
        }
    }
   	
	switch(sg_ucPvCurStatus) 
	{
		case VALVE_OPEN :  //先开阀    
			
		printf("open value\n");
			ValveOpen();			
			sg_ucPvNextStatus = PUMP_OPEN;//阀开启
			runtime = VALE_OPEN_SEC;   //等待一会     	 
			break;
		
		case PUMP_OPEN: 	//再开泵
			
printf("Open PUMP\n");			
            PumpOpen();                           			                       
			sg_ucPvNextStatus = PUMP_CLOSE;
			runtime = DURA_TIME;//打开十秒
			break; 
		
		case PUMP_CLOSE:  //泵关闭
printf("Close PUMP\n");
			PumpClose();
			runtime = PAUSE_TIME;//关闭运行时间为0
			stPvRunPara.ucRunPeriod++;
		
			if (stPvRunPara.ucRunPeriod >= PERIOD_NUM) //只喷一次
			{
printf("Ready to stop\n\n");
				sg_ucPvNextStatus = PV_STOP;
			} 
			else 
			{
				sg_ucPvNextStatus = PUMP_OPEN;
			}
			PvParaSave();
			PvParaRead();
			break;
			
		case PV_STOP:   //阀泵关闭 
printf("Fire open close\n");	
			ValveClose();
			stPvRunPara.ucRunEnd = SET;
			sg_ucPvNextStatus = PV_STOP; 
			PvParaSave();
			break;
    }         
}

void PfhPumpClr(void) 
{
    stPvRunPara.ucFireOpen 		= RESET;
    stPvRunPara.ucLevel 		= RESET;
    stPvRunPara.ucPresOkNum 	= RESET;
    stPvRunPara.ucRunEnd 		= RESET;
    stPvRunPara.ucRunPeriod 	= RESET;
    PvParaSave();
    sg_ucPvCurStatus 			= PV_IDLE;
    sg_ucPvNextStatus 			= PV_IDLE; 

	ValveClose();
	PumpClose();
}

const STR_PV_RUN_PARA * PfhPumpGetPara(void) 
{
    return (const STR_PV_RUN_PARA *)&stPvRunPara;    
}
