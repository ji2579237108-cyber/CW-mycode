#include "selfdiag.h"
#include "AdcCfg.h"
#include "c_mem.h"
#include "appconf.h"
#include "nvm.h"
#include "selfchk_node.h"
//#include "serv.h"
#include "can_ep_handler.h"
#include "c_mem.h"

//-----------------------------------------------------------------------
//外CAN自检
//-----------------------------------------------------------------------
static T_DETCT_COUNT tExCanCounter = 0;  //外CAN自检
void SelfChkExCanCtr(void)
{
    if (CanEpGetRecvCount() == 0) 
		{
        CalcFaultDetctCounter(&tExCanCounter, TRUE);
    }
}
ENU_DETCT_RESULT SelfChkExCan(void)
{    
    return DetctFunction(tExCanCounter, EXCAN_PASS_LIMT, EXCAN_FAIL_LIMT); 
} 


//-----------------------------------------------------------------------
//球阀自检
//-----------------------------------------------------------------------
static T_DETCT_COUNT tBallValveCounter = 0;  //球阀
void SelfBallValveCtr(void)
{
    if (CanEpGetRecvCount() == 0) 
		{
        CalcFaultDetctCounter(&tBallValveCounter, TRUE);
    }
}
ENU_DETCT_RESULT SelfBallValveCounterCan(void)
{    
    return DetctFunction(tBallValveCounter, BALLVALVE_PASS_LIMT, BALLVALVE_FAIL_LIMT); 
} 


//-----------------------------------------------------------------------
//NVM自检 暂时取消
//-----------------------------------------------------------------------

UINT8 NvmCheck(UINT8 index) 
{
    int i;
    typedef UINT8 T_NVM_BUFFER[16];
    T_NVM_BUFFER tBuffer1, tBuffer2; 
    (void)NVMRead(index, tBuffer1, sizeof(T_NVM_BUFFER));
    for (i = 0; i < sizeof(T_NVM_BUFFER); i++) {
        tBuffer2[i] = (UINT8)~tBuffer1[i];    
    }
    (void)NVMWrite(index, tBuffer2, sizeof(T_NVM_BUFFER));
    (void)NVMRead(index, tBuffer1, sizeof(T_NVM_BUFFER));
    if (c_memcmp(tBuffer1, tBuffer2, sizeof(T_NVM_BUFFER)) == 0) {
        return 0;
    } else {
        return 1;
    }    
}





