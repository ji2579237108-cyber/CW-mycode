
#ifndef APPCONF_H
#define APPCONF_H

#include "stdtype.h"
#define CAN0_CHANEL   0
#define CAN1_CHANEL   1

typedef UINT8 T_CW_VERSION[4];


UINT8 MessageConf(void);

extern const T_CW_VERSION tCwAppVersion;
extern void CanOutFifoInit(void);
extern unsigned int CanInPutTask(STR_CAN_STD_TYPE *canf);
extern void repost(void);
extern void BroadCast(void);
extern void doOneSecStak(void);
#endif 


