#ifndef _CJCRC_H_
#define _CJCRC_H_

#include "stdtype.h"

extern void CjCrc32Reset(void);
extern void CjCrc32Cal(UINT8 ucDat);
extern UINT32 CjCrc32Result(void);
extern UINT32 CjCrc32CalBuffer(const void *buf, int count);

#endif /* _CJCRC_H_ */
