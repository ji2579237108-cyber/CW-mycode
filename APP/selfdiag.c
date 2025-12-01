#include "selfdiag.h"
#include "AdcCfg.h"

typedef struct DIAG_NODE {
    pfnDiagFunc pDiagFunc; //诊断函数
    UINT8 ucPeriod; //诊断运行间隔    
} STR_DIAG_NODE; //诊断点结构体
#define DIAG_SLOW_COUNT(X) (X/SELFDIAG_SLOW_PERIOD)
//正常模式执行 诊断
static const STR_DIAG_NODE stDiagNodeTable[] = {   
    { Diag12v,          DIAG_SLOW_COUNT(PWR_DETCT_PERIOD)     },
    { Diag5v,           DIAG_SLOW_COUNT(PWR_DETCT_PERIOD)     },
    { Ntc1SigHighDiag,  DIAG_SLOW_COUNT(PWR_DETCT_PERIOD)     },
    { Ntc1SigLowDiag,   DIAG_SLOW_COUNT(PWR_DETCT_PERIOD)     },
    { Ntc2SigHighDiag,  DIAG_SLOW_COUNT(PWR_DETCT_PERIOD)     },
    { Ntc2SigLowDiag,   DIAG_SLOW_COUNT(PWR_DETCT_PERIOD)     },
    { NtcSigFaultDiag,  DIAG_SLOW_COUNT(NTC_SIGHL_DETCT_PERIOD) },


}; //一般诊断配置表   

/*******************************************************************
函数名称：DiagFastMainProcess
函数原型：void DiagFastMainProcess(void)
功能描述：中断中使用的诊断过程
输入1   ：
返回值  ：
补充信息：     
*********************************************************************/
  void DiagFastMainProcess(void) 
{
	
}

/*******************************************************************
函数名称：SelfDiagMainProcess
函数原型：void SelfDiagMainProcess(void)
功能描述：主循环中使用的诊断过程
输入1   ：
返回值  ：
补充信息：     
*********************************************************************/
  void SelfDiagMainProcess(void) 
{
    UINT8 i;
    volatile const STR_DIAG_NODE *p;
    volatile static UINT16 usCount = 1;
    
    for (i = 0; i < ARRAY_SIZE(stDiagNodeTable); i++) 
    {
        p = &stDiagNodeTable[i];
        if (usCount % p->ucPeriod == 0)
        {
            p->pDiagFunc();
        }   
    } 
    
    if (usCount++ == UINT16_MAX) 
    {
        usCount = 1;    
    }  
}

/*******************************************************************
函数名称：DetctFunction
函数原型：ENU_DETCT_RESULT DetctFunction(T_DETCT_COUNT tCount, 
            T_DETCT_COUNT tPassedLimit, T_DETCT_COUNT tFailedLimit)
功能描述：通过失败次数计数、通过限制次数、失败限制次数输出当前状态
输入1   ：tCount：失败次数计数
输入2   ：tPassedLimit：通过限制次数
输入3   ：tFailedLimit：失败限制次数
返回值  ：ENU_DETCT_RESULT，5个状态
补充信息：     
*********************************************************************/
  ENU_DETCT_RESULT DetctFunction(T_DETCT_COUNT tCount, T_DETCT_COUNT tPassedLimit, T_DETCT_COUNT tFailedLimit) 
{
    if ((tCount * -1) >= tPassedLimit) //检测通过
    {
        return DETCT_PASSED;    
    } 
    else if (tCount == 0) //未执行过检测
    {
        return DETCT_NOTCHECK;
    }
    else if (tCount >= tFailedLimit) //检测失败
    {
        return DETCT_FAILED;   
    }
    else if (tCount < 0) //预通过
    {
        return DETCT_PREPASSED;
    }
    else if (tCount > 0)
    {
        return DETCT_PREFAILED; //预失败
    }
    return DETCT_PASSED;
}

/*******************************************************************
函数名称：CalcFaultDetctCounter
函数原型：void CalcFaultDetctCounter(T_DETCT_COUNT *pssCounter, BOOL bFail)
功能描述：自诊断时，计算失败计数
输入1   ：pssCounter，故障检测计数器指针
输入2   ：bFail，失败标志，0-通过，1-失败
输出1   ：pssCounter，失败计算指针，其范围依赖T_DETCT_COUNT
            当T_DETCT_COUNT为UINT8时，范围为[-128~127]
返回值  ：无
补充信息：以T_DETCT_COUNT为UINT8为例：
          计算顺序为0-通过200次->127-失败1次->-1-失败200次->-128       
*********************************************************************/
  void CalcFaultDetctCounter(T_DETCT_COUNT *ptCounter, BOOL bFail) 
{
    if (!bFail) //通过
    {
        if (*ptCounter > 0) //故障计数处于预失败/失败状态
        {
            *ptCounter = -1; //故障计数反转为-1    
        }
        else if (T_DETCT_MIN != *ptCounter ) //故障计数不等于该类型的最小值则递减
        {
            (*ptCounter)--;  
        }  
    }
    else //失败
    {
        if (*ptCounter < 0) //故障计数处于预成功/成功状态
        {
            *ptCounter = 1; //故障计数反转为+1   
        }
        else if (T_DETCT_MAX != *ptCounter) //故障计数不等于该类型的最大值则递加
        {
            (*ptCounter)++;   
        }  
    }    
}

#define DIAG_CONFIRM_MAX	10

typedef BOOL(*P_FUNC_DETCT_CONFIRMED)(BOOL bFault); //检测结果确认函数接口
typedef struct {
    P_FUNC_DETC pDetectFunc; //检测函数
    P_FUNC_DETCT_CONFIRMED pDetctConfirmed; //检测结果确认函数
} STR_DTC_NODE;


static const STR_DTC_NODE s_stDtcNodeTable[DIAG_CONFIRM_MAX] = 
{
    {  Detct24vH,				Confirm24vH    			}, //0
	{  Detct24vL,         		Confirm24vL				}, //1 UT		
    {  Detct5vH,     			Confirm5vH    			}, //2
	{  Detct5vL,      			Confirm5vL     			}, //3 UT
	{  Ntc1SigHighDetct,      	Ntc1SigHighConfirm     }, //4
    {  Ntc1SigLowDetct,       	Ntc1SigLowConfirm      }, //5 UT
    {  Ntc2SigHighDetct,      	Ntc2SigHighConfirm     }, //6
    {  Ntc2SigLowDetct,       	Ntc2SigLowConfirm      }, //7 UT
	{  NtcSigFaultDetct,      	NtcSigFaultConfirm     }, //8 UT

}; //DTC节点信息表

/*******************************************************************
函数名称：DtcHandle
函数原型：BOOL DtcHandle(const STR_DTC_NODE* pDtcNode, UINT8 ucIndex)
功能描述：DTC处理函数
输入1   ：pDtcNode：DTC节点指针
输入2   ：ucIndex：DTC的顺序号，用于对DTC进行记录顺序号
输出1   ：pDtcNode：更改DTC状态、DTC的失败次数
返回值  ：是否对DTC故障信息进行更新
补充信息：有的DTC故障无法自己恢复，例如灭火装置已使用等，只能进行检测失败/通过确认，
          在失败确认时并保存DTC，成功确认时无法保存已经恢复的DTC
          if (pDtcNode->pDetctConfirmed(TRUE)) 为真时，更新DTC           
*********************************************************************/
static BOOL DtcHandle(const STR_DTC_NODE* pDtcNode, UINT8 ucIndex) 
{
    ENU_DETCT_RESULT eResult = DETCT_NOTCHECK;
    if (pDtcNode->pDetectFunc != NULL) //对诊断的结果进行检测
    {
        eResult = pDtcNode->pDetectFunc();    
    }
	//printf("eResult---- = %d\r\n",eResult);
    if (DETCT_FAILED == eResult) //检测结果为失败
    {
        if (pDtcNode->pDetctConfirmed != NULL)  //对检测结果进行失败确认
        {
            pDtcNode->pDetctConfirmed(TRUE);
        }
        return TRUE;
    } 
    else if (DETCT_PASSED == eResult) //检测结果为通过 
    {
        if (pDtcNode->pDetctConfirmed != NULL) //对检测结果进行成功确认 
        {
            pDtcNode->pDetctConfirmed(FALSE);
        }
        return TRUE;            
        
    }
    return FALSE;  
}


/*******************************************************************
函数名称：DiagnosticMainProcess
函数原型：void DiagnosticMainProcess(void) 
功能描述：诊断服务主流程
输入1   ：无
返回值  ：无
补充信息：       
*********************************************************************/
void DiagnosticMainProcess(void) 
{
    UINT8 i;
    UINT8 ucCount = 0;
    for (i = 0; i < ARRAY_SIZE(s_stDtcNodeTable); i++) {
        if (DtcHandle(&s_stDtcNodeTable[i], i)) {
            ucCount++;    
        }
		else
		{
			;
		}
    }
}
