#ifndef _SELFCHK_H_
#define _SELFCHK_H_

#include "stdtype.h"

//#define DBUG_SELFCHK

#define CHK_OP_ENTER 1 
#define CHK_OP_QUIT  0 

#define RUN_NOR_MODE 0x5AA5 
#define RUN_CHK_MODE 0xA55A 
typedef struct {
    union {  
        UINT8 ucCodeArray[8];
        struct {
            union {
                UINT8 ucCode1;
                struct {
                    UINT8 bExCan: 1; //0  
                    UINT8 bRev1 : 1; //1       
                    UINT8 bRev2 : 1; //2       
                    UINT8 bRev3 : 1; //3       
                    UINT8 bRev4 : 1; //4       
                    UINT8 bRev5 : 1; //5      
                    UINT8 bRev6 : 1; //6       
                    UINT8 bRev7 : 1; //7         
                } uCode1Bits;
            } uCode1;

            union {
                UINT8 ucCode2;
                struct {
                    UINT8 bPostVol:1;     //0
                    UINT8 bRev1:1;        //1
                    UINT8 bBefClampVol:1; //2
                    UINT8 bAftClampVol:1; //3
                    UINT8 b5V2:1;         //4
                    UINT8 bEeprom:1;      //5
					UINT8 bLevel:1;        //6
                    UINT8 bFireKey:1;     //7
                } uCode2Bits;
            } uCode2;
            
            union {
                UINT8 ucCode3;
                struct {
                    UINT8 bFlash:1; //0
                    UINT8 b4162:1;  //1
                    UINT8 bGsm:1;   //2
                    UINT8 bRev1:1;  //3
                    UINT8 bRev2:1;  //4
                    UINT8 bRev3:1;  //5
                    UINT8 bRev4:1;  //6
                    UINT8 bRev5:1;  //7
                } uCode3Bits;
            } uCode3;

            union {
                UINT8 ucCode4;
                struct {
                    UINT8 bHsd1:1; //0
                    UINT8 bHsd2:1; //1
                    UINT8 bHsd3:1; //2
                    UINT8 bHsd4:1; //3
                    UINT8 bHsd5:1; //4
                    UINT8 bHsd6:1; //5
                    UINT8 bHsd7:1; //6
                    UINT8 bHsd8:1; //7
                } uCode4Bits;
            } uCode4;
            union {
                UINT8 ucCode5;
                struct {
                    UINT8 bHsd9:1;  //0
                    UINT8 bHsd10:1; //1
                    UINT8 bHsd11:1; //2
                    UINT8 bHsd12:1; //3
                    UINT8 bRev1:1;  //4
                    UINT8 bRev2:1;  //5
                    UINT8 bRev3:1;  //6
                    UINT8 bRev4:1;  //7
                } uCode5Bits;     
            } uCode5;
            UINT8 ucCode6;
            UINT8 ucCode7;
            UINT8 ucCode8;
        } stCodes;
    } uCodes;
#define CODE_ARRAY    uCodes.ucCodeArray
#define CODE_EXCAN    uCodes.stCodes.uCode1.uCode1Bits.bExCan
#define CODE_EEPROM   uCodes.stCodes.uCode2.uCode2Bits.bEeprom
#define CODE_FIREKEY  uCodes.stCodes.uCode2.uCode2Bits.bFireKey
#define CODE_LEVEL    uCodes.stCodes.uCode2.uCode2Bits.bLevel
#define CODE_24V      uCodes.stCodes.uCode2.uCode2Bits.bBefClampVol
#define CODE_3V      uCodes.stCodes.uCode2.uCode2Bits.bAftClampVol
#define CODE_5V       uCodes.stCodes.uCode2.uCode2Bits.b5V2
#define CODE_FLASH    uCodes.stCodes.uCode3.uCode3Bits.bFlash
#define CODE_GSM      uCodes.stCodes.uCode3.uCode3Bits.bGsm            
} STR_SELF_CHK_INFO;


extern void SelfChkMainProcess(void);
extern void SelfChkInit(void);
extern BOOL IsSelfChkMode(void);
extern BOOL SelfChkOperate (UINT8 ucOp);
extern UINT8 SelfChkQuery(void *ptr, UINT8 len);
extern void SelfChk(void);

#endif /* _SELFCHK_H_ */
