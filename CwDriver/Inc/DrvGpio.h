/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DrvGpio.h
作者: JH
版本号:1.0
生成日期:2023.4.4
概述: Gpio驱动
修改日志：
*********************************************************************/
#ifndef _DRVGPIO_H
#define _DRVGPIO_H
#include "DataTypeDef.h"
#include "gd32f30x.h"

typedef void (*GpioIntCallBack)(void);

extern void DrvSWDInit(void);
extern void DrvGpioInit(uint32_t Port, uint32_t Pin, uint8_t GpioMode);
extern void DrvGpioOperat(uint32_t Port, uint32_t Pin, BOOL Set);
extern void DrvGpioExitSetSource(uint8_t SrcPort, uint8_t SrcPin, exti_trig_type_enum Type);
extern void DrvSetExitCallBack(uint8_t SrcPin, GpioIntCallBack CallBackFunc);
extern BOOL DrvGpioGetInputBit(uint32_t Port, uint32_t Pin);
extern BOOL DrvGpioGetOutputBit(uint32_t Port, uint32_t Pin);

#endif

