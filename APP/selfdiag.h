#ifndef _DIAGNOSTIC_H_
#define _DIAGNOSTIC_H_

#include "selfdiag_conf.h"

#include "AdcCfg.h"
#include "MidAdc.h" 
typedef enum DETCT_RESULT    
{
    DETCT_FAILED, //Ê§°Ü
    DETCT_PREFAILED, //Ô¤Ê§°Ü
    DETCT_NOTCHECK, //Î´¼ì²â
    DETCT_PREPASSED, //Ô¤Í¨¹ý
    DETCT_PASSED //Í¨¹ý
} ENU_DETCT_RESULT; //¼ì²â½á¹û

typedef void(*pfnDiagFunc)(void);
typedef ENU_DETCT_RESULT(*P_FUNC_DETC)(void);

void SelfDiagInit(void);
void DiagFastMainProcess(void);
void SelfDiagMainProcess(void);
void CalcFaultDetctCounter(T_DETCT_COUNT *ptCounter, BOOL bFail);
ENU_DETCT_RESULT DetctFunction(T_DETCT_COUNT tCount, T_DETCT_COUNT tPassedLimit, T_DETCT_COUNT tFailedLimit);
void SelfDiagConfirm(void) ;
void SelfDiagFaultClear(void);
void DiagnosticMainProcess(void); 


#include "pwr_node.h"
#include "ntc_node.h"

#endif /* _DIAGNOSTIC_H_ */ 
