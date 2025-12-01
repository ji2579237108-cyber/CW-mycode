/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DrvSpi.h
作者: LXD
版本号:1.0
生成日期:2023.4.20
概述: SPI驱动
修改日志：
*********************************************************************/
#ifndef CWSPI_H
#define CWSPI_H

#include "DataTypeDef.h"
#include "gd32f30x_libopt.h"
#include "CWDrv.h"



#define STD_DISABLE	0


//////////////////////////////////

//#define _SPI_CS_BY_GPIO_DEF				/*定义该宏则SPI的CS片选使用GPIO控制*/
//#define _SPI_DMA_TRANSFER_FIFO_DEF	    /*定义该宏则启用DMA传输FIFO*/

/**
* @brief  SPI速率
*/
typedef enum
{
	SPI_BANDRATE_42M    = 0,
	SPI_BANDRATE_21M    = 1,
	SPI_BANDRATE_10_5M  = 2,
	SPI_BANDRATE_5_25M  = 3,
	SPI_BANDRATE_2_625M = 4,
	SPI_BANDRATE_1_3125M,
	SPI_BANDRATE_0_65625M,
}BANDRATE;

/**
 * @brief 中断分类
 */
typedef enum
{
	DRV_SPI_INT_TX				= 1,		///< 发送中断		= BIT0
	DRV_SPI_INT_RX				= 2,		///< 接收中断		= BIT1
	DRV_SPI_INT_TRANS_COMPLETE	= 4,		///< 传输完成中断	= BIT2
	DRV_SPI_INT_RX_OVERFLOW		= 8,		///< 接收溢出		= BIT3
}DRV_SPI_INT_TYPE;

/**
* @brief SPI ID
*/
typedef enum
{
    DRV_SPI_ID_0 = 0,
    DRV_SPI_ID_1 = 1,
    DRV_SPI_ID_2 = 2,
    DRV_SPI_ID_3 = 3,
    DRV_SPI_ID_4 = 4,
    DRV_SPI_ID_5 = 5,
    DRV_SPI_ID_6 = 6,
    DRV_SPI_ID_7 = 7,
    DRV_SPI_ID_8 = 8,
    DRV_SPI_ID_9 = 9,
}DRV_SPI_ID;



typedef enum 
{
	mHalf_duplexMode = 0,
	mFull_duplexMode = -1,
	m4WireMode   = -2
}SPI_WorkMode;



/**
* @brief SPI CS
*/
typedef enum
{
    DRV_SPI_CS_0 = 0,
    DRV_SPI_CS_1 = 1,
    DRV_SPI_CS_2 = 2,
    DRV_SPI_CS_3 = 3,
    DRV_SPI_CS_4 = 4,
	DRV_SPI_CS_5 = 5,
    DRV_SPI_CS_6 = 6,
    DRV_SPI_CS_7 = 7,
}DRV_SPI_CS;

/**
* @brief SPI HOLD
*/
typedef enum
{
    DRV_SPI_HOLD_NONE   = 0,		///< 空闲时 SPI CS的状态
    DRV_SPI_HOLD_ACTIVE = 1,		///< 发送前拉低CS
    DRV_SPI_HOLD_CLR    = 2,		///< 发送后拉高CS
	DRV_SPI_HOLD_ACTIVE_AND_CLR    = 3,		///< 发送前拉低CS 并且 发送后拉高CS
}DRV_SPI_HOLD;



/**
* @brief	SPI设备初始化
*
* @param	SpiId			SPI编号
* @note		
* @return	0成功，其他失败
*/
extern S32 DrvSpiDeviceInit(DRV_SPI_ID SpiId );



//SPI发送单个byte
extern uint8_t DrvSpiSendByte(DRV_SPI_ID SpiId, uint8_t byte);

/**
* @brief    SPI阻塞传输
*
* @param    SpiId         SPI编号
* @param    SpiCs         SPI片选选择
* @param    CsHoldEn      SPI片选控制使能
* @param    SendBuf       发送缓冲区，只接收时可为NULL
* @param    RecvBuf       接收缓冲区，只发送时可为NULL
* @param    Length        传输长度
* @return	成功返回0，失败返回非0
 */
extern S32 DrvSpiBlockTransfer(const DRV_SPI_ID SpiId, const DRV_SPI_CS SpiCs, const DRV_SPI_HOLD CsHoldEn, uint8_t *const SendBuf, uint8_t *const RecvBuf, uint32_t Length);

/**
* @brief	片选操作
*
* @param	SpiId		SPI编号
* @param	SpiCs		选择从设备 ，有多个从设备时 ，通过该参数选择
* @param	CsHoldEn	0:拉低，非0:拉高
* @return	0成功，其他失败
*/

extern void DrvSpiCsSet(DRV_SPI_ID SpiId, const DRV_SPI_CS SpiCs, uint8_t set);









#endif
