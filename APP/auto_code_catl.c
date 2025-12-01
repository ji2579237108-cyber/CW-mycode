#include "auto_code_catl.h"
#include "cf.h"
#include "app.h"

#define AUTOCODE_ASK90_STEP_SEC 5
#define AUTOCODE_ASK91_STEP_SEC 0
#define AUTOCODE_CODE_STEP_SEC 3
 
void AutoCodeCatlDownCodeVol(void) 
{
    UINT8 i;
    UINT8 ucFalg;
    STR_CAN_STD_TYPE stCanf;
    
    CanExdInit(&stCanf, 0x1A92F5F4); //V2.1.0.0 
    ucFalg =  autoCodeRunParaPtr->CODE_ORDER == 1 ? 0 : 1;
    bubble_sort(autoCodeRunParaPtr->usArrayCodeVol, 
        autoCodeRunParaPtr->ucVolReplyNum, ucFalg);  
    for (i = 0; i < autoCodeRunParaPtr->ucVolReplyNum; i++) {
        stCanf.data[0] = (UINT8)(autoCodeRunParaPtr->usArrayCodeVol[i] & 0xff);
        stCanf.data[1] = (UINT8)(autoCodeRunParaPtr->usArrayCodeVol[i] >> 8);
        stCanf.data[2] = i + 1;
        stCanf.len = 3;  
        _IFR_CAN_IN_SEND(&stCanf);   
        _IFR_MSLEEP(3);  
    }
}

void AutoCodeCtalProcess(void) 
{
    if (autoCodeRunParaPtr->ucCurWaitSecs != 0) {
        autoCodeRunParaPtr->ucCurWaitSecs--;
    }
    if (autoCodeRunParaPtr->ucCurWaitSecs == 0) {
        if (autoCodeRunParaPtr->autoCodeStep != autoCodeRunParaPtr->autoCodeNextStep) { //2.0.1.0[1]
            autoCodeRunParaPtr->autoCodeStep = autoCodeRunParaPtr->autoCodeNextStep;
        }
        switch (autoCodeRunParaPtr->autoCodeStep) {
        case AUTOCODE_IDLE_STEP:
        case AUTOCODE_OVER_STEP:
                   
        case AUTOCODE_DETCT_STEP:           
            break;
        case AUTOCODE_ASK90_STEP:
            AutoCodeCanSend(0x90);
            autoCodeRunParaPtr->autoCodeNextStep = AUTOCODE_ASK91_STEP; //2.0.1.0[1]
            autoCodeRunParaPtr->ucCurWaitSecs   = AUTOCODE_ASK91_STEP_SEC;
            break;
        case AUTOCODE_ASK91_STEP:
            AutoCodeCanSend(0x91);
            autoCodeRunParaPtr->autoCodeNextStep = AUTOCODE_CODE_STEP; //2.0.1.0[1]
            autoCodeRunParaPtr->ucCurWaitSecs   = AUTOCODE_CODE_STEP_SEC;  
            break; 
        case AUTOCODE_CODE_STEP:
            AutoCodeSetModuleNum(); //自适应探测器个数
            if (AutoCodeCheck() == 0) {
                AutoCodeCatlDownCodeVol();
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
                    autoCodeRunParaPtr->ucCurWaitSecs = 2;                    
                }           
            } else {
                autoCodeRunParaPtr->autoCodeNextStep = AUTOCODE_OVER_STEP; //2.0.1.0[1]   
            }
            break; 
        default:
            break;
        }
    }
}


