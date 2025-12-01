#ifndef _PUMP_H_
#define _PUMP_H_

#include "stdtype.h"

extern void PumpInit(void);
extern void PumpOpen(void);
extern void PumpClose(void); 
extern UINT8 PumpStatus(void);
extern void PumpValveInit(void); 
extern void ValveOpen(void);
extern void ValveClose(void);
extern UINT8 PfhValveStatus(void); 

#endif /* _PUMP_H_ */
