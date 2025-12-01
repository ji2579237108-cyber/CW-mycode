#include "MidUsart.h"
#include "BM22S2301.h"


uint8_t 			g_recvDataTtlLen	= 0;

/**
 * @brief 串口1 中断回调函数
 *
 * @note 
 */
void USART2_CallBack(uint8_t *data, uint8_t size)
{
	uint8_t recieve_data = *data;
	if (!Get_Rx_Flag())//接收数据
	{
//		SEGGER_RTT_printf(0," %x",recieve_data);
		/*d第一字节不为AA则取消*/
		// 如果长度为0，检查起始字节
		if (g_recvDataTtlLen == 0 && recieve_data != 0xAA) {
			// 不是起始字节，忽略，不存入缓冲区
			return;
		}
		g_Rx_BUFFER[g_recvDataTtlLen++] = recieve_data;
		
		if (g_recvDataTtlLen >= BM22S2301_LENGTH) {
			Set_Rx_Flag(1);
			g_usReceiveLength = g_recvDataTtlLen;
			g_recvDataTtlLen = 0;
		}
	}
}
/**
 * @brief usart1初始化
 *
 */
void Usart1_init()
{	
	DRV_USART_CONFIG  UsartConf;
	USART_INT_MANAGE  UsartIntConf;
	
	DrvUsartGpioConfig(USART_2);//GPIO初始化
	
	UsartConf.mBaudRate = 9600;
	UsartConf.mDataBit = USART_WL_8BIT;
	UsartConf.mParityMode = USART_PM_NONE;
	UsartConf.mStopBit = USART_STB_1BIT;
	DrvUsartInit(USART_2,&UsartConf);//USART init
	
	UsartIntConf.Irq = USART_INT_RBNE;
	UsartIntConf.IrqFlag = USART_INT_FLAG_PERR;//not need
	UsartIntConf.IrqType = USART2_IRQn;
	UsartIntConf.nvic_irq_pre = 3;
	UsartIntConf.nvic_irq_sub = 2;
	DrvUsartInterruptConfig(USART_2, &UsartIntConf, ENABLE);//中断配置
	
	DrvUsartInterruptCallBack(USART2_RBNE, USART2_CallBack);
}

void smoke_USART2_send(CHAR *const SendBuf, uint8_t TxSize)
{
	DrvUsartBlockSendData(USART_2,SendBuf, TxSize);
}
