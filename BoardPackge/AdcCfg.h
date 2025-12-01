#ifndef ADCCFG_H
#define ADCCFG_H

#include "GpioMap.h"
#include "DrvAdc.h"
/**
 * @brief ADC GPIO配置索引
 *
 */
typedef  enum 
{
	GPIO_SAMPLE_12V		= 15,
	GPIO_SAMPLE_5V		= 16 ,
	GPIO_SAMPLE_HSD		= 17 ,
	GPIO_SAMPLE_NTC1	= 23,
	GPIO_SAMPLE_NTC2	= 24,
	GPIO_SAMPLE_BAT		= 35,
}ENU_SAMPLE_GPIO_INDEX;

/**
 * @brief AD采集配置信息
 *
 */
typedef struct  
{
	ENU_SAMPLE_GPIO_INDEX 	eSample_index; // AD管脚索引
	uint8_t				    ucADC_x;        // ADC模块
	uint8_t 		        ucChannel;      // AD采集通道
    uint16_t  	            usResisSum;     // 总电阻
	uint16_t  	            usResis;        // 分压电阻
}STR_ADC_CONF;


/**
 * @brief 使用AD采集的引脚通道枚举
 *
 */
typedef enum 
{
	SAMPLE_12V	= 0 ,
	SAMPLE_5V,	
	SAMPLE_HSD,	
	SAMPLE_NTC1,
	SAMPLE_NTC2,
	SAMPLE_BAT,		
    SAMPLE_MAX,
}ENU_SAMPLE_INDEX;

extern const STR_ADC_CONF gstAdcList[];
extern const STR_ADC_CONF *AdcConfPtr;

#endif




