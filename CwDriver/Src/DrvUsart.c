/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DrvUsart.h
作者: JH
版本号:1.0
生成日期:2023.4.13
概述: Usart驱动
修改日志：
*********************************************************************/
#include "DrvUsart.h"
#include <stdio.h>
#include <string.h>

#define SEND_SIZE 256

static UsartIntCallBack sUsartIntCallBack[USART_INT_SET_NUM] = {NULL};

/**
 * @brief 串口资源配置
 * GPIO_USART0_REMAP
 */
const static USART_RESOURCES sUsartResource[] =
{
#ifdef GD32F30X_CL 
	{USART0, GPIOA, GPIO_PIN_9, GPIOA, GPIO_PIN_10, NULL, RCU_USART0},
    {USART1, GPIOA, GPIO_PIN_2, GPIOA, GPIO_PIN_3, NULL, RCU_USART1},
    {USART2, GPIOB, GPIO_PIN_10, GPIOB, GPIO_PIN_11, NULL, RCU_USART2},
    {UART3,  GPIOC, GPIO_PIN_10, GPIOC, GPIO_PIN_11, NULL, RCU_UART3}
#endif
};

/**
 * @brief dma资源配置
 * 
 */
static USART_DMA_RESOURCES sDmaRxResources[] = 
{
    {DMA0, DMA_CH4, DMA0_Channel4_IRQn, NULL, (uint32_t)&USART_DATA(USART0), NULL, NULL,NULL},
    {DMA0, DMA_CH5, DMA0_Channel5_IRQn, NULL, (uint32_t)&USART_DATA(USART1), NULL, NULL,NULL},
};
static USART_DMA_RESOURCES sDmaTxResources[] = 
{
    {DMA0, DMA_CH3, DMA0_Channel3_IRQn, NULL, (uint32_t)&USART_DATA(USART0), NULL, NULL,NULL},
    {DMA0, DMA_CH6, DMA0_Channel6_IRQn, NULL, (uint32_t)&USART_DATA(USART1), NULL, NULL,NULL},
};

#if 1
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART1, (uint8_t)ch);
    while(RESET == usart_flag_get(USART1, USART_FLAG_TBE));
    return ch;
}
#endif

/**
 * @brief IO配置
 *
 * @param
 */
void DrvUsartGpioConfig(USART_PERIPH Periph)
{
    gpio_init(sUsartResource[Periph].mSendPort, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, sUsartResource[Periph].mSendPin);
    gpio_init(sUsartResource[Periph].mRecPort, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, sUsartResource[Periph].mRecPin);
	
    if(sUsartResource[Periph].mRemap != NULL)
    {
        gpio_pin_remap_config(sUsartResource[Periph].mRemap,ENABLE);
    }
}

/**
 * @brief 串口初始化
 *
 * @param
 */
void DrvUsartInit(USART_PERIPH Periph, DRV_USART_CONFIG* Config)
{
    rcu_periph_clock_enable(sUsartResource[Periph].mUsartRcu);

    usart_deinit(sUsartResource[Periph].mPeriph);
    usart_baudrate_set(sUsartResource[Periph].mPeriph, Config->mBaudRate);
    usart_receive_config(sUsartResource[Periph].mPeriph, USART_RECEIVE_ENABLE);
    usart_transmit_config(sUsartResource[Periph].mPeriph, USART_TRANSMIT_ENABLE);
    usart_parity_config(sUsartResource[Periph].mPeriph, Config->mParityMode);
    usart_word_length_set(sUsartResource[Periph].mPeriph, Config->mDataBit);
    usart_stop_bit_set(sUsartResource[Periph].mPeriph, Config->mStopBit);
    
    usart_enable(sUsartResource[Periph].mPeriph);
}

/**
 * @brief 串口中断配置
 *
 * @param
 */
void DrvUsartInterruptConfig(USART_PERIPH Periph, USART_INT_MANAGE* IrqManage, BOOL Enable)
{
    nvic_irq_enable(IrqManage->IrqType, IrqManage->nvic_irq_pre, IrqManage->nvic_irq_sub);
    //usart_flag_clear(sUsartResource[Periph].mPeriph,USART_FLAG_TBE);
    if(Enable)
    {
        usart_interrupt_enable(sUsartResource[Periph].mPeriph, IrqManage->Irq);
    }
    else
    {
        usart_interrupt_disable(sUsartResource[Periph].mPeriph, IrqManage->Irq);
    }
}

/**
 * @brief 串口中断回调
 *
 * @param
 */
void DrvUsartInterruptCallBack(USART_INT_SET IrqSet, UsartIntCallBack CallBackPtr)
{
    sUsartIntCallBack[IrqSet] = CallBackPtr;
}

/**
 * @brief 串口接收DMA配置
 *
 * @param
 */
void DrvUsartRxDmaConfig(USART_PERIPH Periph,const uint8_t* RxDataBuf,const uint8_t RxNum)
{
    dma_parameter_struct dma_init_struct;
    
    dma_deinit(sDmaRxResources[Periph].mPeriph, sDmaRxResources[Periph].mChannel);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr  = (uint32_t)RxDataBuf;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number       = RxNum;
    dma_init_struct.periph_addr  = sDmaRxResources[Periph].mUsartAddr;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(sDmaRxResources[Periph].mPeriph, sDmaRxResources[Periph].mChannel, &dma_init_struct);
   
    dma_circulation_enable(sDmaRxResources[Periph].mPeriph, sDmaRxResources[Periph].mChannel);   
    dma_channel_enable(sDmaRxResources[Periph].mPeriph, sDmaRxResources[Periph].mChannel);

    usart_dma_receive_config(sUsartResource[Periph].mPeriph, USART_RECEIVE_DMA_ENABLE);

    sDmaRxResources[Periph].mDmaMemory  = (uint32_t)RxDataBuf;
    sDmaRxResources[Periph].mMemorySize = RxNum;
}

/**
 * @brief 串口发送DMA配置
 *
 * @param
 */
void DrvUsartTxDmaConfig(USART_PERIPH Periph)
{
    dma_parameter_struct dma_init_struct;
    
    dma_deinit(sDmaTxResources[Periph].mPeriph, sDmaTxResources[Periph].mChannel);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr  = NULL;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number       = 0;
    dma_init_struct.periph_addr  = sDmaTxResources[Periph].mUsartAddr;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(sDmaTxResources[Periph].mPeriph, sDmaTxResources[Periph].mChannel, &dma_init_struct);
   
    dma_circulation_disable(sDmaTxResources[Periph].mPeriph, sDmaTxResources[Periph].mChannel);   
    dma_channel_enable(sDmaTxResources[Periph].mPeriph, sDmaTxResources[Periph].mChannel);

    usart_dma_transmit_config(sUsartResource[Periph].mPeriph, USART_TRANSMIT_DMA_ENABLE);  
}

/**
 * @brief 串口阻塞发送数据
 *
 * @param
 */
S8 DrvUsartBlockSendData(USART_PERIPH Periph, CHAR *const SendBuf, uint8_t TxSize)
{
    uint8_t TxData[SEND_SIZE] = {0};
    uint8_t TxCount = 0;

    if(TxSize == 0 || SendBuf == NULL)
    {
        return -1;
    }

    memcpy((void *)TxData,(void *)SendBuf, TxSize);
      
    for(TxCount=0; TxCount<TxSize; TxCount++)
    {
        while(RESET == usart_flag_get(sUsartResource[Periph].mPeriph, USART_FLAG_TBE));
        usart_data_transmit(sUsartResource[Periph].mPeriph, TxData[TxCount]);
    }
    
    return 0;
}

/**
 * @brief 串口DMA发送数据
 *
 * @param
 */
S8 DrvUsartDmaSendData(USART_PERIPH Periph, uint8_t *const SendBuf, uint8_t TxSize)
{
    uint8_t TxBuf[SEND_SIZE] = {0};

    if(TxSize == 0 || SendBuf == NULL)
    {
        return -1;
    }

    dma_flag_clear(sDmaTxResources[Periph].mPeriph, sDmaTxResources[Periph].mChannel, DMA_FLAG_FTF);

    memcpy(TxBuf, SendBuf, TxSize);

	dma_channel_disable(sDmaTxResources[Periph].mPeriph, sDmaTxResources[Periph].mChannel);

	dma_memory_address_config(sDmaTxResources[Periph].mPeriph, sDmaTxResources[Periph].mChannel,(uint32_t)TxBuf);     //设置要发送数据的内存地址
	dma_transfer_number_config(sDmaTxResources[Periph].mPeriph, sDmaTxResources[Periph].mChannel,TxSize);   //一共发多少个数据
	
	dma_channel_enable(sDmaTxResources[Periph].mPeriph, sDmaTxResources[Periph].mChannel);
	    
	while(RESET == dma_flag_get(sDmaTxResources[Periph].mPeriph, sDmaTxResources[Periph].mChannel, DMA_FLAG_FTF));

    return 0;
}

/**
 * @brief	串口阻塞接收
 *
 * @param	
 * @return	成功返回0,失败返回非0
 */
S8 DrvUsartBlockRecData(USART_PERIPH Periph,uint8_t *RecvBuf,uint8_t RxSize)
{
    uint32_t UartPeriph = sUsartResource[Periph].mPeriph;

    if((NULL == RecvBuf ) ||(0U == RxSize) )
    {
        return -1;
    }
	
	usart_flag_clear(UartPeriph, USART_FLAG_RBNE);				///< 清除接收非空标志，准备接收数据
	
	while(RxSize--)
	{
		while(!usart_flag_get(UartPeriph, USART_FLAG_RBNE));	///< 等待接收缓冲区非空		
		*RecvBuf++ = usart_data_receive(UartPeriph);
	}
  
	return 0;
}

/**
 * @brief 中断回调
 *
 * @param
 */
static void UsartIntHandler(USART_PERIPH Periph, USART_INT_SET IrqSet)
{	
    uint8_t DataBuf[SEND_SIZE] = {0};
	
    uint8_t DataSize = 0;

	if(usart_interrupt_flag_get(sUsartResource[Periph].mPeriph, USART_INT_FLAG_IDLE) != RESET)      ///< 处理空闲中断
	{
		usart_interrupt_flag_clear(sUsartResource[Periph].mPeriph,USART_INT_FLAG_IDLE);	

        if(sDmaRxResources[Periph].mDmaMemory != NULL)                              ///< Dma接收使能，进行Dma接收
        {
            usart_data_receive(sUsartResource[Periph].mPeriph);								
            dma_channel_disable(sDmaRxResources[Periph].mPeriph, sDmaRxResources[Periph].mChannel);						
            
            DataSize = sDmaRxResources[Periph].mMemorySize - dma_transfer_number_get(sDmaRxResources[Periph].mPeriph,\
                        sDmaRxResources[Periph].mChannel);

            dma_memory_address_config(sDmaRxResources[Periph].mPeriph,sDmaRxResources[Periph].mChannel,\
                                     sDmaRxResources[Periph].mDmaMemory);
            dma_transfer_number_config(sDmaRxResources[Periph].mPeriph,sDmaRxResources[Periph].mChannel,\
                                      sDmaRxResources[Periph].mMemorySize);
            dma_channel_enable(sDmaRxResources[Periph].mPeriph, sDmaRxResources[Periph].mChannel);		
        }
    
		if(sUsartIntCallBack[IrqSet+3] != NULL )
		{
			sUsartIntCallBack[IrqSet+3](DataBuf, DataSize);
		}
	}	
	
	if(usart_interrupt_flag_get(sUsartResource[Periph].mPeriph, USART_INT_FLAG_TBE) != RESET)        ///< 处理传输缓冲区空中断
	{
        usart_interrupt_flag_clear(sUsartResource[Periph].mPeriph,USART_INT_FLAG_TBE);

		if(sUsartIntCallBack[IrqSet] != NULL )
		{
			sUsartIntCallBack[IrqSet](DataBuf, DataSize);
		}
	}
	
	if(usart_interrupt_flag_get(sUsartResource[Periph].mPeriph, USART_INT_FLAG_TC) != RESET)         ///< 处理传输完成中断
	{
        usart_interrupt_flag_clear(sUsartResource[Periph].mPeriph,USART_INT_FLAG_TC);

		usart_flag_clear(sUsartResource[Periph].mPeriph, USART_FLAG_TC);

        if(sUsartIntCallBack[IrqSet+1] != NULL )
		{
			sUsartIntCallBack[IrqSet+1](DataBuf, DataSize);
		}

	}
	
	if(usart_interrupt_flag_get(sUsartResource[Periph].mPeriph, USART_INT_FLAG_RBNE) != RESET)            ///< 处理接收缓冲区非空中断
	{
        usart_interrupt_flag_clear(sUsartResource[Periph].mPeriph,USART_INT_FLAG_RBNE);

		usart_flag_clear(sUsartResource[Periph].mPeriph,USART_FLAG_RBNE);
		
		DataBuf[0] = usart_data_receive(sUsartResource[Periph].mPeriph);//读取接收数据
		
        if(sUsartIntCallBack[IrqSet+2] != NULL )
		{
			sUsartIntCallBack[IrqSet+2](DataBuf, DataSize);
		}
	}
}


/**
 * @brief 串口0中断
 *
 * @param
 */
void USART0_IRQHandler(void)
{
    UsartIntHandler(USART_0, USART0_TBE);
}

/**
 * @brief 串口1中断
 *
 * @param
 */
void USART1_IRQHandler(void)
{	
    UsartIntHandler(USART_1, USART1_TBE);
}


void USART2_IRQHandler(void)
{	
    UsartIntHandler(USART_2, USART2_TBE);
}


void UART3_IRQHandler(void)
{
	uint8_t DataBuf = 0;
	if(usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE) != RESET)            ///< 处理接收缓冲区非空中断
	{
        usart_interrupt_flag_clear(UART3,USART_INT_FLAG_RBNE);

		usart_flag_clear(UART3,USART_FLAG_RBNE);
		DataBuf = usart_data_receive(UART3);
        
		sUsartIntCallBack[UART3_TBE+2](&DataBuf, 1);
		
	}
}




/**
 * @brief 串口0DMA通道3中断
 *
 * @param
 */
void DMA0_Channel3_IRQHandler(void)
{
    
}

/**
 * @brief 串口0DMA通道4中断
 *
 * @param
 */
void DMA0_Channel4_IRQHandler(void)
{
    
}

/**
 * @brief 串口1DMA通道5中断
 *
 * @param
 */
void DMA0_Channel5_IRQHandler(void)
{
    
}

/**
 * @brief 串口1DMA通道6中断
 *
 * @param
 */
void DMA0_Channel6_IRQHandler(void)
{
    
}


