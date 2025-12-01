#ifndef DIOCFG_H
#define DIOCFG_H

#include "DrvGpio.h"



/**
 * @brief DO配置索引
 *
 */
typedef enum
{
	HSD_IN     	= 57, //电磁阀输出控制
//	HSD_BK     	= 58, //电磁阀反馈
	CAN_STB    	= 67,
	EEPROM_WC1 	= 97,//eeprom使能
	WDI    	   	= 18, //看门狗使能
	SMOKE_STATUS= 38, //烟雾状态位
	BEEP_ALARM  = 84, //蜂鸣器
	BUCK_EN   	= 2, //备点电池充电 降压EN
	BOOST_EN   	= 5, //备点电池充电 升压EN
	LED_RUN		= 33,
	LED_GREEN	= 56,
	LED_ALARM	= 87,
	LED_OPEN	= 86 ,
	LED_YELLOW	= 85,
	DO_ALARM	= 1,
    DO_FAULT 	= 98,       
    DO_SPARY 	= 96,
    DO_LEVEL2 	= 95,
    DO_BAT1 	= 3,
    DO_BAT2 	= 4,
	/*输入引脚索引*/
	DI_ALARM	= 66,
	DI_STOP		= 60,
	DI_START	= 61,
	DI_MUTE		= 62,
	DI_POWER	= 65,
	DI_PRESS	= 59,
} ENU_GPIO_INDEX;

/**
 * @brief 引脚初始化参数对应结构表
 *
 */
typedef struct  
{
    ENU_GPIO_INDEX   index;  ///< 引脚索引
	uint16_t         usMode; ///< 引脚配置模式
} STR_GPIO_INIT; 

extern const STR_GPIO_INIT gstGpioList[];
extern const uint8_t gucGpioInitNbr;

#endif
