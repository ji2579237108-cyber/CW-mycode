#include "auto_code.h"
#include "cf.h"
#include "app.h"
#include "auto_code_cw.h"

static STR_AUTO_CODE_RUN_PARA sg_AutoCodeInfo;
STR_AUTO_CODE_RUN_PARA* const autoCodeRunParaPtr = &sg_AutoCodeInfo;

void AutoCodeInit(void)   //KP_WAIT
{
    AutoCodeRunParaClr();
    c_bzero(&sg_AutoCodeInfo, sizeof(sg_AutoCodeInfo));
    autoCodeRunParaPtr->autoCodeStep = AUTOCODE_IDLE_STEP;
    autoCodeRunParaPtr->autoCodeNextStep = AUTOCODE_IDLE_STEP; //2.0.1.0[1] 
}

//自动编码新老版本判断
void AutoCodeVersionDetectByB0(STR_CAN_STD_TYPE *canf) 
{
    if (autoCodeRunParaPtr->autoCodeStep == AUTOCODE_DETCT_STEP) {
        if (autoCodeRunParaPtr->ucB0Ctr != 0xffu) {
            autoCodeRunParaPtr->ucB0Ctr++;
        }  
        if (canf->data[6] != 0x55 && canf->data[6] != 0xaa) {
            if (autoCodeRunParaPtr->ucCwB0Ctr != 0xffu) {
                autoCodeRunParaPtr->ucCwB0Ctr++;    
            } 
        } else {
            if (autoCodeRunParaPtr->ucCatlB0Ctr != 0xffu) {
                autoCodeRunParaPtr->ucCatlB0Ctr++;    
            }
        }
    }
}

void AutoCodeVersionDetect(void) 
{
    //(void)printf("@AutoCodeVersionDetect\n");     
    if (autoCodeRunParaPtr->ucB0Ctr == 0) { //Not Recv B0
        if (autoCodeRunParaPtr->ssB0TimeOut < 0) {
            autoCodeRunParaPtr->ssB0TimeOut = 1;
        } else {
            if (autoCodeRunParaPtr->ssB0TimeOut != 127) {
                autoCodeRunParaPtr->ssB0TimeOut++;    
            }  
        }
    } else { //Recv B0
        autoCodeRunParaPtr->ucB0Ctr = 0;
        if (autoCodeRunParaPtr->ssB0TimeOut > 0) {
            autoCodeRunParaPtr->ssB0TimeOut = -1;
        } else {
            if (autoCodeRunParaPtr->ssB0TimeOut != -128) {
                autoCodeRunParaPtr->ssB0TimeOut--;    
            }  
        }   
    }
    if (autoCodeRunParaPtr->ssB0TimeOut < -10) {
        if (autoCodeRunParaPtr->ucCwB0Ctr > 0) {
            sg_AutoCodeInfo.ucAutoCodeVersion = CODE_VER_CW;
            sg_AutoCodeInfo.autoCodeNextStep = AUTOCODE_ASK04H_STEP;
        } else {
            sg_AutoCodeInfo.ucAutoCodeVersion = CODE_VER_CATL; 
            sg_AutoCodeInfo.autoCodeNextStep = AUTOCODE_ASK90_STEP;
        }
    }
}

void AutoCodeCanSend(UINT8 ucCommand) 
{
    STR_CAN_STD_TYPE stCanf;
    c_bzero(stCanf.data, 8);
    stCanf.exd = 1;
    stCanf.rtr = 0;
    stCanf.len = 8;
    switch (ucCommand) {
    case 0x90:
        stCanf.CANEXDID = 0x1a90f5f4;        
        break;
    case 0x91:
        stCanf.CANEXDID = 0x1a91f5f4;
        break;
    case 0x24:
        stCanf.CANEXDID = 0x1a24f5ff;
        break;
    case 0x0c:
        stCanf.CANEXDID = 0x1a0cf5ff;
        break;
    case 0x04:
        stCanf.CANEXDID = 0x1a04f5ff; 
        stCanf.data[0] = 0xff;
        stCanf.len = 1;               
        break;
    case 0x05:
        stCanf.CANEXDID = 0x1a05f5ff; 
        stCanf.data[0] = 0xff;
        stCanf.len = 1;
        break; 
    case 0x06:
        stCanf.CANEXDID = 0x1a06f5ff; 
        stCanf.data[0] = 0xff;
        stCanf.len = 1;
        break; 
     case 0x07:
        stCanf.CANEXDID = 0x1a07f5ff; 
        stCanf.data[0] = 0xff;
        stCanf.len = 1;
        break;
     case 0x08:
        stCanf.CANEXDID = 0x1a08f5ff; 
        stCanf.data[0] = 0xff;
        stCanf.len = 1;
        break;  
    default:
        return;          
    }
    _IFR_CAN_IN_SEND(&stCanf);     
}


UINT8 AutoCodeLineCheck(UINT8 ucModuleNum, UINT8 ucOrder) 
{                      
    UINT8 i;
    UINT16 *usPtr;
    
    usPtr = autoCodeRunParaPtr->usArrayCodeVol;    
    bubble_sort(usPtr, ucModuleNum, ucOrder);
    for (i = 0; i < ucModuleNum - 1; i++) {
        if (usPtr[i] == usPtr[i+1]) {
#if (DBUG_AUTOCODE > 0u)
            (void)printf("V's NotEQ\r\n");
#endif
            return 1;
        }
    }
    return 0;
}

void AutoCodeRunParaClr(void) 
{
    autoCodeRunParaPtr->ucParaModuleMax = 0;
    autoCodeRunParaPtr->ucParaModuleMin = 0;
    autoCodeRunParaPtr->ucParaModuleNum = 0;
    autoCodeRunParaPtr->ucParaReplyNum = 0;
    autoCodeRunParaPtr->ucVolReplyNum = 0;
    //KP-WAIT autoCodeRunParaPtr->B0_RECV = 0;    
    sg_AutoCodeInfo.ucAutoCodeVersion = CODE_VER_UNK; 
    autoCodeRunParaPtr->ucB0Ctr = 0;
    autoCodeRunParaPtr->ucCwB0Ctr = 0;
    autoCodeRunParaPtr->ucCatlB0Ctr = 0;  
    autoCodeRunParaPtr->ssB0TimeOut = 0;      
}

void AutoCodeMainProcess(void) 
{
    if (autoCodeRunParaPtr->autoCodeStep == AUTOCODE_DETCT_STEP) {
        AutoCodeVersionDetect();       
    }
    if (sg_AutoCodeInfo.ucAutoCodeVersion == CODE_VER_CATL) {
        AutoCodeCtalProcess();
    } else {
        AutoCodeCwProcess();
    }
}

void AutoCode0ChRecv(STR_CAN_STD_TYPE *canf)
{
    if (autoCodeRunParaPtr->ucParaReplyNum == 0) {
        autoCodeRunParaPtr->ucParaModuleMin = canf->data[3];
        autoCodeRunParaPtr->ucParaModuleMax = autoCodeRunParaPtr->ucParaModuleMin;        
    } else {
        if (autoCodeRunParaPtr->ucParaModuleMin > canf->data[3]) {
            autoCodeRunParaPtr->ucParaModuleMin = canf->data[3];
        }
        if (autoCodeRunParaPtr->ucParaModuleMax < canf->data[3]) {
            autoCodeRunParaPtr->ucParaModuleMax = canf->data[3];
        }
    }
    if (autoCodeRunParaPtr->ucParaReplyNum != 0xff) {
        autoCodeRunParaPtr->ucParaReplyNum++;
    }
#if (DBUG_AUTOCODE > 0u)
    printf("autoCodeRunParaPtr->ucParaReplyNum:%d, max:%d, min:%d\r\n", 
        autoCodeRunParaPtr->ucParaReplyNum, 
        autoCodeRunParaPtr->ucParaModuleMax, 
        autoCodeRunParaPtr->ucParaModuleMin);
#endif
}

void AutoCode90hRecv(STR_CAN_STD_TYPE *canf) 
{
    AutoCode0ChRecv(canf);
}

void AutoCode24hRecv(STR_CAN_STD_TYPE *canf)
{
    UINT8 ucNum = autoCodeRunParaPtr->ucVolReplyNum;
    autoCodeRunParaPtr->usArrayCodeVol[ucNum] = ((UINT16)(canf->data[0]) << 8) + canf->data[1];
    autoCodeRunParaPtr->ucArrayDevId[ucNum] = canf->J1939SA;
    autoCodeRunParaPtr->ucVolReplyNum++;
#if (DBUG_AUTOCODE > 0u)
    printf("autoCodeRunParaPtr->ucVolReplyNum:%d, Id:%d, Vol:%d\r\n", 
        autoCodeRunParaPtr->ucVolReplyNum,
        autoCodeRunParaPtr->ucArrayDevId[ucNum],
         autoCodeRunParaPtr->usArrayCodeVol[ucNum]);
#endif
}

void AutoCode91hRecv(STR_CAN_STD_TYPE *canf) 
{
    UINT8 ucNum = autoCodeRunParaPtr->ucVolReplyNum;
    autoCodeRunParaPtr->usArrayCodeVol[ucNum] = ((UINT16)(canf->data[1]) << 8) + canf->data[0];
    autoCodeRunParaPtr->ucArrayDevId[ucNum] = canf->J1939SA;
    autoCodeRunParaPtr->ucVolReplyNum++;
#if (DBUG_AUTOCODE > 0u)
    (void)printf("91RX:\r\n");
    printf("autoCodeRunParaPtr->ucVolReplyNum:%d, Id:%d, Vol:%d\r\n", 
        autoCodeRunParaPtr->ucVolReplyNum,
        autoCodeRunParaPtr->ucArrayDevId[ucNum],
         autoCodeRunParaPtr->usArrayCodeVol[ucNum]);
#endif
}

void AutoCode07hRecv(STR_CAN_STD_TYPE *canf)
{
    if (canf->J1939PF != 0x07) 
        return;
} 

void AutoCodeC7hRecv(STR_CAN_STD_TYPE *canf)
{
    if (canf->J1939PF != 0xC7) 
        return;
} 

void AutoCode92hRecv(STR_CAN_STD_TYPE *canf)
{
    if (canf->J1939PF != 0x92) 
        return;
} 

void AutoCodeSetModuleNum(void) 
{
    UINT8 ucModuleNum = 0;
     
#if (DBUG_AUTOCODE > 0u)
    (void)printf("PaRN:%d,VRN:%d\n",  
        autoCodeRunParaPtr->ucParaReplyNum,
        autoCodeRunParaPtr->ucVolReplyNum);
#endif 
    if (autoCodeRunParaPtr->ucParaReplyNum != 0 &&                \
        autoCodeRunParaPtr->ucVolReplyNum != 0) {
        ucModuleNum = autoCodeRunParaPtr->ucParaReplyNum >        \
                      autoCodeRunParaPtr->ucVolReplyNum ?         \
                      autoCodeRunParaPtr->ucParaReplyNum :        \
                      autoCodeRunParaPtr->ucVolReplyNum; //取90和91回复最大个数    
    }

#if (DBUG_AUTOCODE > 0u)
    (void)printf("M1,%d\n", ucModuleNum);
#endif
    if (autoCodeRunParaPtr->ucParaModuleMin == autoCodeRunParaPtr->ucParaModuleMax) {
        if (autoCodeRunParaPtr->ucParaModuleMax > ucModuleNum) {
            ucModuleNum =  autoCodeRunParaPtr->ucParaModuleMax;
            #if (DBUG_AUTOCODE > 0u)
            (void)printf("M2,:%d\n", ucModuleNum);
            #endif        
        }
    } else {
        autoCodeRunParaPtr->MODULE_NUM_INVALID = TRUE;
#if (DBUG_AUTOCODE > 0u)
        (void)printf("MREP:T\n");
#endif
    }
    if (ucModuleNum != 0 && ucModuleNum != pSysPara->ucModuleNum) { //重新设置ID
#if (DBUG_AUTOCODE > 0u)
    (void)printf("MNUM:%d,%d\n", ucModuleNum, pSysPara->ucModuleNum);
#endif
        if(ucModuleNum > FD_NUM) return; //20.0.16.0+ 限制最大终端数量
        pSysPara->ucModuleNum = ucModuleNum; 
        pSysPara->ucFdTypeWay = TYPE_SET_FD;//FT_01_19   
        ParaWrite();
        FdReset();     
        (void)FdSetNumber(pSysPara->ucModuleNum);
    }
}

BOOL AutoCodeIsIdle(void) 
{
    if (autoCodeRunParaPtr->autoCodeStep == AUTOCODE_IDLE_STEP  || \
        autoCodeRunParaPtr->autoCodeStep == AUTOCODE_DETCT_STEP || \
        autoCodeRunParaPtr->autoCodeStep == AUTOCODE_OVER_STEP) { //2.0.1.0[1] AUTOCODE_DETCT_STEP 
        return TRUE;
    } else {
        return FALSE;
    }
}

void AutoCodeEnable(void) 
{ 
    if (AutoCodeIsIdle()) {
        AutoCodeInit();     
        autoCodeRunParaPtr->autoCodeStep = AUTOCODE_DETCT_STEP; 
        autoCodeRunParaPtr->autoCodeNextStep = AUTOCODE_DETCT_STEP; //2.0.1.0[1]
    }
}


/*
 * @ModuleName: AutoCode_IdRepChk
 * @Version   : 0.1
 */
UINT8 AutoCode_IdRepChk(UINT8 *ucIdPtr, UINT8 ucCnt) 
{
    UINT8 ucRes = AUTOCODE_ID_NONE_ERR;
    int ix, ix2;
    if (ucCnt == 0) {
        ucRes |= AUTOCODE_ID_EMPTY_ERR;
    } else {
        for (ix = 0; ix < ucCnt; ix++) {
            if (ucIdPtr[ix] == 0x00u) {
                ucRes |= AUTOCODE_ID_00_ERR;
            } else if (ucIdPtr[ix] == 0xFFu) {
                ucRes |= AUTOCODE_ID_FF_ERR;
            }
        }
        if (ucCnt > 1) {
            for (ix = 0; ix < ucCnt-1; ix++) {
                for (ix2 = ix+1; ix2 < ucCnt; ix2++) {
                    if (ucIdPtr[ix] == ucIdPtr[ix2] && 
                        ucIdPtr[ix] != 0xFFu        &&
                        ucIdPtr[ix] != 0x00u) {
                        ucRes |= AUTOCODE_ID_REP_ERR;
                    }
                }
            }
        }
    }
    return ucRes;
}


void AutoCodeDisable(void) 
{
    if (autoCodeRunParaPtr->autoCodeStep == AUTOCODE_DETCT_STEP || \
        autoCodeRunParaPtr->autoCodeStep == AUTOCODE_OVER_STEP) { //2.0.1.0[1]
        autoCodeRunParaPtr->autoCodeStep = AUTOCODE_IDLE_STEP; 
    }
       
}

UINT8 AutoCodeCheck(void) 
{
    UINT8 ucRes;
    UINT16 usSum;
    UINT8 i;
        
    ucRes = AutoCode_IdRepChk(autoCodeRunParaPtr->ucArrayDevId,
                              autoCodeRunParaPtr->ucVolReplyNum);
    if (ucRes == AUTOCODE_ID_EMPTY_ERR) { //地址个数为0 
        autoCodeRunParaPtr->ID_STATUS = AUTOCODE_NG_STATUS;
        return 1;
    } else {
        if (ucRes == AUTOCODE_ID_NONE_ERR) {
            usSum =  autoCodeRunParaPtr->ucVolReplyNum * \
                    (autoCodeRunParaPtr->ucVolReplyNum + 1) / 2;      
            for (i = 0; i < autoCodeRunParaPtr->ucVolReplyNum; i++) { // 电池箱个数连续识别               
                usSum -= autoCodeRunParaPtr->ucArrayDevId[i];    
            }
            if (usSum == 0) {
                autoCodeRunParaPtr->ID_STATUS = AUTOCODE_OK_STATUS;   // ID OK                            
                autoCodeRunParaPtr->CODE_STATUS = AUTOCODE_OK_STATUS; // 设置编码状态为OK                 
                return 1;
            } else { 
                //V0.1.0.0[9]+
                autoCodeRunParaPtr->ID_STATUS = AUTOCODE_NG_STATUS;   // ID NG,00,FF,REP
                if (autoCodeRunParaPtr->ucVolReplyNum != pSysPara->ucModuleNum) { // 不连续，不需要编码，例如：4个箱，只上传1，2，3
                    autoCodeRunParaPtr->CODE_STATUS = AUTOCODE_OK_STATUS;         // 设置编码状态为OK
                    return 1;
                } else { //不连续，但仍然需要编码
                    ;
                }
                //V0.1.0.0[9] END
            }
        }  
        //V0.1.0.0[9]+
        else 
        { 
            autoCodeRunParaPtr->ID_STATUS = AUTOCODE_NG_STATUS;       // ID NG,00,FF,REP 
        }
        //V0.1.0.0[9] END
  
        if (0 == AutoCodeLineCheck(autoCodeRunParaPtr->ucVolReplyNum, 
                                   autoCodeRunParaPtr->CODE_ORDER)) { // 编码线检查通过                   
            autoCodeRunParaPtr->CODE_STATUS = AUTOCODE_UNK_STATUS;    // 设置编码状态为未知               
            autoCodeRunParaPtr->CODE_LINE_FAULT = 0;
            return 0;
        } else { //编码线未检查通过
            autoCodeRunParaPtr->CODE_STATUS = AUTOCODE_NG_STATUS;     // 设置编码状态为NG                 
            autoCodeRunParaPtr->CODE_LINE_FAULT = 1;
        }        
    }

    return 1;
}
                                                                                                                        //V2.1.0.0+
//需要编码，但编码线故障                                                                                                
UINT8 AutoCodeLineError(void) 
{
    return autoCodeRunParaPtr->CODE_LINE_FAULT;
}

