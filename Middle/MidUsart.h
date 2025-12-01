#ifndef MIDUSART_H
#define MIDUSART_H

#include "CWDrv.h"
#include "DioTest.h"

void Usart1_init(void);
void Usart2_init(void);
void Uart3_init(void);
void smoke_USART2_send(CHAR *const SendBuf, uint8_t TxSize);



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

