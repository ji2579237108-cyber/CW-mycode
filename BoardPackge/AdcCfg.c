/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：AdcCfg.c
作者: LXD
版本号:1.0
生成日期:2023.5.4
概述:     ADC配置文件
修改日志：
*********************************************************************/
#include "AdcCfg.h"
/**
 * @brief ADC结构体链表
 *
 */
const STR_ADC_CONF gstAdcList[] =
{
    { GPIO_SAMPLE_12V,	ADC_0, ADC_CHANNEL_10, 715+110, 110 },
    { GPIO_SAMPLE_5V,	ADC_0, ADC_CHANNEL_11, 715+110, 110	},
    { GPIO_SAMPLE_HSD,	ADC_0, ADC_CHANNEL_12, 715+65,  65	},
    { GPIO_SAMPLE_NTC1, ADC_0, ADC_CHANNEL_0,  100	 ,  100 },
	{ GPIO_SAMPLE_NTC2, ADC_0, ADC_CHANNEL_1,  100	 ,  100 },
	{ GPIO_SAMPLE_BAT,	ADC_0, ADC_CHANNEL_8,  715+110, 110 },
};

const uint8_t gucAdcInitNbr = (sizeof(gstAdcList)/sizeof(gstAdcList[0]));  ///< 需要操作的ADC数量
const STR_ADC_CONF *AdcConfPtr = gstAdcList;  ///< ADC链表指针
