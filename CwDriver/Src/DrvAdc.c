/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DrvAdc.c
作者: JH
版本号:1.0
生成日期:2023.4.11
概述: ADC驱动
修改日志：
*********************************************************************/
#include "DrvAdc.h"

#define ADC_REF_VOLTAGE   3300
static uint32_t AdcValue[16] = {0};
static uint8_t  AdcIndex[16] = {0};

/**
 * @brief ADC资源配置
 */
const static ADC_RESOURCE sAdcResource[] =
{
#ifdef GD32F30X_HD
	{ADC0, RCU_CKADC_CKAPB2_DIV6, ADC_RESOLUTION_12B, DMA0, RCU_ADC0, RCU_DMA0, DMA_CH0},
    {ADC1, RCU_CKADC_CKAPB2_DIV6, ADC_RESOLUTION_12B, NULL, RCU_ADC1, RCU_DMA0, DMA_CH0},
    {ADC2, RCU_CKADC_CKAPB2_DIV6, ADC_RESOLUTION_12B, DMA1, RCU_ADC2, RCU_DMA1, DMA_CH4}
#endif
	
#ifdef GD32F30X_CL
	{ADC0, RCU_CKADC_CKAPB2_DIV6, ADC_RESOLUTION_12B, DMA0, RCU_ADC0, RCU_DMA0, DMA_CH0},
	{ADC1, RCU_CKADC_CKAPB2_DIV6, ADC_RESOLUTION_12B, NULL, RCU_ADC1, RCU_DMA0, DMA_CH0},
#endif
};

/***************************************************************
 * @brief ADC测试
 * @param 
 ***************************************************************/
void ADCTest(void)
{
    CHANNEL_CONFIG Config[3];
    Config[0].Index   = 0;
    Config[0].Channel = ADC_CHANNEL_0;
    Config[1].Index   = 1;
    Config[1].Channel = ADC_CHANNEL_5;
    Config[2].Index   = 2;
    Config[2].Channel = ADC_CHANNEL_6;

    DrvAdcGpioInit(GPIOA, GPIO_PIN_5);
    DrvAdcRcuConfig(ADC_0);
    //DrvADCNormalInit(ADC_0);
    DrvADCUseDmaInit(ADC_0, 3, Config);
   
    //DEBUG_LOG_INFO(DEBUG_LEVEL_2, "Ad Valude: %d.%d%d\n", ((uint32_t)ADValue*100)/100, ((uint32_t)ADValue*100)%100/10, ((uint32_t)ADValue*100)/100%10);
}

/**
 * @brief ADC gpio配置
 *
 * @param 
 */
void DrvAdcGpioInit(uint32_t GpioPort, uint32_t GpioPin)
{
    gpio_init(GpioPort,GPIO_MODE_AIN,GPIO_OSPEED_50MHZ,GpioPin);
}

/**
 * @brief ADC时钟初始化
 *
 * @param 
 */
void DrvAdcRcuConfig(ADC_PERIPH Periph)
{
    rcu_periph_clock_enable(sAdcResource[Periph].RCUx);
    
    rcu_adc_clock_config(sAdcResource[Periph].CLKx);
}

/**
 * @brief ADC常规初始化
 *
 * @param 
 */
void DrvADCNormalInit(ADC_PERIPH Periph)
{
    adc_deinit(sAdcResource[Periph].ADCx);

    adc_mode_config(ADC_MODE_FREE);

    adc_data_alignment_config(sAdcResource[Periph].ADCx, ADC_DATAALIGN_RIGHT);
   
    adc_channel_length_config(sAdcResource[Periph].ADCx, ADC_REGULAR_CHANNEL, 1U);
    
    adc_external_trigger_source_config(sAdcResource[Periph].ADCx, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE); 
    
    adc_external_trigger_config(sAdcResource[Periph].ADCx, ADC_REGULAR_CHANNEL, ENABLE);

    adc_resolution_config(sAdcResource[Periph].ADCx, sAdcResource[Periph].Resolution);

    adc_enable(sAdcResource[Periph].ADCx);

    adc_calibration_enable(sAdcResource[Periph].ADCx);
}

static void RegularChannelConfig(uint32_t ADCx, const CHANNEL_CONFIG* ConfigPtr, uint8_t ChNum)
{
    uint8_t i = 0;

    for(i=0; i<ChNum; i++)
    {
        adc_regular_channel_config(ADCx, ConfigPtr[i].Index, ConfigPtr[i].Channel, ADC_SAMPLETIME_239POINT5);
        AdcIndex[i] = ConfigPtr[i].Channel;
    }

}

/**
 * @brief ADC使用DMA初始化
 *
 * @param 
 */
S8 DrvADCUseDmaInit(ADC_PERIPH Periph, uint8_t DMAChNum, const CHANNEL_CONFIG* ConfigPtr)
{
    dma_parameter_struct dma_data_parameter; 

    if(Periph == ADC_1)                                 //ADC1没有DMA
    {
        return -1;
    }                           
    
    rcu_periph_clock_enable(sAdcResource[Periph].DMA_RCU);

    dma_deinit(sAdcResource[Periph].DMAx, sAdcResource[Periph].DMA_CHx);

    dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA(sAdcResource[Periph].ADCx));
    dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr  = (uint32_t)(AdcValue);
    dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_32BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_32BIT;  
    dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number       = DMAChNum;
    dma_data_parameter.priority     = DMA_PRIORITY_HIGH;
    dma_init(sAdcResource[Periph].DMAx, sAdcResource[Periph].DMA_CHx, &dma_data_parameter);

    dma_circulation_enable(sAdcResource[Periph].DMAx, sAdcResource[Periph].DMA_CHx);
  
    dma_channel_enable(sAdcResource[Periph].DMAx, sAdcResource[Periph].DMA_CHx);

    adc_mode_config(ADC_MODE_FREE); 

    adc_special_function_config(sAdcResource[Periph].ADCx, ADC_CONTINUOUS_MODE, ENABLE);
    
    adc_special_function_config(sAdcResource[Periph].ADCx, ADC_SCAN_MODE, ENABLE);
   
    adc_data_alignment_config(sAdcResource[Periph].ADCx, ADC_DATAALIGN_RIGHT);

    adc_channel_length_config(sAdcResource[Periph].ADCx, ADC_REGULAR_CHANNEL, DMAChNum);

    RegularChannelConfig(sAdcResource[Periph].ADCx, ConfigPtr, DMAChNum);
    
    adc_external_trigger_source_config(sAdcResource[Periph].ADCx, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
    adc_external_trigger_config(sAdcResource[Periph].ADCx, ADC_REGULAR_CHANNEL, ENABLE);

    adc_resolution_config(sAdcResource[Periph].ADCx, sAdcResource[Periph].Resolution);

    adc_dma_mode_enable(sAdcResource[Periph].ADCx);

    adc_enable(sAdcResource[Periph].ADCx);

    adc_calibration_enable(sAdcResource[Periph].ADCx);
    
	adc_software_trigger_enable(sAdcResource[Periph].ADCx, ADC_REGULAR_CHANNEL);

	return 0;
}

/**
 * @brief ADC单次采样转换
 *
 * @param 
 */
uint32_t DrvAdcChannelSample(ADC_PERIPH Periph, uint8_t Channel)
{
    uint32_t AdcValue = 0;

    adc_regular_channel_config(sAdcResource[Periph].ADCx, 0U, Channel, ADC_SAMPLETIME_239POINT5);
    
    adc_software_trigger_enable(sAdcResource[Periph].ADCx, ADC_REGULAR_CHANNEL);

    while(!adc_flag_get(sAdcResource[Periph].ADCx, ADC_FLAG_EOC));

    adc_flag_clear(sAdcResource[Periph].ADCx, ADC_FLAG_EOC);
    
    AdcValue = adc_regular_data_read(sAdcResource[Periph].ADCx);

    return (AdcValue*ADC_REF_VOLTAGE)/0xfff;;
}


/**
 * @brief 获取DMA通道AD转换值
 *
 * @param
 */
uint32_t DrvGetDmaChannelAdValue(uint8_t Channel)
{
    uint32_t AdValue = 0;
    uint8_t  i = 0;

    for(i=0; i<16; i++)
    {
        if(AdcIndex[i] == Channel)
        {
            AdValue = (AdcValue[i]*ADC_REF_VOLTAGE)/0xfff;
						
            break;
        }
    }
    return AdValue;
} 
 
