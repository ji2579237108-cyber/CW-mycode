/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DrvTimer.h
作者: JH
版本号:1.0
生成日期:2023.4.12
概述: Timer驱动
修改日志：
*********************************************************************/
#ifndef _DRVTIMER_H
#define _DRVTIMER_H
#include "DataTypeDef.h"
#include "gd32f30x.h"

typedef void (*TimeCallBack)(void);  ///< 定时器中断回调函数类型

typedef enum
{
    TIMER_0 = 0,
    TIMER_1,
    TIMER_2,
    TIMER_3,
    TIMER_4,
    TIMER_5,
    TIMER_6,
    TIMER_7,
    TIMER_8,
    TIMER_9,
    TIMER_10,
    TIMER_11,
    TIMER_12,
    TIMER_PERIPH_NUM,
}TIMER_PERIPH;

typedef struct
{
    U16 mChannel;
    U16 mDutycycle;     ///< 占空比
    U16 mPwmMode;       ///< 输出Pwm模式，TIMER_OC_MODE_PWM0/...
    U16 mPwmCapMode;    ///< 捕获模式，TIMER_IC_POLARITY_RISING/TIMER_IC_POLARITY_FALLING
}PWM_CHANNEL_CONFIG;

/**
 * @brief 定时器的系统参数对应结构表
 *
 */
typedef struct
{
    uint32_t				mTIMx;				///< 定时器基地址
    rcu_periph_enum	mEnableTime;		///< 定时器时钟 
    uint32_t				mIsrNum;			///< 中断通道号    
    TimeCallBack	mTimeCallBack;		///< 指向回调函数
	uint32_t				mBasicUnity;		///< 基本单位，不分频/US/MS
}DRV_TIME;

extern void DrvTimerGpioConfig(uint32_t GpioPort, uint32_t Pin, BOOL Mode);
extern void DrvTimerInit(TIMER_PERIPH Periph, U16 BasicUnity, U16 Period);
extern void DrvTimerPwmOutputInit(TIMER_PERIPH Periph, U16 BasicUnity, U16 Period, PWM_CHANNEL_CONFIG* ConfigPtr, uint8_t ChannelNum);
extern uint32_t DrvTimerGetCounter(TIMER_PERIPH Periph);
extern void DrvTimerInterruptSwitch(TIMER_PERIPH Periph, uint32_t IsrNum, uint32_t IrqType);
extern void DrvTimerSetWork(TIMER_PERIPH Periph, BOOL Enable);
extern void DrvSetInterruptCallBack(TIMER_PERIPH Periph, TimeCallBack CallBack);
extern S32 DrvTimerClrCounter(TIMER_PERIPH Periph);
#endif

