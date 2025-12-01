#ifndef _APPDEF_H_
#define _APPDEF_H_

#include "stdtype.h"

typedef UINT8 T_CW_VERSION[4];

extern volatile UINT32 guwSysClock;
extern UINT8 gucPdQrCode[];

void PdQrcodeRefresh(void);

#endif 

