#ifndef _NTC_H_
#define _NTC_H_

#include "ntc_tab.h"
#include "stdtype.h"
#include "MidAdc.h"

typedef struct {
    float k;
    float b;
} kb_t;

extern void NTC1GetValue(void* pDat);
extern void NTC2GetValue(void* pDat);



#endif
