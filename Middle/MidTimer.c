/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：TIMER_Test.c
作者: LXD
版本号:1.0
生成日期:2023.4.22
概述:  针对GD32F30X的定时器驱动测试：
			使用307的定时器1，并使用中断回调函数。
修改日志：
*********************************************************************/
#include "MidTimer.h"
#include "message.h"
#include "DioTest.h"
#include "time.h"


uint8_t gucWorkMode = 0; 
/**
 * @brief LED灯闪烁，证明程序正常运行
 *
 * @param 
 */
void TestRunLed(uint8_t Sta)
{
	if(Sta == 0 )
	{
		DioWriteChannel(LED_GREEN,  0);
	}
	else 
	{
		DioWriteChannel(LED_GREEN,  1);
	}

}

/**
 * @brief 定时器1 中断回调函数
 *
 * @note 每隔1秒 打印 "timer1"
 */
void Timer1_CallCack()
{
	static U16 count =0;
	count++;
	if(count == 100)//1s
	{
		count = 0;
		TestRunLed(0);
		tTimeStamp++;
		tSysTime++;
	}
	else
	{
		TestRunLed(1);
	}
	MessageSend();
}


/**
 * @brief 定时器1 初始化函数
 *
 * @note 1us为单位，20ms为一中断周期
 */
void Timer1_Init()
{
	DrvTimerInit(TIMER_1,100,10);//1us为单位，10ms一周期
	DrvTimerInterruptSwitch(TIMER_1, TIMER1_IRQn, TIMER_INT_UP);
	DrvSetInterruptCallBack(TIMER_1, Timer1_CallCack);
}


