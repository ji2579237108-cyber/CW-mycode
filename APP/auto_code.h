#ifndef _AUTO_CODE_H_
#define _AUTO_CODE_H_

#include "stdtype.h"
#include "auto_code_catl.h"

#define CODE_VER_UNK     0
#define CODE_VER_CW      1
#define CODE_VER_CATL    2
//编码重复次数
#define CODE_REPEATE_COUNT  3u
//上电后延时时间
#define POWER_ON_DELAY 5u

#define AUTOCODE_UNK_STATUS 0
#define AUTOCODE_OK_STATUS  1
#define AUTOCODE_NG_STATUS  2

#define AUTOCODE_OK 0
#define AUTOCODE_REPEATE 1
#define AUTOCODE_FAIL 2


typedef enum {
    AUTOCODE_IDLE_STEP = 0, 
    AUTOCODE_DETCT_STEP, 
    
    AUTOCODE_ASK90_STEP,
    AUTOCODE_ASK91_STEP,
    
    AUTOCODE_ASK04H_STEP,
    AUTOCODE_ASK05H_STEP,
    AUTOCODE_ASK06H_STEP,
    AUTOCODE_ASK07H_STEP,
    AUTOCODE_ASK0CH_STEP,
    AUTOCODE_ASK24H_STEP,
    
    AUTOCODE_CODE_STEP,
    
    AUTOCODE_OVER_STEP 
} AUTO_CODE_STEPS;

typedef union {
    UINT16 usStatus;
    struct {
        UINT16 bB0Recv               : 1; 
        UINT16 bModuleNumInvalid     : 1;
        UINT16 bCodeFailed           : 1;
        UINT16 bCodeOrder            : 1;
        UINT16 bIdStatus             : 2;
        UINT16 bCodeStatus           : 2;
        UINT16 bCodeLineFault        : 1;
    } statusBits;
} U_AUTOCODE_STATUS;

typedef struct {
    UINT8 ucAutoCodeVersion;
    AUTO_CODE_STEPS autoCodeStep;
    AUTO_CODE_STEPS autoCodeNextStep; //2.0.1.0[1]
    UINT8 ucParaModuleMax;
    UINT8 ucParaModuleMin;
    UINT8 ucParaModuleNum;
    UINT8 ucParaReplyNum;
    UINT8 ucVolReplyNum;  
    UINT8 ucCodeCounter; 
    UINT8 ucCurWaitSecs; 
    U_AUTOCODE_STATUS uAutoCodeStatus;
    UINT8 ucArrayDevId[20];
    UINT16 usArrayCodeVol[20];
    UINT8 ucB0Ctr;
    UINT8 ucCatlB0Ctr;
    UINT8 ucCwB0Ctr;
    INT16 ssB0TimeOut;
    
//#define B0_RECV             uAutoCodeStatus.statusBits.bB0Recv
#define MODULE_NUM_INVALID  uAutoCodeStatus.statusBits.bModuleNumInvalid 
#define CODE_ORDER          uAutoCodeStatus.statusBits.bCodeOrder
#define ID_STATUS           uAutoCodeStatus.statusBits.bIdStatus
#define CODE_STATUS         uAutoCodeStatus.statusBits.bCodeStatus
#define CODE_FAILED         uAutoCodeStatus.statusBits.bCodeFailed
#define CODE_LINE_FAULT     uAutoCodeStatus.statusBits.bCodeLineFault

} STR_AUTO_CODE_RUN_PARA;
extern STR_AUTO_CODE_RUN_PARA* const autoCodeRunParaPtr;

void AutoCodeInit(void);
void AutoCodeVersionDetectByB0(STR_CAN_STD_TYPE *canf);
void AutoCodeCanSend(UINT8 ucCommand);
void AutoCodeRunParaClr(void);
UINT8 AutoCodeLineCheck(UINT8 ucModuleNum, UINT8 ucOrder);
void AutoCodeSetModuleNum(void);
UINT8 AutoCodeCheck(void);
void AutoCodeMainProcess(void);
void AutoCodeEnable(void);
void AutoCode0ChRecv(STR_CAN_STD_TYPE *canf);
void AutoCode24hRecv(STR_CAN_STD_TYPE *canf);
void AutoCodeC7hRecv(STR_CAN_STD_TYPE *canf);
void AutoCode90hRecv(STR_CAN_STD_TYPE *canf);
void AutoCode91hRecv(STR_CAN_STD_TYPE *canf);
void AutoCode92hRecv(STR_CAN_STD_TYPE *canf);
BOOL AutoCodeIsIdle(void);
void AutoCodeDisable(void);

/*
 * @ModuleName: AutoCode_IdRepChk
 * @Version   : 0.1
 */
#define AUTOCODE_ID_NONE_ERR  0x00u
#define AUTOCODE_ID_EMPTY_ERR 0x01u
#define AUTOCODE_ID_00_ERR    0x02u
#define AUTOCODE_ID_FF_ERR    0x04u
#define AUTOCODE_ID_REP_ERR   0x08u
UINT8 AutoCode_IdRepChk(UINT8 *ucIdPtr, UINT8 ucCnt); 

UINT8 AutoCodeLineError(void); //V2.1.0.0
void AutoCodeVersionDetect(void);
#endif /* _AUTO_CODE_H_ */
