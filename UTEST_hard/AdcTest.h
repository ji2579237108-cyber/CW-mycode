/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：ADC_Test.h
作者: LXD
版本号:1.0
生成日期:2023.4.21
概述:  针对GD32F30X的ADC驱动测试:
			使用ADC0 DMA采集引脚电压，并通过分压电阻计算实际电压。
			使用串口输出采集电压，或使用JLINK_RTT输出采集电压
修改日志：
*********************************************************************/

#ifndef ADCTEST_H
#define ADCTEST_H

#include "CWDrv.h"
#include "GpioMap.h"
#include "AdcCfg.h"
#include "MidAdc.h"

void  TestAdcSampleDisplay(void);

#endif
