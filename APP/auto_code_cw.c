#include "auto_code_cw.h"
#include "cf.h"

#include "app.h"
#include "serv.h"

void AutoCodeCwDownCodeVol(void) 
{
    UINT8 i;
    UINT8 ucFalg;
    STR_CAN_STD_TYPE stCanf;
    c_bzero(stCanf.data, 8);
    stCanf.CANEXDID = 0x1AC7F5FF;
    stCanf.exd = 1;
    stCanf.rtr = 0;
    ucFalg =  autoCodeRunParaPtr->CODE_ORDER == 1 ? 0 : 1;
    bubble_sort(autoCodeRunParaPtr->usArrayCodeVol, 
        autoCodeRunParaPtr->ucVolReplyNum, ucFalg);  
    for (i = 0; i < autoCodeRunParaPtr->ucVolReplyNum; i++) {
        stCanf.data[0] = (UINT8)(autoCodeRunParaPtr->usArrayCodeVol[i] >> 8);
        stCanf.data[1] = (UINT8)(autoCodeRunParaPtr->usArrayCodeVol[i] & 0xff);
        stCanf.data[2] = i + 1;
        stCanf.len = 3;
        _IFR_CAN_IN_SEND(&stCanf);
#if (DBUG_AUTOCODE > 0u)
        (void)printf("[%d]:[%d]\r\n", stCanf.data[2], ((UINT16)stCanf.data[0]<<8)+(UINT16)stCanf.data[1]);
#endif   
        _IFR_MSLEEP(3);  
    }
}

#define AUTOCODE_ASK24_STEP_SEC   0x04u

void AutoCodeCwProcess(void) 
{
    if (autoCodeRunParaPtr->ucCurWaitSecs != 0) {
        autoCodeRunParaPtr->ucCurWaitSecs--;    
    }
    
    if (autoCodeRunParaPtr->ucCurWaitSecs == 0) {
        if (autoCodeRunParaPtr->autoCodeStep != autoCodeRunParaPtr->autoCodeNextStep) {
            autoCodeRunParaPtr->autoCodeStep = autoCodeRunParaPtr->autoCodeNextStep;
            switch (autoCodeRunParaPtr->autoCodeStep) {
            case AUTOCODE_IDLE_STEP:
            case AUTOCODE_OVER_STEP:

            case AUTOCODE_DETCT_STEP:
                break; 
            case AUTOCODE_ASK04H_STEP:
                AutoCodeCanSend(0x04);
                autoCodeRunParaPtr->ucCurWaitSecs = 1; 
                autoCodeRunParaPtr->autoCodeNextStep = AUTOCODE_ASK05H_STEP; //2.0.1.0[1] 
                break;
            case AUTOCODE_ASK05H_STEP:
                AutoCodeCanSend(0x05);
                autoCodeRunParaPtr->ucCurWaitSecs = 1; 
                autoCodeRunParaPtr->autoCodeNextStep = AUTOCODE_ASK06H_STEP; //2.0.1.0[1] 
                break;
            case AUTOCODE_ASK06H_STEP:
                AutoCodeCanSend(0x06);
                autoCodeRunParaPtr->ucCurWaitSecs = 1; 
                autoCodeRunParaPtr->autoCodeNextStep = AUTOCODE_ASK07H_STEP; //2.0.1.0[1] 
                break;
            case AUTOCODE_ASK07H_STEP:
                AutoCodeCanSend(0x07);
                autoCodeRunParaPtr->ucCurWaitSecs = 1; 
                autoCodeRunParaPtr->autoCodeNextStep = AUTOCODE_ASK0CH_STEP; //2.0.1.0[1] 
                break;
            case AUTOCODE_ASK0CH_STEP:
                autoCodeRunParaPtr->ucParaReplyNum = 0;
                AutoCodeCanSend(0x0C);
                autoCodeRunParaPtr->ucCurWaitSecs = 2; 
                autoCodeRunParaPtr->autoCodeNextStep = AUTOCODE_ASK24H_STEP; //2.0.1.0[1] 
                break;
            case AUTOCODE_ASK24H_STEP:
                autoCodeRunParaPtr->ucVolReplyNum = 0;
                AutoCodeCanSend(0x24);
                autoCodeRunParaPtr->ucCurWaitSecs = AUTOCODE_ASK24_STEP_SEC;
                autoCodeRunParaPtr->autoCodeNextStep = AUTOCODE_CODE_STEP; //2.0.1.0[1] 
                break;                                    
            case AUTOCODE_CODE_STEP:
                AutoCodeSetModuleNum();
                if (AutoCodeCheck() == 0) {
                    AutoCodeCwDownCodeVol();                  
                }
               
                if (autoCodeRunParaPtr->CODE_STATUS != AUTOCODE_OK_STATUS) {
                    if (autoCodeRunParaPtr->ucCodeCounter != CODE_REPEATE_COUNT) {
                        autoCodeRunParaPtr->ucCodeCounter++;
                    }
                    if (autoCodeRunParaPtr->ucCodeCounter == CODE_REPEATE_COUNT) {
                        autoCodeRunParaPtr->CODE_STATUS = AUTOCODE_NG_STATUS;
                        autoCodeRunParaPtr->CODE_FAILED = TRUE;  
                        autoCodeRunParaPtr->autoCodeNextStep = AUTOCODE_OVER_STEP; //2.0.1.0[1]                   
                    } else {
                        AutoCodeRunParaClr();
                        autoCodeRunParaPtr->autoCodeNextStep = AUTOCODE_DETCT_STEP; //2.0.1.0[1] 
                        autoCodeRunParaPtr->ucCurWaitSecs = 5;  
                    }           
                } else { // stAutoCodeRunPara.ucCodeStatus == AUTOCODE_OK_STATUS
                    autoCodeRunParaPtr->autoCodeNextStep = AUTOCODE_OVER_STEP; //2.0.1.0[1]   
                }
                AutoCodeCanSend(0x08);
                FdNorInit();
            default:
                break;
            }   
        }
    }
}