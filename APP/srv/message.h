#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "stdtype.h"

typedef void (*P_MESSAGE_PROC)(void) ;

void MessageSend(void);
void MessageProcess(void);
BOOL MessageInit(void);
BOOL MessageConfigTicks(UINT16 usMsTicks);
BOOL MessageRegister(P_MESSAGE_PROC pMessageProc, UINT16 usInterTime);//×¢²áÏûÏ¢ 

#endif
