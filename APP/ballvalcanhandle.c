#include "ballvalcanhandle.h"
#include "para.h"
static STR_BALL_VALVE stBallvalves[DETECTOR_NBR] = { 0 };


STR_BALL_VALVE *BallValPtr (UINT8 ucDeviceID)
{
    if (ucDeviceID > 0 && ucDeviceID <= DETECTOR_NBR)
    {
		return &stBallvalves[ucDeviceID - 1];
	} 
    else 
    {
		return NULL;
	}
}