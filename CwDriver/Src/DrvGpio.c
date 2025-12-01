/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DrvGpio.h
作者: JH
版本号:1.0
生成日期:2023.4.4
概述: Gpio驱动
修改日志：
*********************************************************************/
#include "DrvGpio.h"

/**
* @brief	GPI端口地址
*/
static const uint32_t sPorterMap[] =
{
    GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG
};

/**
* @brief	GPIO外部中断线定义
*/
static const exti_line_enum sPortExtiLine[] = 
{
	EXTI_0, EXTI_1, EXTI_2,  EXTI_3,  EXTI_4,  EXTI_5,  EXTI_6,  EXTI_7,
	EXTI_8, EXTI_9, EXTI_10, EXTI_11, EXTI_12, EXTI_13, EXTI_14, EXTI_15,
    EXTI_16, EXTI_17, EXTI_18, EXTI_19
};

static const IRQn_Type sIrqHandle[] =
{
    EXTI0_IRQn, EXTI1_IRQn, EXTI2_IRQn, EXTI3_IRQn, EXTI4_IRQn, 
    EXTI5_9_IRQn, EXTI5_9_IRQn, EXTI5_9_IRQn, EXTI5_9_IRQn, EXTI5_9_IRQn,
    EXTI10_15_IRQn, EXTI10_15_IRQn, EXTI10_15_IRQn, EXTI10_15_IRQn,EXTI10_15_IRQn,EXTI10_15_IRQn
};


/***************************************************************
 * @brief GPIO测试
 * @param 
 ***************************************************************/
void GpioTest(void)
{
    DrvGpioInit(GPIOB, GPIO_PIN_10, GPIO_MODE_OUT_PP);
    DrvGpioInit(GPIOB, GPIO_PIN_11, GPIO_MODE_OUT_PP);
    DrvGpioInit(GPIOA, GPIO_PIN_7, GPIO_MODE_OUT_PP);
    DrvGpioInit(GPIOB, GPIO_PIN_8, GPIO_MODE_OUT_PP);
    DrvGpioOperat(GPIOB, GPIO_PIN_8, 0);
}

/**
* @brief	GPIO中断回调函数类型
*/
static GpioIntCallBack sDrvGpioFunc[16] = {NULL_PTR};

/**
 * @brief swd初始化
 * @param 禁用JTAG和NJTRST 使用PB4 PA15
 */
void DrvSWDInit(void)
{
	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP,ENABLE);//PB4 PA15
}

/**
 * @brief GPIO初始化
 * @param GPIO_MODE_AIN                   
          GPIO_MODE_IN_FLOATING            
          GPIO_MODE_IPD                    
          GPIO_MODE_IPU                    
          GPIO_MODE_OUT_OD                 
          GPIO_MODE_OUT_PP                 
          GPIO_MODE_AF_OD                  
          GPIO_MODE_AF_PP
 */
void DrvGpioInit(uint32_t Port, uint32_t Pin, uint8_t GpioMode)
{
    gpio_init(Port, GpioMode, GPIO_OSPEED_50MHZ, Pin); 
}

/**
 * @brief GPIO操作
 * @param 
 */
void DrvGpioOperat(uint32_t Port, uint32_t Pin, BOOL Set)
{
    if(Set)
    {
        gpio_bit_set(Port, Pin);
    }
    else
    {
        gpio_bit_reset(Port, Pin);
    }
}

/**
 * @brief GPIO输入电平获取
 * @param 
 */
BOOL DrvGpioGetInputBit(uint32_t Port, uint32_t Pin)
{
   FlagStatus Bit;

    Bit = gpio_input_bit_get(Port, Pin);

    return Bit;
}

/**
 * @brief GPIO输出电平获取
 * @param 
 */
BOOL DrvGpioGetOutputBit(uint32_t Port, uint32_t Pin)
{
    FlagStatus Bit;

    Bit = gpio_output_bit_get(Port, Pin);

    return Bit;
}   

/**
 * @brief GPIO中断设置
 * @param 
 *  EXTI_TRIG_RISING = 0,                                    
    EXTI_TRIG_FALLING,                                       
    EXTI_TRIG_BOTH,                                          
    EXTI_TRIG_NONE  
 */
void DrvGpioExitSetSource(uint8_t SrcPort, uint8_t SrcPin, exti_trig_type_enum Type)
{
    gpio_init(sPorterMap[SrcPort], GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, BIT(SrcPin));
    gpio_exti_source_select(SrcPort,SrcPin);
    exti_init(sPortExtiLine[SrcPin], EXTI_INTERRUPT, Type);
    exti_interrupt_enable(sPortExtiLine[SrcPin]);
    exti_interrupt_flag_clear(sPortExtiLine[SrcPin]);
    nvic_irq_enable((uint8_t)sIrqHandle[SrcPin], 1U, 0U);
}

/**
 * @brief 中断使能设置
 * @param 
 *  
 */
void DrvGpioExitItSet(uint8_t SrcPin, BOOL Set)
{
    if(Set)
    {
        exti_interrupt_enable(sPortExtiLine[SrcPin]);
    }
    else
    {
        exti_interrupt_disable(sPortExtiLine[SrcPin]);
    }
}

/**
 * @brief 设置中断回调
 * @param 
 *  
 */
void DrvSetExitCallBack(uint8_t SrcPin, GpioIntCallBack CallBackFunc)
{
    if(SrcPin>=16)
    {
        return;
    }
    
    sDrvGpioFunc[SrcPin] = CallBackFunc;
}

/**
 * @brief 中断回调
 * @param 
 *  
 */
static __INLINE void GpioExtiIsr(uint32_t inx)
{		
	if(exti_interrupt_flag_get((exti_line_enum)(1<<inx)) != RESET)
	{
		if(sDrvGpioFunc[inx] != NULL_PTR) 
		{
			sDrvGpioFunc[inx]();
		}
		exti_interrupt_flag_clear((exti_line_enum)(1<<inx));
	}
}

/**
 * @brief Pin0回调
 * @param 
 *  
 */
void EXTI0_IRQHandler(void)
{
    GpioExtiIsr(0);
}

/**
 * @brief Pin1回调
 * @param 
 *  
 */
void EXTI1_IRQHandler(void)
{
    GpioExtiIsr(1);
}

/**
 * @brief Pin2回调
 * @param 
 *  
 */
void EXTI2_IRQHandler(void)
{
   GpioExtiIsr(2);
}

/**
 * @brief Pin3回调
 * @param 
 *  
 */
void EXTI3_IRQHandler(void)
{
    GpioExtiIsr(3);
}

/**
 * @brief Pin4回调
 * @param 
 *  
 */
void EXTI4_IRQHandler(void)
{
    GpioExtiIsr(4);
}

/**
 * @brief Pin5_9回调
 * @param 
 *  
 */
void EXTI5_9_IRQHandler(void)
{
    GpioExtiIsr(5);
    GpioExtiIsr(6);
    GpioExtiIsr(7);
    GpioExtiIsr(8);
    GpioExtiIsr(9);
}

/**
 * @brief Pin10_15回调
 * @param 
 *  
 */
void EXTI10_15_IRQHandler(void)
{
    GpioExtiIsr(10);
    GpioExtiIsr(11);
    GpioExtiIsr(12);
    GpioExtiIsr(13);
    GpioExtiIsr(14);
    GpioExtiIsr(15);
}

