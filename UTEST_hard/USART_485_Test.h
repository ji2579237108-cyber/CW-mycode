/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：USART_485_Test.h
作者: LXD
版本号:1.0
生成日期:2023.4.22
概述:  针对GD32F30X的串口驱动测试：
			usart1作为串口输出
			usart2作为485_1通讯口
			usart3作为485_2通讯口
修改日志：
*********************************************************************/


#ifndef USART_485_TEST_H
#define USART_485_TEST_H

#include "CWDrv.h"
#include "DioTest.h"

void Usart1_init(void);
void Usart2_init(void);
void Uart3_init(void);



#define IN485_TX_EN 	DioWriteChannel(DIR_485_2,TRUE)
#define IN485_RX_EN 	DioWriteChannel(DIR_485_2,FALSE)
#define OT485_TX_EN 	DioWriteChannel(DIR_485_1,TRUE)
#define OT485_RX_EN 	DioWriteChannel(DIR_485_1,FALSE)


#define OUT_RS485_SEND(BUFFER, BUFFER_COUNT) do {\
    OT485_TX_EN;\
    vdelay_ms(5); \
    DrvUsartBlockSendData(UART_3,BUFFER, BUFFER_COUNT); \
    OT485_RX_EN; \
    vdelay_ms(5); } while(0);

#define IN_RS485_SEND(BUFFER, BUFFER_COUNT) do {\
    IN485_TX_EN;\
    vdelay_ms(5); \
    DrvUsartBlockSendData(USART_2,BUFFER, BUFFER_COUNT); \
    IN485_RX_EN; \
    vdelay_ms(5); } while(0);





void TEST_USART_trans(void);

#endif
