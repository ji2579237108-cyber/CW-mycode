/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DrvWdgt.h
作者: JH
版本号:1.0
生成日期:2023.4.4
概述: 
修改日志：
*********************************************************************/

#ifndef _DRVWDGT_H
#define _DRVWDGT_H
#include "DataTypeDef.h"
#include "gd32f30x_libopt.h"
#include "gd32f30x_rcu.h"

extern void DrvFwdgtInit(uint32_t LoadValue, uint8_t PscValue);
extern void DrvFeedFwdgt(void);
//extern void DrvCloseFwdgt(void);

extern S32 DrvWatchDogIntCallBackSet(void (*IntCallBack)(void));
extern S32 DrvWatchDogWarningIntEn(uint32_t WaringTime, uint8_t Enable);

#endif 
