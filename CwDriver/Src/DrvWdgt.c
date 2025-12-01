/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DrvWdgt.c
作者: JH
版本号:1.0
生成日期:2023.4.4
概述:  看门狗驱动
修改日志：
*********************************************************************/
#include "DrvWdgt.h"
#include "SEGGER_RTT.h"
static uint8_t sWwdgCout = 0x7f;						        ///< 窗口看门狗周期中断重载值

typedef void (*WATCHDOG_IntCallBack)(void);				///< 看门狗回调处理类型
static WATCHDOG_IntCallBack sWatchDogCallBack = NULL;	///< 看门狗中断回调函数指针


/**
 * @brief 窗口看门狗回调
 *
 * @param
 * @return
 */
void WWdgtIntCallBack(void)
{
    //DEBUG_LOG_INFO(DEBUG_LEVEL_2, "WWdgt Interrupt\n");
}
void WWdgtTest(void)
{
		//DrvFwdgtInit(500, FWDGT_PSC_DIV256);     
    //DrvWatchDogIntCallBackSet(WWdgtIntCallBack);
    //DrvWatchDogWarningIntEn(126, 1);
}


/**
 * @brief 独立看门狗初始化
 *
 * @param  LoadValue  加载值 0x0000 - 0x0FFF
 * @param  PscValue   分频值 FWDGT_PSC_DIV256,  256分频, 156Hz, 6ms
 * @brief 
 * @return 
 */
void DrvFwdgtInit(uint32_t LoadValue, uint8_t PscValue)
{
	rcu_osci_on(RCU_IRC40K);

    while(SUCCESS != rcu_osci_stab_wait(RCU_IRC40K)){};   //等待起振稳定
    
	fwdgt_config(LoadValue,PscValue);                    //40KHz/ 64 = 0.625 KHz 
    
    fwdgt_enable();

    fwdgt_write_enable();
}

/**
 * @brief 喂狗
 *
 * @param 
 * @brief 
 * @return 
 */
void DrvFeedFwdgt(void)
{
	fwdgt_write_disable();

	fwdgt_counter_reload();

	fwdgt_write_enable();
}

/**
 * @brief 关闭看门狗
 *
 * @param  
 * @param 
 * @brief 
 * @return 
 */
void DrvCloseFwdgt(void)
{
	fwdgt_write_disable();

	fwdgt_counter_reload();

	fwdgt_write_enable();

	rcu_osci_off(RCU_IRC40K);
}


/**
 * @brief 窗口看门狗中断函数入口
 *
 */
void WWDGT_IRQHandler(void)
{
	wwdgt_counter_update(sWwdgCout);		        	///< 重设窗口看门狗值
	wwdgt_flag_clear();				                    ///< 清除提前唤醒中断标志位
	
	if(sWatchDogCallBack != NULL)
	{
		sWatchDogCallBack();
	}

}


/**
 * @brief	设置看门狗中断回调函数
 *
 * @param	IntCallBack       中断回调函数
 * @return	成功返回0，失败返回非0
 */
S32 DrvWatchDogIntCallBackSet(void (*IntCallBack)(void))
{
	if(IntCallBack != NULL)
	{
		sWatchDogCallBack = IntCallBack;
		return 0;
	}
	else
	{
		return -1;
	}
}


/**
 * @brief	看门狗报警中断使能设置
 *        
 * @param	WaringTime  报警时间，该时间为剩余时间，单位546us,  127
 * ~546 * (127-80)= 25.6ms < refresh window < ~546 * (127-63) =34.9ms.
 * @param	Enable      使能/禁止，使用宏ENABLE,DISABLE
 * @return	成功返回0， 失败返回非0
 */
S32 DrvWatchDogWarningIntEn(uint32_t WaringTime, uint8_t Enable)
{
	//uint32_t APB1Clk;
	
	if(Enable)
	{	
		wwdgt_deinit();		
		
		//APB1Clk = rcu_clock_freq_get(CK_APB1);					///< 看门狗定时器由APB1提供
			
		if(WaringTime > 0x7F)
		{
			return -1;
		}

		nvic_irq_enable(WWDGT_IRQn, 0, 1);
		rcu_periph_clock_enable(RCU_WWDGT);						///< 使能完时钟后, 计数器CNT即开始工作				
		wwdgt_enable();											///< 使能窗口看门狗
		wwdgt_config(WaringTime, \
					(80), \
					 WWDGT_CFG_PSC_DIV8);						///< 配置窗口看门狗参数
		wwdgt_flag_clear();										///< 清除EWIF
		wwdgt_interrupt_enable();								///< 使能EWI中断, 注意需在最后使能中断, 否则会出现配置完即复位
	}
	else
	{
		sWwdgCout = 0x7f;
		wwdgt_flag_clear();										///< 清除EWIF
		wwdgt_deinit();											///< 去初始化
		rcu_periph_clock_disable(RCU_WWDGT);					///< 禁止时钟
	}	
	
	return 0;
}
