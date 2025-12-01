#ifndef __BALLVALCANHANDLE_H__
#define	__BALLVALCANHANDLE_H__
#include "stdtype.h"
#include "jddetector.h"
typedef struct {
    UINT8 ucBallValStatus;
    UINT8 ucBallValOpenStatus;
    UINT8 ucBallValCloseStatus;
    
} STR_BALL_VALVE;

STR_BALL_VALVE *BallValPtr (UINT8 ucId);

#endif

