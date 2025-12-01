#ifndef _C_CRC_H_
#define _C_CRC_H_

#include "stdtype.h"

extern UINT16 CRC16(const void *buffer, int count);
extern UINT16 CallSumCRC16(const void *buffer, int count);
extern UINT8 CrcAccNegation(const void *buffer, int count);

#endif


