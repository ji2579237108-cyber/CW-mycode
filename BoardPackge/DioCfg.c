/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DioCfg.c
作者: LYM
版本号:1.0
生成日期:2023.4.27
概述:  Dio配置文件
修改日志：
*********************************************************************/
#include "DioCfg.h"

/**
 * @brief Dio结构体链表
 *
 */
const STR_GPIO_INIT gstGpioList[] =
{
	/*输出*/
	{ HSD_IN,     	GPIO_MODE_OUT_PP},  
//	{ HSD_BK,     	GPIO_MODE_OUT_PP}, 
	{ CAN_STB,    	GPIO_MODE_OUT_PP},
	{ EEPROM_WC1, 	GPIO_MODE_OUT_PP},
	{ WDI  ,  	   	GPIO_MODE_OUT_PP},
	{ BEEP_ALARM,   GPIO_MODE_OUT_PP},
	{ BUCK_EN ,  	GPIO_MODE_OUT_PP},
	{ BOOST_EN ,  	GPIO_MODE_OUT_PP},
	{ LED_RUN,		GPIO_MODE_OUT_PP},
	{ LED_GREEN,	GPIO_MODE_OUT_PP},
	{ LED_ALARM,	GPIO_MODE_OUT_PP},
	{ LED_OPEN	,   GPIO_MODE_OUT_PP},
	{ LED_YELLOW,	GPIO_MODE_OUT_PP},
	/*输入*/
	{ SMOKE_STATUS, GPIO_MODE_IPD},
	{ DI_ALARM,   	GPIO_MODE_IPD},
	{ DI_MUTE,   	GPIO_MODE_IPD},
	{ DI_POWER,   	GPIO_MODE_IPD},
	{ DI_PRESS,   	GPIO_MODE_IPD},
	{ DI_START,   	GPIO_MODE_IPD},
	{ DI_STOP,   	GPIO_MODE_IPD},

};  	

const uint8_t gucGpioInitNbr = (sizeof(gstGpioList)/sizeof(gstGpioList[0])); ///<计算需要初始化的DO个数
