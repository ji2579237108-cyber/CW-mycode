#ifndef _GLOBESTA_CTRL_H
#define _GLOBESTA_CTRL_H


#include "stdtype.h"

typedef struct 
{
	UINT8 _ucModeFlg;
	UINT8 _ucModeCounter;
}MODE_CTRL_STR;

extern void avdMasterProcess(void);

#endif
