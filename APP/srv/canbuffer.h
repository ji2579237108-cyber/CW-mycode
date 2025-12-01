#ifndef _CANBUFFER_H_
#define _CANBUFFER_H_ 

#include "stdtype.h"

#define CAN_BUFFER_SIZE 64

void CanBufferInit(void);
void CanBufferAppend(STR_CAN_STD_TYPE *ptr);
void CanBufferGetFirst(STR_CAN_STD_TYPE *ptr);
BOOL CanBufferIsEmpty(void);
//2.0.0.0[12]+
void OtCanBufferInit(void);
void OtCanBufferAppend(STR_CAN_STD_TYPE *ptr);
void OtCanBufferGetFirst(STR_CAN_STD_TYPE *ptr);
BOOL OtCanBufferIsEmpty(void);
//2.0.0.0[12]END

#endif /* _CANBUFFER_H_ */ 

