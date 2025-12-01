/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：USART_485_Test.c
作者: LXD
版本号:1.0
生成日期:2023.4.22
概述:  针对GD32F30X的串口驱动测试：
			usart1作为串口输出
			usart2作为485_1通讯口
			usart3作为485_2通讯口
修改日志：
*********************************************************************/
#include "USART_485_Test.h"


#ifndef ENABLE 
#define ENABLE 1

#endif

CHAR TEST_usart1[]	 ={0X32,0X33,0X35,0X54,0X46};
CHAR TEST_uart_rec2[]={0X62,0X63,0X65,0X64,0X47};
CHAR TEST_uart_rec3[]={0X42,0X43,0X45,0X64,0X58};


/**
 * @brief 串口1 中断回调函数
 *
 * @note 
 */
void Usart1_CallBack(uint8_t *data, uint8_t size)
{
	DrvUsartBlockSendData(USART_1,TEST_usart1,5 );
}


/**
 * @brief 串口2 中断回调函数
 *
 * @note 使用串口打印同时使用RTT打印数据，同时发送接收到的数据
 */
void Usart2_R485_1_CallBack(uint8_t *data, uint8_t size)
{
	printf("*485 1\n");
	DEBUG_LOG_INFO(DEBUG_LEVEL_2, "\n 485_1 usart2 IRQ : %X \n",*data);
	
//	OUT_RS485_SEND((CHAR*)data, 1);

}



/**
 * @brief 串口3 中断回调函数
 *
 * @note 使用串口打印同时使用RTT打印数据，同时发送接收到的数据
 */
void Usart3_R485_2_CallBack(uint8_t *data, uint8_t size)
{
	printf("*485 2\n");
	DEBUG_LOG_INFO(DEBUG_LEVEL_2, "\n 485_2 usart3 IRQ : %X \n",*data);
	
//	IN_RS485_SEND((CHAR*)data, 1);	
	
}







/**
 * @brief usart2初始化，外485_1
 *
 * @note 波特率：9600
 *
 */
void Usart2_init()
{	
	DRV_USART_CONFIG  usart_conf;
	USART_INT_MANAGE  usart_INT_conf;
	
	DrvUsartGpioConfig(USART_2);//GPIO初始化
	
	usart_conf.mBaudRate = 9600;
	usart_conf.mDataBit = USART_WL_8BIT;
	usart_conf.mParityMode = USART_PM_NONE;
	usart_conf.mStopBit = USART_STB_1BIT;
	DrvUsartInit(USART_2,&usart_conf);//USART init
	
	usart_INT_conf.Irq = USART_INT_RBNE;
	usart_INT_conf.IrqFlag = USART_INT_FLAG_PERR;//not need
	usart_INT_conf.IrqType = USART2_IRQn;
	usart_INT_conf.nvic_irq_pre = 3;
	usart_INT_conf.nvic_irq_sub = 3;
	
	DrvUsartInterruptConfig(USART_2, &usart_INT_conf, ENABLE);//中断配置
	
	DrvUsartInterruptCallBack(USART2_RBNE, Usart2_R485_1_CallBack);
}




/**
 * @brief usart3初始化，内485_2
 *
 * @note 波特率：9600
 *
 */
void Uart3_init()
{	
	DRV_USART_CONFIG  usart_conf;
	USART_INT_MANAGE  usart_INT_conf;
	
	DrvUsartGpioConfig(UART_3);//GPIO初始化
	
	usart_conf.mBaudRate = 9600;
	usart_conf.mDataBit = USART_WL_8BIT;
	usart_conf.mParityMode = USART_PM_NONE;
	usart_conf.mStopBit = USART_STB_1BIT;
	DrvUsartInit(UART_3,&usart_conf);//USART init
	
	usart_INT_conf.Irq = USART_INT_RBNE;
	usart_INT_conf.IrqFlag = USART_INT_FLAG_PERR;//not need
	usart_INT_conf.IrqType = UART3_IRQn;
	usart_INT_conf.nvic_irq_pre = 3;
	usart_INT_conf.nvic_irq_sub = 2;
	DrvUsartInterruptConfig(UART_3, &usart_INT_conf, ENABLE);//中断配置
	
	DrvUsartInterruptCallBack(UART3_RBNE, Usart3_R485_2_CallBack);
}




/**
 * @brief 内外485 发送测试
 *
 * @note 
 *
 */

void TEST_USART_trans()
{	
//	OUT_RS485_SEND(TEST_uart_rec2, sizeof(TEST_uart_rec2)/sizeof(TEST_uart_rec2[0]));
	
//	IN_RS485_SEND(TEST_uart_rec3, sizeof(TEST_uart_rec3)/sizeof(TEST_uart_rec3[0]));	
}




