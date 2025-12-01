#include "MidAdc.h"

/**
 * @brief ADC DMA初始化
 *
 * @note GD32F30X系列AD引脚既可以使用ADC0 又可以用ADC1。
 *	    所以只初始化ADC0。如果使用其他芯片可能需要增加ADCx的选项
 *
 */
void AdcInit(void)
{
	uint8_t i = 0;
    CHANNEL_CONFIG Config[SAMPLE_MAX];
    
    for(i=0; i < SAMPLE_MAX; i++)
    {
        Config[i].Index   = i;
				Config[i].Channel = gstAdcList[i].ucChannel;     
				DrvAdcGpioInit((g_gpio_ptr + gstAdcList[i].eSample_index)->uwPort, (g_gpio_ptr + gstAdcList[i].eSample_index)->uwPin );       
    }
	
    DrvAdcRcuConfig(ADC_0);	
    DrvADCUseDmaInit(ADC_0, SAMPLE_MAX, Config);//DMA的初始化
}


/**
 * @brief ADC DMA采集
 *
 * @param Sample_IO     AD采集索引：从ENU_SAMPLE_INDEX选择
 *
 */
uint32_t AdcDmaValue(ENU_SAMPLE_INDEX Sample_IO)
{
	 uint32_t temp = DrvGetDmaChannelAdValue(gstAdcList[Sample_IO].ucChannel);
//	if(Sample_IO == SAMPLE_HSD || Sample_IO == SAMPLE_5V || Sample_IO == SAMPLE_HSD)
//	{
//		SEGGER_RTT_printf(0, "temp = %d\r\n",temp);
//	}
	 uint32_t ADC_Res = ((temp*(gstAdcList[Sample_IO].usResisSum)/gstAdcList[Sample_IO].usResis));
	
	 return ADC_Res;
}
