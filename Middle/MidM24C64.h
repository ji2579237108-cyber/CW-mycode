#ifndef _MIDM24C64_H_
#define _MIDM24C64_H_

#include "stdtype.h"
#include "DrvIIC.h"



void M24M_Init(void);
BOOL M24M_WriteBuff(UINT32 add,UINT8 *ptr,UINT16 writelen);
BOOL M24M_ReadBuff(UINT32 add,UINT8 *ptr,UINT16 writelen);
#endif

