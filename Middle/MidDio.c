#include "MidDio.h"


#if defined(USE_GD32F307_ZX)
#define  GD32GpioList  GD32ZxGpioList
#elif defined(USE_GD32F307_VX)
#define  GD32GpioList  GD32VxGpioList
#elif defined(USE_GD32F307_RX)
#define  GD32GpioList  GD32RxGpioList
#endif

/**
 * @brief GPIO初始化
 *
 * @param 
 * @note  引脚的时钟使能、重映射、引脚初始化
 */
void DioInit()
{
	uint8_t i ;
	
//初始化引脚时钟
	rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_AF);

//禁用JTAG和NJTRST 使用PB4 PA15
	DrvSWDInit();
	
//初始化引脚
    for(i=0; i < gucGpioInitNbr; i++)
    {
        DrvGpioInit((g_gpio_ptr + gstGpioList[i].index)->uwPort, \
        (g_gpio_ptr + gstGpioList[i].index)->uwPin, gstGpioList[i].usMode);
    }
	/*有电就亮的LED  & eeprom*/
	DioWriteChannel(LED_RUN,  	0);
	DioWriteChannel(EEPROM_WC1,	1);
	DioWriteChannel(BEEP_ALARM, 0);
}

/**
 * @brief GPIO引脚输出高低电平
 *
 * @param index ：操作的gpio引脚索引，详细引脚见 GpioMap.c文件
 * @param Set  ：输出电平
 */
void DioWriteChannel(ENU_GPIO_INDEX index,  uint8_t Set)
{
	DrvGpioOperat(GD32GpioList[index].uwPort, GD32GpioList[index].uwPin, Set);
}

/**
 * @brief 获取GPIO引脚输入电平
 *
 * @param index ：操作的gpio引脚索引，详细引脚见 GpioMap.c文件
 */

uint8_t DioReadChannel (ENU_GPIO_INDEX index)
{
	return DrvGpioGetInputBit(GD32GpioList[index].uwPort, GD32GpioList[index].uwPin);
}

void BEEP_ALARM_OFF(void)
{
    DioWriteChannel(BEEP_ALARM, 0);
}
void BEEP_ALARM_ON(void)
{
    DioWriteChannel(BEEP_ALARM, 1);
}
