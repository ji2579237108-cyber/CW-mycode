#ifndef _FD_H_
#define _FD_H_

#include "stdtype.h"
#include "stdint.h"

#define PACK_FD_NUM 	150

typedef union {
    UINT8 ucStatus;
    struct {
        UINT8 bLive           : 2;     //Ô¤Áô
        UINT8 bH2             : 1;
        UINT8 bSMOKE          : 1; 
        UINT8 bCO             : 1;
        UINT8 bVOC            : 1;
        UINT8 bTemp1          : 1;
        UINT8 bTemp2          : 1;       
    } bits;
} STR_FD_STATUS; 

typedef struct {
    STR_FD_STATUS fdStatus;
    
    UINT8  ucTemp1;      
    UINT8  ucTemp2;
    UINT16 ucCo;
    UINT16 ucVoc;
    UINT16 ucH2;
    UINT8 ucSmoken;    // ¼ÙÊý¾Ý
    UINT8  ucAlarm;
    UINT8  ucFault; 
    UINT8  ucCheckLine; 
    UINT32 ucOffLineCount;
#define FD_STATUS_H2            fdStatus.bits.bH2 //V0.1.0.0[13]+
#define FD_STATUS_CO            fdStatus.bits.bCO
#define FD_STATUS_SMOKE         fdStatus.bits.bSMOKE
#define FD_STATUS_VOC           fdStatus.bits.bVOC
#define FD_STATUS_TEMP1         fdStatus.bits.bTemp1
#define FD_STATUS_TEMP2         fdStatus.bits.bTemp2

} STR_FD_DATA;

extern STR_FD_DATA gstFdDataBuffer[PACK_FD_NUM];
void FdReset(void);
void FdInit(void);
void FdB0hHandler(STR_CAN_STD_TYPE *canf);
void Fd93hHandler(STR_CAN_STD_TYPE *canf);
#endif /* _FD_H_ */

