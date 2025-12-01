#ifndef MIDADC_H
#define MIDADC_H

#include "AdcCfg.h"

void AdcInit(void);
uint32_t AdcDmaValue(ENU_SAMPLE_INDEX eSample_IO);

#endif
