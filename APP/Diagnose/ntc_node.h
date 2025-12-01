#ifndef _NTC_NODE_H_
#define _NTC_NODE_H_

#include "selfdiag.h"



void Ntc1SigHighDiag(void);
void Ntc1SigLowDiag(void);
void Ntc2SigHighDiag(void);
void Ntc2SigLowDiag(void);
void NtcSigFaultDiag(void);
ENU_DETCT_RESULT Ntc1SigHighDetct(void);
ENU_DETCT_RESULT Ntc1SigLowDetct(void);
ENU_DETCT_RESULT Ntc2SigHighDetct(void);
ENU_DETCT_RESULT Ntc2SigLowDetct(void);
ENU_DETCT_RESULT NtcSigFaultDetct(void);
BOOL Ntc1SigHighConfirm(BOOL bFault);
BOOL Ntc1SigLowConfirm(BOOL bFault);
BOOL Ntc2SigHighConfirm(BOOL bFault);
BOOL Ntc2SigLowConfirm(BOOL bFault);
BOOL NtcSigFaultConfirm(BOOL bFault);

UINT8 Ntc1_FaultIsH(void);
UINT8 Ntc1_FaultIsL(void);
UINT8 Ntc2_FaultIsH(void);
UINT8 Ntc2_FaultIsL(void);
UINT8 Ntc_FaultIsSignal(void);

#endif /* _NTC_NODE_H_ */ 
