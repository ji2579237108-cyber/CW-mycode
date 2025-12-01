#include "pump.h"
//#include "serv.h"
#include "MidDio.h"

static UINT8 ucPumpStatus = 0;
static UINT8 ucValveStatus = 0;

  void PumpInit(void) 
{
    PumpClose();
}

void PumpOpen(void) 
{
 //   DioWriteChannel(PUMP_POW,SET);   
    //TODO:外水泵继电器 DoSet(DO_EX_PUMP);
    ucPumpStatus = 1;    
}
void PumpClose(void) 
{
//    DioWriteChannel(PUMP_POW,RESET);       
    //TODO:外水泵继电器 DoReset(DO_EX_PUMP);
    ucPumpStatus = 0;    
}

UINT8 PumpStatus(void)
{
    return ucPumpStatus; 
}

void PumpValveInit(void) 
{
    ValveClose();
}

void ValveOpen(void) 
{    
    DioWriteChannel(QFFK_ON,SET);
    ucValveStatus = 1;
}

void ValveClose(void) 
{
    DioWriteChannel(QFFK_OFF,1);
    ucValveStatus = 0;
}

UINT8 PfhValveStatus(void) 
{
    return ucValveStatus;
}
