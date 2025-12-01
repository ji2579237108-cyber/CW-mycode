#include "selfchk.h"
#include "appconf.h"
#include "cf.h"
#include "saecrc.h"
#include "cjcrc.h"
#include "nvm.h"
#include "selfdiag.h"
#include "pwr_node.h"
#include "selfchk_node.h"
#include "AdcCfg.h"
#include "DioCfg.h"
#include "para.h"





typedef struct {
    UINT16 usRunMode;
    UINT16 usCrc;
} STR_SYS_RUN_MODE;

STR_SELF_CHK_INFO gstSelfChkInfo;
STR_SYS_RUN_MODE  gstSysRunMode;

void SelfChk_SaveMode(void) 
{
   ParaWrite();   
}

void SelfChkInit(void)
{
//    (void)NVMRead(DINDEX_SELFCHK, &gstSysRunMode, sizeof(gstSysRunMode));
//    if (gstSysRunMode.usRunMode != (UINT16)~gstSysRunMode.usCrc) { //自检参数校验失败
//        gstSysRunMode.usRunMode = RUN_CHK_MODE; //运行模式设置为自检模式
//        SelfChk_SaveMode();
//    }
//    c_bzero(&gstSelfChkInfo, sizeof(gstSelfChkInfo));
}



BOOL IsSelfChkMode(void)
{
    if (pSysPara->usSelfModeFlag != RUN_CHK_MODE) {
        return TRUE;
    } else {
        return FALSE;
    }
}



/*******************************************************************
函数名称：SelfChkOperate
函数原型：BOOL SelfChkOperate (UINT8 ucOp)
功能描述：自检模块退出/进入
补充信息：
输入1   ：ucOp:CHK_OP_ENTER, CHK_OP_QUIT
输出1   ：gstSelfChkInfo
返回    ：TRUE-操作成功，FALSE-操作失败
*********************************************************************/
BOOL SelfChkOperate (UINT8 ucOp)
{
    if (ucOp == CHK_OP_ENTER) {
        pSysPara->usSelfModeFlag = RUN_CHK_MODE;
        SelfChk_SaveMode();
        c_bzero(&gstSelfChkInfo, sizeof(gstSelfChkInfo));
        return TRUE;
    } else if (ucOp == CHK_OP_QUIT) {
        pSysPara->usSelfModeFlag = RUN_NOR_MODE;
        SelfChk_SaveMode();
        return TRUE;
    }
    return FALSE;
}


/*******************************************************************
函数名称：SelfChkQuery
函数原型：UINT8 SelfChkQuery(void *ptr, UINT8 len)
功能描述：自检结果查询
补充信息：
输入1   ：ptr
输出1   ：len
返回    ：UINT8，结果长度
*********************************************************************/
  UINT8 SelfChkQuery(void *ptr, UINT8 len)
{    
    c_memcpy(ptr, gstSelfChkInfo.CODE_ARRAY, len);
    return len;
}

#define SELFCHK_PERIOD 100 //100ms

typedef void (*P_FUNC_CODE_SET)(void); 

typedef struct {
    pfnDiagFunc pDiagFunc;
    UINT16 usTick;
} STR_SELFCHK_NODE;

#define SELFCHK_TICKS(X) (X/SELFCHK_PERIOD)
STR_SELFCHK_NODE stSelfChkNode[] = {
   { Diag24v,      			SELFCHK_TICKS(PWR24V_DETCT_PERIOD) },  
   { Diag5v,       			SELFCHK_TICKS(PWR5V_DETCT_PERIOD) },
   { Diag3v,       			SELFCHK_TICKS(PWR3V_DETCT_PERIOD) },
   { SelfChkExCanCtr,    	SELFCHK_TICKS(EXCAN_DETCT_PERIOD) }, 
	 
};

 void SelfChk(void)
{
    UINT8 i;
    static UINT32 uwTicks = 0;
    
    for (i = 0; i < ARRAY_SIZE(stSelfChkNode); i++) {
        if (stSelfChkNode[i].usTick != 0 && \
            uwTicks % stSelfChkNode[i].usTick == 0) {
                stSelfChkNode[i].pDiagFunc();
        }
    }
    uwTicks++;
}

  void SelfChkMainProcess(void)
{
    if (gstSelfChkInfo.CODE_24V == 0 && (Detct24vH() == DETCT_FAILED || Detct24vL() == DETCT_FAILED)) { //24v自检
        gstSelfChkInfo.CODE_24V = 1;		
    }
    if (gstSelfChkInfo.CODE_3V == 0 && (Detct3vH() == DETCT_FAILED || Detct3vL() == DETCT_FAILED)) { //12v自检
        gstSelfChkInfo.CODE_3V = 1;
    } 
    if (gstSelfChkInfo.CODE_5V == 0 && (Detct5vH() == DETCT_FAILED || Detct5vL() == DETCT_FAILED)) { //5v自检
        gstSelfChkInfo.CODE_5V = 1;
    }  

    if (gstSelfChkInfo.CODE_EXCAN == 0 && SelfChkExCan() == DETCT_FAILED) {
        gstSelfChkInfo.CODE_EXCAN = 1;
    } 

		//电磁阀
		//编码线
} 
