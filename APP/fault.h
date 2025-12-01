#ifndef _FAULT_H_
#define _FAULT_H_

#include "stdtype.h"

#define DBUG_FAULT

typedef union {

    UINT16 usFault; 
      
    struct {
        UINT16 bRev7      : 1; //15
        UINT16 bRev6      : 1; //14   
        UINT16 bFDShort      : 1; //13      
        UINT16 bFDOpen      : 1; //12       
        UINT16 bValOpen      : 1; //11
        UINT16 bValShort      : 1; //10  
        UINT16 bPfhUsed   	: 1	; //9-È«·úÒÑÍªÒÑÊ¹ÓÃ       
        UINT16 bLevel          : 1; //8-ÒºÎ»¹ÊÕÏ
        UINT16 bFdValveUsed    : 1; //7-Ì½²âÆ÷Ãð»ðÆ÷ÒÑÊ¹ÓÃ   
        //UINT16 bFdFault   : 1; //6-Ì½²âÆ÷¹ÊÕÏ      
        UINT16 bFdHw      : 1; //5-Ì½²âÆ÷Ó²¼þ¹ÊÕÏ      
        UINT16 bFdOffline : 1; //4-Ì½²âÆ÷ÀëÏß¹ÊÕÏ
        UINT16 bFdSensor  : 1; //3-Ì½²âÆ÷´«¸ÐÆ÷¹ÊÕÏ      
        UINT16 bMoto      : 1; //2-±Ã¹ÊÕÏ 
        UINT16 bVoLow     : 1; //1-µç³ØµçÑ¹µÍ¹ÊÕÏ       
        UINT16 bCode      : 1; //0-±àÂëÏß¹ÊÕÏ 
    } ucFaultBits;
    
#define FAT_PFH_USED     ucFaultBits.bPfhUsed
#define FAT_LEVEL        ucFaultBits.bLevel 
#define FAT_FD_VALVE_USED   ucFaultBits.bFdValveUsed
//#define FAT_FDFAULT      ucFaultBits.bFdFault
#define FAT_FD_HARDWARE    ucFaultBits.bFdHw
#define FAT_FD_OFFLINE    ucFaultBits.bFdOffline
#define FAT_FD_SENSOR     ucFaultBits.bFdSensor
#define FAT_VOLLOW       ucFaultBits.bVoLow
#define FAT_MOTO         ucFaultBits.bMoto
#define FAT_CODELINE     ucFaultBits.bCode
	
#define FAT_VAL_SHORTCIR    ucFaultBits.bValShort //¶ÌÂ·
#define FAT_VAL_OPENCIR     ucFaultBits.bValOpen  //¿ªÂ·
#define FAT_FD_SHORTCIR     ucFaultBits.bFDOpen
#define FAT_FD_OPENCIR      ucFaultBits.bFDShort			
	
} STR_FAULT;





extern STR_FAULT gstFault;

void SysFaultInit(void); 
void SysFaultSet(void);
void FaultSetLevelLow(BOOL bFault);
void FaultSetBatLow(BOOL bFault);


void FaultSetVulShort(BOOL bFault) ;
void FaultSetVulOpen(BOOL bFault) ;
void FaultSetFDShort(BOOL bFault) ;
void FaultSetFDOpen(BOOL bFault) ;




UINT8 FaultIsBatLow(void);
UINT8 FaultIsLevelLow(void);
UINT8 FaultIsValOpenCircuit(void);
UINT8 FaultIsFDShortCircuit(void);




#endif /* _FAULT_H_ */ 

