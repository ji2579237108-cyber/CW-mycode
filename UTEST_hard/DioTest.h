/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DIDO_Test.h
作者: LXD
版本号:1.0
生成日期:2023.4.21
概述:  针对GD32F30X的GPIO驱动测试:
			进行输入输出引脚的 时钟使能，引脚初始化。
			控制引脚输出高低电平
		注意：PB4  PA15引脚需要做重映射。
修改日志：
*********************************************************************/

#ifndef DIOTEST_H
#define DIOTEST_H

#include "CWDrv.h"
#include "DioCfg.h"
#include "GpioMap.h"
#include "MidDio.h"

extern uint8_t gucWorkMode;  


void TestDioAllUp(void);
void TestRunLed(uint8_t Sta);
void CheckModeLED(void);
#endif
