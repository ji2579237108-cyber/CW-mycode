#ifndef AUTO_ADDRESSING_TOKEN_MASTER_H
#define AUTO_ADDRESSING_TOKEN_MASTER_H

#include "stdtype.h"
#include "DrvCAN.h"
#include "MidCan.h"
#include "MidDio.h"
#include "fd.h"

/*  the function :AutoCodeToken_MainProcess  excution time */ 
#define ENCODE_PROCESS_EXCUTE_TIME      1000 // (ms) 


typedef enum 
{
    POSITIVE_SEQUENCE = 0,
    NEGTIVE_SEQUENCE,
    SEQUENCE_MAXx
}ENU_SEQUENCE;

/**
 * @brief  coding step
 * 
 */
typedef enum 
{
    BROADCAST_START_CODE = 0,
    BROADCAST_TOKEN_REPLY,
    SET_TOKEN_ID,
    TOKEN_TRANS,
    TOKEN_CODE_FINISH,
    ENCODE_STEP_MAX
}ENU_CODE_STEP;


typedef struct 
{
    uint8_t         CunrrentID;                     // the id of dector whitch holds the token. the id number start from one. 
    uint8_t         CodeCompletedNumber;            // the number of detctor that completed coding
    uint8_t         CodeExpectNumber;               // the number of dector that user expect to code, 
    uint8_t         CurrentMode;                    // coding mode -ENU_CODE_STEP-five in all.
    ENU_SEQUENCE    sequence;                       // positive sequence or negive sequence
    uint8_t         EffictiveTime;                  // timeout, counter will accumulate,until timout ,then mode switch to TOKEN_CODE_FINISH.

}STR_CODE_DATA;



typedef enum 
{
    AUTOCODE_CLOSE,
    AUTOCODE_OPEN,

}ENU_CODE_MODE;



BOOL AutoCodeToken_Start(STR_CAN_STD_TYPE *ptr);

void AutoCodeToken_MainProcess (void);
     
void AutoCodeToken_DataRecive(STR_CAN_STD_TYPE *ptr);
extern BOOL AutoCodeHandler(STR_CAN_STD_TYPE *ptr);
void FDDebugMode(uint8_t Id);
void FDExDebugMode(void);

void FDClearError(void);
#endif


