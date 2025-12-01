/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：CAN_Test.h
作者: LXD
版本号:1.0
生成日期:2023.4.22
概述:  针对GD32F30X的CAN驱动测试：
			CAN0 CAN1 使用过滤器0，发送接收测试。
修改日志：
*********************************************************************/

#ifndef CANTEST_H
#define CANTEST_H

#include "CWDrv.h"
#include "MidCan.h"
	   

void TestCan0Trans(void);
void TestCan1Trans(void);

#endif


