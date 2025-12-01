#ifndef MIDCAN_H
#define MIDCAN_H

#include "DrvCAN.h"
#include "CWDrv.h"



void CanInit(UINT8 Can0Baud, UINT8 Can1Baud);


void ExCan0Send(STR_CAN_STD_TYPE *can_ptr);
void InCan1Send(STR_CAN_STD_TYPE *can_ptr);
void InCan1Send_UpGrade(STR_CAN_STD_TYPE *can_ptr);
#endif
