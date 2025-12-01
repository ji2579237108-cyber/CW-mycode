#ifndef _PWR_NODE_H_
#define _PWR_NODE_H_

#include "selfdiag.h"


void Diag12v(void);
void Diag5v(void);
void Diag3v(void);

ENU_DETCT_RESULT Detct24vH(void);
ENU_DETCT_RESULT Detct24vL(void);
ENU_DETCT_RESULT Detct5vH(void);
ENU_DETCT_RESULT Detct5vL(void);
ENU_DETCT_RESULT Detct3vH(void);
ENU_DETCT_RESULT Detct3vL(void);

BOOL Confirm24vH(BOOL bFault);
BOOL Confirm24vL(BOOL bFault);
BOOL Confirm5vH(BOOL bFault);
BOOL Confirm5vL(BOOL bFault);
BOOL Confirm3vH(BOOL bFault);
BOOL Confirm3vL(BOOL bFault);



UINT8 PowerFaultIs12H(void);
UINT8 PowerFaultIs12L(void);
UINT8 PowerFaultIs5H(void);
UINT8 PowerFaultIs5L(void); 
UINT8 PowerFaultIs3H(void); 
UINT8 PowerFaultIs3L(void);




void DiagLevel(void);
ENU_DETCT_RESULT DetctLevelLow(void);
BOOL ConfirmLevelLow(BOOL bFault); 

void DiagBatLow(void);
ENU_DETCT_RESULT DetctBatLow(void);
BOOL ConfirmBatLow(BOOL bFault); 


void DiagValueShortOrOpen(void);
ENU_DETCT_RESULT DetctValueShortCir(void);
ENU_DETCT_RESULT DetctValueOpenCir(void);
BOOL ConfirmValueOpen(BOOL bFault);
BOOL ConfirmValueShort(BOOL bFault) ;


void DiagFDShortOrOpen(void);
ENU_DETCT_RESULT DetctFDShortCir(void);
ENU_DETCT_RESULT DetctFDOpenCir(void);
BOOL ConfirmFDOpen(BOOL bFault);
BOOL ConfirmFDShort(BOOL bFault) ;


#endif /* _PWR_NODE_H_ */ 
