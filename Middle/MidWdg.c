#include "MidWdg.h"
#include "MidDio.h"

/**
 * @brief 看门狗初始化
 *		
 *				
 */		
void HardDog_GPIOinit()
{
	DrvGpioInit(GPIOC,GPIO_PIN_3, GPIO_MODE_OUT_PP);
}

/**
 * @brief 喂狗
 *		
 *				
 */	
void Feed_HardDog()
{
	static uint8_t feed_state = 0;
	feed_state = ~feed_state;
	DioWriteChannel(WDI,feed_state);
}


