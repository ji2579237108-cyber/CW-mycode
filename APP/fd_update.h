#ifndef __FD_UPDATE_H__
#define __FD_UPDATE_H__

#include "stdtype.h"
#include "stdint.h"
enum
{
    TRANSPARENT_DISABLE = 0,
    TRANSPARENT_ENABLE,
};

extern uint8_t gucFdUpdateFllag;

extern BOOL FdUpdateMain(STR_CAN_STD_TYPE *canf);
extern BOOL RecvFdData(STR_CAN_STD_TYPE *canf);
extern BOOL Transparent(STR_CAN_STD_TYPE *canf);
extern BOOL TransparentDebug(STR_CAN_STD_TYPE *canf);
extern BOOL FdVersion(STR_CAN_STD_TYPE *canf);
#endif /* _ZONEHANDLE_H_ */
