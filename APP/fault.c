//#include "app.h"
#include "fault.h"
#include "fd.h"
#include "pfh_pump.h"
STR_FAULT gstFault;
STR_FAULT gstFaultMsk;//FT_01_20



  void SysFaultSet(void)
{ 
    gstFault.FAT_FD_OFFLINE = FdIsFaultOffline(); //探测器离线    
    gstFault.FAT_FD_SENSOR = FdIsFaultSensor();    
    gstFault.FAT_FD_HARDWARE = FdIsFaultHardware();
    gstFault.FAT_PFH_USED = PfhPumpStatus() == FIRE_NOT_OPEN ? 0 : 1; 

//	printf("gstFault.FAT_FD_SENSOR   %d \n",gstFault.FAT_FD_SENSOR);
//	printf("gstFault.FAT_FD_HARDWARE %d \n",gstFault.FAT_FD_HARDWARE);

}

void FaultSetLevelLow(BOOL bFault) 
{
    gstFault.FAT_LEVEL = bFault;
//	printf("gstFault.FAT_LEVEL %d \n",gstFault.FAT_LEVEL);	
}

void FaultSetBatLow(BOOL bFault) 
{
    gstFault.FAT_VOLLOW = bFault;   
}


void FaultSetVulOpen(BOOL bFault) 
{
    gstFault.FAT_VAL_OPENCIR = bFault;
	printf("gstFault.FAT_ValOpen %d \n",gstFault.FAT_VAL_OPENCIR);	
}

void FaultSetFDShort(BOOL bFault) 
{
    gstFault.FAT_FD_SHORTCIR = bFault;
	//printf("gstFault.FAT_FDShort %d \n",gstFault.FAT_FDShort);	
}




void SysFaultInit(void)//FT_01_20 //初始化故障 
{
    gstFault.usFault = 0;
    gstFaultMsk.usFault = 0xFFFF;
    gstFaultMsk.FAT_LEVEL = 0; //屏蔽液位过低报警指示
    gstFaultMsk.FAT_VOLLOW = 0; //FT_01_13 屏蔽电量过低报警指示

}




UINT8 FaultIsBatLow(void)
{
    return gstFault.FAT_VOLLOW;
}


UINT8 FaultIsLevelLow(void)
{
    return gstFault.FAT_LEVEL;
}


UINT8 FaultIsValOpenCircuit(void)
{
    return gstFault.FAT_VAL_OPENCIR;
}



UINT8 FaultIsFDShortCircuit(void)
{
    return gstFault.FAT_FD_SHORTCIR;

}



