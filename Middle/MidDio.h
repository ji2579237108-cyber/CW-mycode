#ifndef MIDDIO_H
#define MIDDIO_H

#include "DioCfg.h"
#include "GpioMap.h"

void DioInit(void);
void DioWriteChannel(ENU_GPIO_INDEX index,  uint8_t Set);
uint8_t DioReadChannel (ENU_GPIO_INDEX index);
void BEEP_ALARM_ON(void);
void BEEP_ALARM_OFF(void);


#endif
