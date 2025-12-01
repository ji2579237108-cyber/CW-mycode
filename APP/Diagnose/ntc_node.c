#include "selfdiag.h"

/********************************************************************
版权（C），烟台创为新能源科技有限公司
文 件 名：ntc_node.c
作    者：chungway
版    本：1.0
生成日期：
概    述：NTC0,1,2自诊断 
*********************************************************************/

typedef struct 
{
	UINT8 NTC1_Falut_H;
	UINT8 NTC1_Falut_L;
	UINT8 NTC2_Falut_H;
	UINT8 NTC2_Falut_L;
	UINT8 NTC_Fault_Signal;
}STR_NTC_FAULT;

STR_NTC_FAULT gstNtcFault;

UINT8 Ntc1_FaultIsH() 
{
    return gstNtcFault.NTC1_Falut_H ;	
}
UINT8 Ntc1_FaultIsL() 
{
    return gstNtcFault.NTC1_Falut_L ;	
}
UINT8 Ntc2_FaultIsH() 
{
    return gstNtcFault.NTC2_Falut_H  ;	
}
UINT8 Ntc2_FaultIsL() 
{
    return gstNtcFault.NTC2_Falut_L  ;	
}
UINT8 Ntc_FaultIsSignal() 
{
    return gstNtcFault.NTC_Fault_Signal ;	
}

//######## NTC2 Signal High Diagnostic ########
static T_DETCT_COUNT tNTC1SigHCounter = 0; //NTC1信号过高计数器
/*******************************************************************
函数名称：Ntc1SigHighDiag
函数原型：void Ntc1SigHighDiag(void)
功能描述：NTC1信号过高诊断
输入1   ：无
返回值  ：无
补充信息：本版本硬件NTC1电源无法关闭，诊断条件不需要判断NTC1的电源状态 
*********************************************************************/
void Ntc1SigHighDiag(void)
{

	if (AdcDmaValue(SAMPLE_NTC1) > NTC_CRITICAL_MAX_VOL) 
	{
		CalcFaultDetctCounter(&tNTC1SigHCounter, TRUE);  
	} 
	else 
	{
		CalcFaultDetctCounter(&tNTC1SigHCounter, FALSE); 
	}
}
/*******************************************************************
函数名称：Ntc1SigHighDetct
函数原型：ENU_DETCT_RESULT Ntc1SigHighDetct(void)
功能描述：NTC1信号过高检测
输入1   ：无
返回值  ：检测结果
补充信息：
*********************************************************************/
ENU_DETCT_RESULT Ntc1SigHighDetct(void)
{
    return DetctFunction(tNTC1SigHCounter, NTC_SIGHL_PASS_LIMIT, NTC_SIGHL_FAIL_LIMIT); 
} 

/*******************************************************************
函数名称：Ntc1SigHighConfirm
函数原型：BOOL Ntc1SigHighConfirm(BOOL bFault)
功能描述：NTC1信号过高确认
输入1   ：bFault：TRUE：失败确认，FALSE：通过确认
返回值  ：TRUE：确认；FALSE：不确认
补充信息：
*********************************************************************/
BOOL Ntc1SigHighConfirm(BOOL bFault) 
{
#ifdef SELFDIAG_DEBUG
    SelfDiagDebug("Ntc1SigHighConfirm", tNTC1SigHCounter, NTC_SIGHL_PASS_LIMIT, NTC_SIGHL_FAIL_LIMIT);   
#endif
    gstNtcFault.NTC1_Falut_H = bFault; 
    return TRUE;
}
//######## NTC1 Signal Low Diagnostic ########
static T_DETCT_COUNT tNTC1SigLowCounter = 0; //NTC1信号过低计数器
/*******************************************************************
函数名称：Ntc1SigLowDiag
函数原型：void Ntc1SigLowDiag(void)
功能描述：NTC1信号过低诊断
输入1   ：无
返回值  ：无
补充信息：本版本硬件NTC1电源无法关闭，诊断条件不需要判断NTC1的电源状态 
*********************************************************************/
void Ntc1SigLowDiag(void)
{
	if (AdcDmaValue(SAMPLE_NTC1) < NTC_CRITICAL_MIN_VOL) 
    {
        CalcFaultDetctCounter(&tNTC1SigLowCounter, TRUE);  
    } 
    else 
    {
        CalcFaultDetctCounter(&tNTC1SigLowCounter, FALSE); 
    }
}
/*******************************************************************
函数名称：Ntc1SigLowDetct
函数原型：ENU_DETCT_RESULT Ntc1SigLowDetct(void)
功能描述：NTC1信号过低检测
输入1   ：无
返回值  ：检测结果
补充信息：
*********************************************************************/
ENU_DETCT_RESULT Ntc1SigLowDetct(void)
{
    return DetctFunction(tNTC1SigLowCounter, NTC_SIGHL_PASS_LIMIT, NTC_SIGHL_FAIL_LIMIT); 
} 
    
/*******************************************************************
函数名称：Ntc1SigLowConfirm
函数原型：BOOL Ntc1SigLowConfirm(BOOL bFault)
功能描述：NTC1信号过低确认
输入1   ：bFault：TRUE：失败确认，FALSE：通过确认
返回值  ：TRUE：确认；FALSE：不确认
补充信息：
*********************************************************************/
BOOL Ntc1SigLowConfirm(BOOL bFault) 
{
#ifdef SELFDIAG_DEBUG
    SelfDiagDebug("Ntc1SigLowConfirm", tNTC1SigLowCounter, NTC_SIGHL_PASS_LIMIT, NTC_SIGHL_FAIL_LIMIT);   
#endif
    gstNtcFault.NTC1_Falut_L = bFault; 
    return TRUE;
}

//######## NTC2 Signal Low Diagnostic ########
static T_DETCT_COUNT tNTC2SigLCounter = 0; //NTC2信号过低计数器
/*******************************************************************
函数名称：Ntc2SigLowDiag
函数原型：void Ntc2SigLowDiag(void)
功能描述：NTC2信号过低诊断
输入1   ：无
返回值  ：无
补充信息：本版本硬件NTC2电源无法关闭，诊断条件不需要判断NTC2的电源状态 
*********************************************************************/
void Ntc2SigLowDiag(void)
{  
	if (AdcDmaValue(SAMPLE_NTC2) < NTC_CRITICAL_MIN_VOL) 
	{
		CalcFaultDetctCounter(&tNTC2SigLCounter, TRUE);  
	} 
	else
	{
		CalcFaultDetctCounter(&tNTC2SigLCounter, FALSE);
	}
}
/*******************************************************************
函数名称：Ntc2SigLowDetct
函数原型：ENU_DETCT_RESULT Ntc2SigLowDetct(void)
功能描述：NTC2信号过低检测
输入1   ：无
返回值  ：检测结果
补充信息：
*********************************************************************/
ENU_DETCT_RESULT Ntc2SigLowDetct(void)
{
    return DetctFunction(tNTC2SigLCounter, NTC_SIGHL_PASS_LIMIT, NTC_SIGHL_FAIL_LIMIT); 
}

/*******************************************************************
函数名称：Ntc2SigLowConfirm
函数原型：BOOL Ntc1S2gLowConfirm(BOOL bFault)
功能描述：NTC2信号过低确认
输入1   ：bFault：TRUE：失败确认，FALSE：通过确认
返回值  ：TRUE：确认；FALSE：不确认
补充信息：
*********************************************************************/
BOOL Ntc2SigLowConfirm(BOOL bFault) 
{
#ifdef SELFDIAG_DEBUG
    SelfDiagDebug("Ntc2SigLowConfirm", tNTC2SigLCounter, NTC_SIGHL_PASS_LIMIT, NTC_SIGHL_FAIL_LIMIT);   
#endif
    gstNtcFault.NTC2_Falut_H = bFault; 

    return TRUE;
}     
 
//######## NTC2 Signal High Diagnostic ########
static T_DETCT_COUNT tNTC2SigHCounter = 0; //NTC2信号过高计数器
/*******************************************************************
函数名称：Ntc2SigHighDiag
函数原型：void Ntc2SigHighDiag(void)
功能描述：NTC2信号过高诊断
输入1   ：无
返回值  ：无
补充信息：本版本硬件NTC2电源无法关闭，诊断条件不需要判断NTC2的电源状态 
*********************************************************************/
void Ntc2SigHighDiag(void)
{
    if (AdcDmaValue(SAMPLE_NTC2) > NTC_CRITICAL_MAX_VOL) 
    {
        CalcFaultDetctCounter(&tNTC2SigHCounter, TRUE);  
    } 
    else 
    {
        CalcFaultDetctCounter(&tNTC2SigHCounter, FALSE); 
    }
}

/*******************************************************************
函数名称：Ntc2SigHighDetct
函数原型：ENU_DETCT_RESULT Ntc2SigHighDetct(void)
功能描述：NTC2信号过高检测
输入1   ：无
返回值  ：检测结果
补充信息：
*********************************************************************/
ENU_DETCT_RESULT Ntc2SigHighDetct(void)
{
    return DetctFunction(tNTC2SigHCounter, NTC_SIGHL_PASS_LIMIT, NTC_SIGHL_FAIL_LIMIT); 
}

/*******************************************************************
函数名称：Ntc2SigHighConfirm
函数原型：BOOL Ntc2SigHighConfirm(BOOL bFault)
功能描述：NTC2信号过高确认
输入1   ：bFault：TRUE：失败确认，FALSE：通过确认
返回值  ：TRUE：确认；FALSE：不确认
补充信息：
*********************************************************************/
BOOL Ntc2SigHighConfirm(BOOL bFault) 
{
#ifdef SELFDIAG_DEBUG
    SelfDiagDebug("Ntc2SigHighConfirm", tNTC2SigHCounter, NTC_SIGHL_PASS_LIMIT, NTC_SIGHL_FAIL_LIMIT);   
#endif
    gstNtcFault.NTC2_Falut_L = bFault; 

    return TRUE;
}
//######## NTC2 Signal Fault Diagnostic ########
static T_DETCT_COUNT tNTCSigFaultCounter = 0; //NTC信号错误计数器

/*******************************************************************
函数名称：NtcSigFaultDiag
函数原型：void NtcSigFaultDiag(void)
功能描述：NTC信号不可信诊断
输入1   ：无
返回值  ：无
补充信息：
*********************************************************************/
void NtcSigFaultDiag(void)
{
    
    float fNTC1, fNTC2, fDiff;
    
    fNTC1 = AdcDmaValue(SAMPLE_NTC1);
    fNTC2 = AdcDmaValue(SAMPLE_NTC2);

    if (DETCT_PASSED == Ntc1SigHighDetct() && \
        DETCT_PASSED == Ntc1SigLowDetct() && \
        DETCT_PASSED == Ntc2SigHighDetct() && \
        DETCT_PASSED == Ntc2SigLowDetct())
    {
        if (fNTC1 > 100 && fNTC2 > 100) 
        {
            CalcFaultDetctCounter(&tNTCSigFaultCounter, FALSE);
            return;
        } 
        else 
        {
            fDiff = fNTC1 - fNTC2;
            if (fDiff > 30 || fDiff < -30) 
            {
                CalcFaultDetctCounter(&tNTCSigFaultCounter, TRUE);    
            } 
            else 
            {
                CalcFaultDetctCounter(&tNTCSigFaultCounter, FALSE);    
            }
        }
    }
}

/*******************************************************************
函数名称：NtcSigFaultDetct
函数原型：ENU_DETCT_RESULT NtcSigFaultDetct(void)
功能描述：NTC信号不可信检测
输入1   ：无
返回值  ：检测结果
补充信息：
*********************************************************************/
ENU_DETCT_RESULT NtcSigFaultDetct(void)
{
    return DetctFunction(tNTCSigFaultCounter, NTC_SIGHL_PASS_LIMIT, NTC_SIGHL_FAIL_LIMIT); 
}

/*******************************************************************
函数名称：NtcSigFaultConfirm
函数原型：BOOL NtcSigFaultConfirm(BOOL bFault)
功能描述：NTC信号不可信确认
输入1   ：bFault：TRUE：失败确认，FALSE：通过确认
返回值  ：TRUE：确认；FALSE：不确认
补充信息：
*********************************************************************/
BOOL NtcSigFaultConfirm(BOOL bFault) 
{
#ifdef SELFDIAG_DEBUG
    SelfDiagDebug("NtcSigFaultConfirm", tNTCSigFaultCounter, NTC_SIGHL_PASS_LIMIT, NTC_SIGHL_FAIL_LIMIT);   
#endif
	gstNtcFault.NTC_Fault_Signal = bFault;  
    return TRUE;
}     
