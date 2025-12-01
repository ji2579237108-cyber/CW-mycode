/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DrvCAN.h
作者: JH
版本号:1.0
生成日期:2023.4.4
概述: CAN驱动
修改日志：
*********************************************************************/

#ifndef _DRVCAN_H
#define _DRVCAN_H
#include "DataTypeDef.h"
#include "gd32f30x_libopt.h"
#include "gd32f30x_rcu.h"

#define CAN_EFID  4
#define CAN_SFID  0

typedef enum
{
    CAN_0 = 0,
    CAN_1,
    CAN_PERIPH_NUM,
}CAN_PERIPH;

typedef struct 
{
    uint32_t mCANx;
    uint32_t mRxPort;
    uint32_t mRxPin;
    uint32_t mTxPort;
    uint32_t mTxPin;
    uint32_t mRemap;
    uint8_t  mCanMode; 
    rcu_periph_enum mCAN_RCU;
	rcu_periph_enum mGPIO_RCU;
}CAN_RESOURCE;

/**
 * @brief 错误分类
 */
typedef enum
{
    DRV_CAN_ERR_BIT         = BIT(0),			///< 位错误
    DRV_CAN_ERR_STUFF       = BIT(1),			///< 填充错误
    DRV_CAN_ERR_CRC         = BIT(2),			///< CRC错误
    DRV_CAN_ERR_FORM        = BIT(3),			///< 格式错误
    DRV_CAN_ERR_ACK         = BIT(4),			///< Ack错误
}DRV_CAN_ERR_TYPE;

extern void (*CAN0TxInterruptCallBack)(void);
extern void (*CAN0RxInTerruptCallBack)(uint8_t* DataPtr, uint8_t DataLen);

extern void (*CAN1TxInterruptCallBack)(void);
extern void (*CAN1RxInTerruptCallBack)(uint8_t* DataPtr, uint8_t DataLen);

extern S32 DrvCanDeviceInit(CAN_PERIPH CanPeriph, uint32_t BaudRate);
extern S32 DrvCanFilterConfig(CAN_PERIPH CanPeriph, uint32_t Mask, uint32_t CanId, uint8_t FilterNum);
extern void DrvCanInterruptSet(CAN_PERIPH CanPeriph, uint32_t ItType, BOOL Enable);
extern uint8_t  DrvCanSendData(CAN_PERIPH CanPeriph,uint32_t FrameFormat, uint32_t SendID, uint8_t *const SendData, U16 DataLength);
extern uint32_t DrvGetCanError(CAN_PERIPH CanPeriph);

#endif 
