/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DrvTimer.c
作者: JH
版本号:1.0
生成日期:2023.4.12
概述: Timer驱动
修改日志：
*********************************************************************/
#include "DrvTimer.h"

/**
 * @brief 定时器的时钟和中断号等对应表
 *
 */
static DRV_TIME sDevTime[] =
{
#ifdef GD32F30X_HD
    {TIMER0,	RCU_TIMER0,		TIMER0_UP_IRQn,		        NULL,	0x0},
    {TIMER1,	RCU_TIMER1,		TIMER1_IRQn,				NULL,	0x0},
    {TIMER2,	RCU_TIMER2,		TIMER2_IRQn,				NULL,	0x0},
    {TIMER3,	RCU_TIMER3,		TIMER3_IRQn,				NULL,	0x0},
    {TIMER4,	RCU_TIMER4,		TIMER4_IRQn,				NULL,	0x0},
    {TIMER5,	RCU_TIMER5,		TIMER5_IRQn,          	    NULL,	0x0},
    {TIMER6,	RCU_TIMER6,    	TIMER6_IRQn,              	NULL,	0x0},
    {TIMER7,	RCU_TIMER7,    	TIMER7_UP_IRQn,             NULL,	0x0},	 
#endif
#ifdef GD32F30X_CL
	{TIMER0,	RCU_TIMER0,		TIMER0_UP_TIMER9_IRQn,		NULL,	0x0},
	{TIMER1,	RCU_TIMER1,		TIMER1_IRQn,				NULL,	0x0},
    {TIMER2,	RCU_TIMER2,		TIMER2_IRQn,				NULL,	0x0},
    {TIMER3,	RCU_TIMER3,		TIMER3_IRQn,				NULL,	0x0},
    {TIMER4,	RCU_TIMER4,		TIMER4_IRQn,				NULL,	0x0},
    {TIMER5,	RCU_TIMER5,		TIMER5_IRQn,          	    NULL,	0x0},
    {TIMER6,	RCU_TIMER6,    	TIMER6_IRQn,              	NULL,	0x0},
#endif
	
};

uint8_t GetBit(uint32_t Regval)
{
    uint8_t i = 0;
	
	for(i=0; i<32; i++)
	{
		if((Regval>>i)&1)
		{
			return i;
		}
	}
	
    return 0;
}

void TimerItCallBackTest()
{
    #include "SEGGER_RTT.h"
    DEBUG_LOG_INFO(DEBUG_LEVEL_2, "Timer Interrupt Call Back\n");
}
void TimerTest()
{
    PWM_CHANNEL_CONFIG Config;

    Config.mChannel   = TIMER_CH_3;
    Config.mDutycycle = 250;
    Config.mPwmMode   = TIMER_OC_MODE_PWM0;

    DrvTimerGpioConfig(GPIOB, GPIO_PIN_9, 1);
    //gpio_pin_remap_config(GPIO_TIMER2_PARTIAL_REMAP,ENABLE);
    DrvTimerPwmOutputInit(TIMER_3, 1000, 1000, &Config, 1);
    DrvTimerInterruptSwitch(TIMER_3, TIMER3_IRQn, TIMER_INT_UP);
    DrvSetInterruptCallBack(TIMER_3, TimerItCallBackTest);
}

/**
 * @brief 定时器输入输出引脚配置
 *
 * @param BOOL Mode TRUE：输出 FALSE :输入
 *  
 * @brief 
 * @return 
 */
void DrvTimerGpioConfig(uint32_t GpioPort, uint32_t Pin, BOOL Mode)
{
    if(Mode)
    {
        gpio_init(GpioPort,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,Pin);
    }
    else
    {
        gpio_init(GpioPort,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_50MHZ,Pin);
    }
    
}

/**
 * @brief 定时器初始化
 *
 * @param BasicUnity 计数周期1us
 *  
 * @brief 
 * @return 
 */
void DrvTimerInit(TIMER_PERIPH Periph, U16 BasicUnity, U16 Period)
{
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(sDevTime[Periph].mEnableTime);

    timer_deinit(sDevTime[Periph].mTIMx);

    timer_initpara.prescaler         = 120*BasicUnity-1; //系统120M
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = (10*Period-1);
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(sDevTime[Periph].mTIMx,&timer_initpara);

    timer_auto_reload_shadow_enable(sDevTime[Periph].mTIMx);

    timer_enable(sDevTime[Periph].mTIMx);
}


/**
 * @brief 定时器通道配置
 *
 * @param PWM_CHANNEL_CONFIG 数组指针
 *  
 * @brief 可配置多个通道
 * @return 
 */
static void DrvTimerPwmOutputChannelConfig(TIMER_PERIPH Periph, PWM_CHANNEL_CONFIG* ConfigPtr, uint8_t ChannelNum)
{
	uint8_t i = 0;
    timer_oc_parameter_struct timer_ocintpara;

    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    for(i=0; i<ChannelNum; i++)
    {
        timer_channel_output_config(sDevTime[Periph].mTIMx,ConfigPtr[i].mChannel,&timer_ocintpara);
        timer_channel_output_pulse_value_config(sDevTime[Periph].mTIMx,ConfigPtr[i].mChannel,ConfigPtr[i].mDutycycle);
        timer_channel_output_mode_config(sDevTime[Periph].mTIMx,ConfigPtr[i].mChannel,ConfigPtr[i].mPwmMode);
        timer_channel_output_shadow_config(sDevTime[Periph].mTIMx,ConfigPtr[i].mChannel,TIMER_OC_SHADOW_DISABLE);
    }

}

/**
 * @brief 定时器Pwm输出初始化
 *
 * @param BasicUnity 计数周期单位/1us
 *  
 * @brief 
 * @return 
 */
void DrvTimerPwmOutputInit(TIMER_PERIPH Periph, U16 BasicUnity, U16 Period, PWM_CHANNEL_CONFIG* ConfigPtr, uint8_t ChannelNum)
{
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(sDevTime[Periph].mEnableTime);

    timer_deinit(sDevTime[Periph].mTIMx);

    timer_initpara.prescaler         = 120*BasicUnity-1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = Period;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(sDevTime[Periph].mTIMx,&timer_initpara);

    DrvTimerPwmOutputChannelConfig(Periph, ConfigPtr, ChannelNum);

    timer_auto_reload_shadow_enable(sDevTime[Periph].mTIMx);

    timer_enable(sDevTime[Periph].mTIMx);
}

/**
 * @brief 定时器PWM捕获
 *
 * @param 
 *  
 * @brief 有捕获功能定时器，通道选择0/1
 * @return 
 */
void DrvTimerCapturePwm(TIMER_PERIPH Periph, U16 BasicUnity, PWM_CHANNEL_CONFIG* ConfigPtr)
{
    timer_ic_parameter_struct timer_icinitpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(sDevTime[Periph].mEnableTime);

    timer_deinit(sDevTime[Periph].mTIMx);

    timer_initpara.prescaler         = 120*BasicUnity-1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 65535;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(sDevTime[Periph].mTIMx,&timer_initpara);

    timer_icinitpara.icpolarity  = ConfigPtr->mPwmCapMode;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter    = 0x0;
    timer_input_pwm_capture_config(sDevTime[Periph].mTIMx,ConfigPtr->mChannel,&timer_icinitpara);

    timer_input_trigger_source_select(sDevTime[Periph].mTIMx,TIMER_SMCFG_TRGSEL_CI0FE0);
    timer_slave_mode_select(sDevTime[Periph].mTIMx,TIMER_SLAVE_MODE_RESTART);

    timer_master_slave_mode_config(sDevTime[Periph].mTIMx,TIMER_MASTER_SLAVE_MODE_ENABLE);

    timer_auto_reload_shadow_enable(sDevTime[Periph].mTIMx);

    //timer_interrupt_flag_clear(sDevTime[Periph].mTIMx,TIMER_INT_FLAG_CH0);

    //timer_interrupt_enable(sDevTime[Periph].mTIMx,TIMER_INT_CH0);

    timer_enable(sDevTime[Periph].mTIMx);
}

/**
 * @brief	获取定时的计数值
 *
 * @param	
 * @return	返回定时器计数值
 */
uint32_t DrvTimerGetCounter(TIMER_PERIPH Periph)
{
    return timer_counter_read(sDevTime[Periph].mTIMx);
}

/**
 * @brief	清除定时器计数值
 *
 * @param	
 * @return 
 */
S32 DrvTimerClrCounter(TIMER_PERIPH Periph)
{
    timer_counter_value_config(sDevTime[Periph].mTIMx, (uint32_t)0x0);

    return 0;
}

/**
 * @brief	开关定时器
 *
 * @param	
 * @return	
 */
void DrvTimerSetWork(TIMER_PERIPH Periph, BOOL Enable)
{
    if(Enable)
    {
        timer_enable(sDevTime[Periph].mTIMx);		///< 使能定时器
    }
    else
    {
        timer_disable(sDevTime[Periph].mTIMx);		///< 关闭定时器
    }
}

/**
 * @brief	定时器中断使能
 *
 * @param   IsrNum  中断通道号 IRQn_Type
 * @param	IrqType 中断类型 TIMER_INT_UP/...
 * @return	
 */
void DrvTimerInterruptSwitch(TIMER_PERIPH Periph, uint32_t IsrNum, uint32_t IrqType)
{
    //nvic_irq_enable(sDevTime[Periph].mIsrNum, 1, 0);
    nvic_irq_enable(IsrNum, 1, 0);
    timer_interrupt_flag_clear(sDevTime[Periph].mTIMx, IrqType);
    timer_interrupt_enable(sDevTime[Periph].mTIMx,IrqType);
}

/**
 * @brief	清除中断标志
 *
 * @param	IrqType 中断类型 TIMER_INT_UP/...
 * @return	
 */
static __INLINE void DrvClearInterruptFlag(TIMER_PERIPH Periph)
{
    uint8_t i = 0;

    for(i=0; i<8; i++)
    {
        if(timer_interrupt_flag_get(sDevTime[Periph].mTIMx, BIT(i)))
        {
            timer_interrupt_flag_clear(sDevTime[Periph].mTIMx, BIT(i));
        }
    }
    
}

/**
 * @brief	设置中断回调
 *
 * @param	IrqType 
 * @return	
 */
void DrvSetInterruptCallBack(TIMER_PERIPH Periph, TimeCallBack CallBack)
{
    sDevTime[Periph].mTimeCallBack = CallBack;
}

/**
 * @brief	定时器0死区中断
 *
 * @param	IrqType 
 * @return	
 */
void TIMER0_BRK_IRQHandler(void)
{
    
    DrvClearInterruptFlag(TIMER_0);
    if(sDevTime[TIMER_0].mTimeCallBack != NULL)
    {
        sDevTime[TIMER_0].mTimeCallBack();
    }
    
}    
    
/**
 * @brief	定时器0更新中断
 *
 * @param	IrqType 
 * @return	
 */                
void TIMER0_UP_IRQHandler(void)
{
    
    DrvClearInterruptFlag(TIMER_0);
    if(sDevTime[TIMER_0].mTimeCallBack != NULL)
    {
        sDevTime[TIMER_0].mTimeCallBack();
    }
    
}  

/**
 * @brief	定时器0比较中断
 *
 * @param	IrqType 
 * @return	
 */
void TIMER0_TRG_CMT_IRQHandler(void)
{
    
    DrvClearInterruptFlag(TIMER_0);
    if(sDevTime[TIMER_0].mTimeCallBack != NULL)
    {
        sDevTime[TIMER_0].mTimeCallBack();
    }
    
}  

/**
 * @brief	定时器0通道中断
 *
 * @param	IrqType 
 * @return	
 */
void TIMER0_Channel_IRQHandler(void)
{
    
    DrvClearInterruptFlag(TIMER_0);
    if(sDevTime[TIMER_0].mTimeCallBack != NULL)
    {
        sDevTime[TIMER_0].mTimeCallBack();
    }
    
}  

/**
 * @brief	定时器1中断管理
 *
 * @param	IrqType 中断类型 TIMER_INT_UP/...
 * @return	
 */
void TIMER1_IRQHandler()
{
    
    DrvClearInterruptFlag(TIMER_1);
    if(sDevTime[TIMER_1].mTimeCallBack != NULL)
    {
        sDevTime[TIMER_1].mTimeCallBack();
    }
    
}

/**
 * @brief	定时器2中断管理
 *
 * @param	IrqType 中断类型 TIMER_INT_UP/...
 * @return	
 */
void TIMER2_IRQHandler()
{
    
    DrvClearInterruptFlag(TIMER_2);
    if(sDevTime[TIMER_2].mTimeCallBack != NULL)
    {
        sDevTime[TIMER_2].mTimeCallBack();
    }
    
}

/**
 * @brief	定时器3中断管理
 *
 * @param	IrqType 中断类型 TIMER_INT_UP/...
 * @return	
 */
void TIMER3_IRQHandler()
{
    
    DrvClearInterruptFlag(TIMER_3);
    if(sDevTime[TIMER_3].mTimeCallBack != NULL)
    {
        sDevTime[TIMER_3].mTimeCallBack();
    }
    
}

/**
 * @brief	定时器4中断管理
 *
 * @param	IrqType 中断类型 TIMER_INT_UP/...
 * @return	
 */
void TIMER4_IRQHandler()
{
    
    DrvClearInterruptFlag(TIMER_4);
    if(sDevTime[TIMER_4].mTimeCallBack != NULL)
    {
        sDevTime[TIMER_4].mTimeCallBack();
    }
    
}
