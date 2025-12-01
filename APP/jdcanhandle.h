#ifndef __JDCANHANDLE_H__
#define	__JDCANHANDLE_H__

//#include "swp_types.h"
#include "jddetector.h"
#include "stdtype.h"
BOOL JdCanParse (STR_CAN_STD_TYPE *canf);
void Jd51CanParse (STR_CAN_STD_TYPE *canf, STR_JD_DETECTOR *jdptr);
void Jd52CanParse (STR_CAN_STD_TYPE *canf, STR_JD_DETECTOR *jdptr);
void Jd54CanParse (STR_CAN_STD_TYPE *canf, STR_JD_DETECTOR *jdptr);
void Jd69CanParse(STR_CAN_STD_TYPE *canf);
void ValeCtr(UINT8 addr,STR_JD_DETECTOR *dptr);
void JdTempBufInit(void);
void JdTempAppend(float temp, float *p);
#endif