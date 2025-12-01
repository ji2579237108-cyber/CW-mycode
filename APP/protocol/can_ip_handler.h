#ifndef _CAN_IP_HANDLER_H_ 
#define _CAN_IP_HANDLER_H_

#include "stdtype.h"
#include "stdint.h"
#include "stdbool.h"
extern void CanIpHandler(STR_CAN_STD_TYPE *canf);
extern void CanIpTmrSend(void);
extern void CanIP_FDid_Init(void);

#endif /* _CAN_IP_HANDLER_H_ */

