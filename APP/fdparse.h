#ifndef _FD_PARSE_H_
#define _FD_PARSE_H_

#include "stdtype.h"
#include "stdint.h"

extern BOOL FdParse(STR_CAN_STD_TYPE *ptr);
void CanExdInit(STR_CAN_STD_TYPE *canf, UINT32 uwExdId);
#endif          
