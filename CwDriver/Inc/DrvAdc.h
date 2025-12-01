/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DrvAdc.h
作者: JH
版本号:1.0
生成日期:2023.4.11
概述: ADC驱动
修改日志：
*********************************************************************/
#ifndef _DRVADC_H
#define _DRVADC_H
#include "DataTypeDef.h"
#include "gd32f30x_libopt.h"

typedef enum
{
    ADC_0 = 0,
    ADC_1,
    ADC_2,
    ADC_PERIPH_NUM,
}ADC_PERIPH;

typedef struct
{
    uint8_t Index;
    uint8_t Channel;
}CHANNEL_CONFIG;

typedef struct
{
    uint32_t ADCx;
    uint32_t CLKx;
    uint32_t Resolution;
    uint32_t DMAx;
    rcu_periph_enum  RCUx;
    rcu_periph_enum  DMA_RCU;
    dma_channel_enum DMA_CHx;
}ADC_RESOURCE;

extern void DrvAdcGpioInit(uint32_t GpioPort, uint32_t GpioPin);
extern void DrvAdcRcuConfig(ADC_PERIPH Periph);
extern void DrvADCNormalInit(ADC_PERIPH Periph);
extern S8 DrvADCUseDmaInit(ADC_PERIPH Periph, uint8_t DMAChNum, const CHANNEL_CONFIG* ConfigPtr);
extern uint32_t DrvAdcChannelSample(ADC_PERIPH Periph, uint8_t Channel);
extern uint32_t DrvGetDmaChannelAdValue(uint8_t Channel);

#endif

