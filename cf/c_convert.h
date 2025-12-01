#ifndef _C_CONVERT_H_
#define _C_CONVERT_H_

#include "stdtype.h"

extern void msb32_to_array(UINT32 uwData, unsigned char *array); 
extern void msb16_to_array(UINT16 usData, unsigned char *array);
extern void array_to_msb32(const void *buf, UINT32 *ptr);
extern void array_to_msb16(const void *buf, UINT16 *ptr);
extern UINT8 dec_to_bcd(UINT8 data);
extern UINT8 bcd_to_dec(UINT8 data);
UINT16 u16_frombe(const UINT8 *buf);

#endif
