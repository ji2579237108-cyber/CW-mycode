/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DrvUsart.h
作者: JH
版本号:1.0
生成日期:2023.4.13
概述: Usart驱动
修改日志：
*********************************************************************/
#ifndef _DRVUSART_H
#define _DRVUSART_H
#include "DataTypeDef.h"
#include "gd32f30x.h"

#if defined(__CC_ARM) 
#pragma diag_suppress 1296
#elif defined(__ICCARM__)
#elif defined(__GNUC__)
#endif


typedef void (*UsartIntCallBack)(uint8_t* Data, uint8_t Size);

typedef enum
{
    USART_0 = 0,
    USART_1,
    USART_2,
	UART_3,
    USART_PERIPH_NUM,
}USART_PERIPH;

/**
* @brief 串口DMA配置
*/
typedef struct
{
    uint32_t					mPeriph;				///< DMA基地址
	dma_channel_enum	mChannel;			    ///< DMA通道编号
	IRQn_Type		  	mIrq;       			///< 中断编号
	uint32_t					mIrqFlag;				///< 中断标志位
    uint32_t                 mUsartAddr;             ///< 外设地址
    uint32_t                 mDmaMemory;             ///< 内存地址
    uint8_t                  mMemorySize;            ///< 接收数据大小
	uint8_t					mDmaInx;				///< DMA编号
    
}USART_DMA_RESOURCES;

/**
* @brief 串口配置
*/
typedef struct
{
    uint32_t					mPeriph;				///< 设备地址
    uint32_t					mSendPort;				///< 发送端口
    uint32_t					mSendPin;				///< 发送引脚
    uint32_t					mRecPort;		        ///< 接收端口
    uint32_t					mRecPin;				///< 接收引脚
    uint32_t                 mRemap;                 ///< 重映射
    rcu_periph_enum     mUsartRcu;              ///< 串口时钟
	
}USART_RESOURCES;

/**
 * @brief 配置结构体
 */
typedef struct
{
    uint32_t             mBaudRate;				///< 波特率
    uint8_t              mParityMode;			///< 校验方式
    uint8_t              mDataBit;				///< 数据位数
    uint8_t              mStopBit;				///< 停止位数
}DRV_USART_CONFIG;

/**
* @brief 中断配置
*/
typedef enum
{
	USART0_TBE = 0,
    USART0_TC,
    USART0_RBNE,
    USART0_IDLE,
    USART1_TBE,
    USART1_TC,
    USART1_RBNE,
    USART1_IDLE,
    USART2_TBE,
    USART2_TC,
    USART2_RBNE,
	USART2_IDLE,   
	UART3_TBE,
    UART3_TC,
    UART3_RBNE,
    UART3_IDLE,
    USART_INT_SET_NUM,
}USART_INT_SET;

/**
* @brief 中断配置
*/
typedef struct
{
	IRQn_Type                 IrqType; 
    usart_interrupt_enum      Irq;
    usart_interrupt_flag_enum IrqFlag;
    uint8_t                        nvic_irq_pre;
    uint8_t                        nvic_irq_sub;
}USART_INT_MANAGE;

extern void DrvUsartGpioConfig(USART_PERIPH Periph);
extern void DrvUsartInit(USART_PERIPH Periph, DRV_USART_CONFIG* Config);
extern void DrvUsartInterruptConfig(USART_PERIPH Periph, USART_INT_MANAGE* IrqManage, BOOL Enable);
extern void DrvUsartInterruptCallBack(USART_INT_SET IrqSet, UsartIntCallBack CallBackPtr);
extern void DrvUsartRxDmaConfig(USART_PERIPH Periph,const uint8_t* RxDataBuf,const uint8_t RxNum);
extern void DrvUsartTxDmaConfig(USART_PERIPH Periph);
extern S8 DrvUsartBlockSendData(USART_PERIPH Periph, CHAR *const SendBuf, uint8_t TxSize);
extern S8 DrvUsartDmaSendData(USART_PERIPH Periph, uint8_t *const SendBuf, uint8_t TxSize);
extern S8 DrvUsartBlockRecData(USART_PERIPH Periph,uint8_t *RecvBuf,uint8_t RxSize);

#endif
