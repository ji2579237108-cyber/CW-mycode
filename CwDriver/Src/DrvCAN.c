/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DrvCAN.c
作者: JH
版本号:1.0
生成日期:2023.4.4
概述:  单片机CAN驱动
修改日志：
*********************************************************************/
#include <string.h>
#include <stdio.h>
#include "DrvCAN.h"

FlagStatus ReceiveFlag;
can_receive_message_struct  ReceiveMessageCan0;
can_receive_message_struct  ReceiveMessageCan1;
can_trasnmit_message_struct TransmitMessage; 


void (*CAN0TxInterruptCallBack)(void) = NULL_PTR;
void (*CAN0RxInTerruptCallBack)(uint8_t* DataPtr, uint8_t DataLen) = NULL_PTR;

void (*CAN1TxInterruptCallBack)(void) = NULL_PTR;
void (*CAN1RxInTerruptCallBack)(uint8_t* DataPtr, uint8_t DataLen) = NULL_PTR;


/**
 * @brief CAN资源配置
 */
const static CAN_RESOURCE sCanResource[] =
{
#ifdef GD32F30X_HD
	{CAN0, GPIOA, GPIO_PIN_11, GPIOA, GPIO_PIN_12, GPIO_CAN_PARTIAL_REMAP, CAN_NORMAL_MODE, RCU_CAN0},
#endif
#ifdef GD32F30X_CL
	{CAN0, GPIOA, GPIO_PIN_11, GPIOA, GPIO_PIN_12, NULL, CAN_NORMAL_MODE, RCU_CAN0, RCU_GPIOA},
	{CAN1, GPIOB, GPIO_PIN_12, GPIOB, GPIO_PIN_13, NULL, CAN_NORMAL_MODE, RCU_CAN1, RCU_GPIOB},
#endif
	
};


/**
 * @brief Can中断优先级配置
 *
 * @param 使用的CAN外设
 */
static void DrvCanNvicConfig(CAN_PERIPH CanPeriph)
{
#ifdef GD32F30X_HD
	if(CanPeriph == CAN_0)
	{
		nvic_irq_enable(CAN0_RX1_IRQn,1,0);
	}
#endif
	
#ifdef GD32F30X_CL 
	
	if(CanPeriph == CAN_0)
	{		
		nvic_irq_enable(CAN0_RX1_IRQn,2,0);
	}
	
	if(CanPeriph == CAN_1)
	{
		nvic_irq_enable(CAN1_RX1_IRQn,2,1);
	}

#endif
}

/**
 * @brief CAN GPIO配置
 *
 * @param CanPeriph     Can外设
 */
static void DrvCanGpioConfig(CAN_PERIPH CanPeriph)
{
	rcu_periph_clock_enable(sCanResource[CanPeriph].mGPIO_RCU);
    gpio_init(sCanResource[CanPeriph].mRxPort,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,sCanResource[CanPeriph].mRxPin); 
    gpio_init(sCanResource[CanPeriph].mTxPort,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,sCanResource[CanPeriph].mTxPin);
    //gpio_pin_remap_config(sCanResource[CanPeriph].mRemap,ENABLE);
}



/**
 * @brief Can初始化
 *
 * @param CanPeriph     Can外设
 * @param BaudRate      波特率,输入10，代表波特率10K
 *
 */
S32 DrvCanDeviceInit(CAN_PERIPH CanPeriph, uint32_t BaudRate)
{
	can_parameter_struct canParameter;

	if(((uint32_t)CanPeriph >= CAN_PERIPH_NUM)
	 || (BaudRate > 1000000))
	{
		return -1;
	}

	rcu_periph_clock_enable(sCanResource[CanPeriph].mCAN_RCU);

	DrvCanGpioConfig(CanPeriph);

	can_struct_para_init(CAN_INIT_STRUCT, &canParameter);
	can_deinit(sCanResource[CanPeriph].mCANx);

    canParameter.time_triggered 		= DISABLE;				//费时间触发通信
    canParameter.auto_bus_off_recovery 	= ENABLE;	//ENABLE;
    canParameter.auto_wake_up 			= DISABLE;	//ENABLE;	//睡眠模式通过软件唤醒 CAN->MCR的SLEEP位
    canParameter.auto_retrans 			= ENABLE;
    canParameter.rec_fifo_overwrite		= DISABLE;				//报文不锁定，新的覆盖旧的
	canParameter.trans_fifo_order 		= DISABLE;	//ENABLE;
	canParameter.working_mode 			= sCanResource[CanPeriph].mCanMode;

    canParameter.resync_jump_width 		= CAN_BT_SJW_1TQ;
    canParameter.time_segment_1 		= CAN_BT_BS1_13TQ;
	canParameter.time_segment_2 		= CAN_BT_BS2_2TQ;  // 根据波特率和SystemCoreClock（当前72M）等需求, 配置CAN通讯参数:segm1\segm2 ,
                                            //87.5%
	canParameter.prescaler				= (SystemCoreClock/2)/(BaudRate*((canParameter.resync_jump_width\
										  +canParameter.time_segment_1\
										  +canParameter.time_segment_2 + 3)*1000));
	
    can_init(sCanResource[CanPeriph].mCANx, &canParameter);

	DrvCanNvicConfig(CanPeriph);

	return 0;
}

/**
 * @brief CAN过滤器配置
 *
 * @param Mask掩码
 */
S32 DrvCanFilterConfig(CAN_PERIPH CanPeriph, uint32_t Mask, uint32_t CanId, uint8_t FilterNum)
{
    can_filter_parameter_struct     can_filter1;

    can_struct_para_init(CAN_FILTER_STRUCT, &can_filter1);

	if(CanPeriph == CAN_0 && FilterNum>=16)
	{
		return -1;
	}

	if(CanPeriph == CAN_1 && FilterNum<16)
	{
		return -1;
	}

    can_filter1.filter_number = FilterNum;

    can_filter1.filter_mode = CAN_FILTERMODE_MASK;
    can_filter1.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter1.filter_list_high   = (U16)(CanId>>16);
    can_filter1.filter_list_low    = (U16)CanId;
    can_filter1.filter_mask_high   = (U16)(Mask>>16);
    can_filter1.filter_mask_low    = (U16)Mask;   
    can_filter1.filter_fifo_number = CAN_FIFO1;
    can_filter1.filter_enable = ENABLE;

    can_filter_init(&can_filter1);

	can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &ReceiveMessageCan0);
	can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &ReceiveMessageCan1);
    //can_interrupt_enable(CAN1, CAN_INT_RFNE1);
	
	return 0;
}

/**
 * @brief CAN中断使能设置
 *
 * @param
*   \arg        CAN_INT_TME: transmit mailbox empty interrupt enable
	\arg        CAN_INT_RFNE0: receive FIFO0 not empty interrupt enable
	\arg        CAN_INT_RFF0: receive FIFO0 full interrupt enable
	\arg        CAN_INT_RFO0: receive FIFO0 overfull interrupt enable
	\arg        CAN_INT_RFNE1: receive FIFO1 not empty interrupt enable
	\arg        CAN_INT_RFF1: receive FIFO1 full interrupt enable
	\arg        CAN_INT_RFO1: receive FIFO1 overfull interrupt enable
	\arg        CAN_INT_WERR: warning error interrupt enable
	\arg        CAN_INT_PERR: passive error interrupt enable
	\arg        CAN_INT_BO: bus-off interrupt enable
	\arg        CAN_INT_ERRN: error number interrupt enable
	\arg        CAN_INT_ERR: error interrupt enable
	\arg        CAN_INT_WAKEUP: wakeup interrupt enable
	\arg        CAN_INT_SLPW: sleep working interrupt enable
 */
void DrvCanInterruptSet(CAN_PERIPH CanPeriph, uint32_t ItType, BOOL Enable)
{
	if(CanPeriph>=CAN_PERIPH_NUM)return;

	if(Enable)
	{
		can_interrupt_enable(sCanResource[CanPeriph].mCANx, ItType);
	}
	else
	{
		can_interrupt_disable(sCanResource[CanPeriph].mCANx, ItType);
	}

}

/**
 * @brief CAN发送数据
 *
 * @param FrameFormat CAN_FF_EXTENDED CAN_FF_STANDARD
 */
uint8_t  DrvCanSendData(CAN_PERIPH CanPeriph,uint32_t FrameFormat, uint32_t SendID, uint8_t *const SendData, U16 DataLength)
{
    S16 SendLength = DataLength;
    //uint8_t  *SendDataPtr = SendData;

    can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &TransmitMessage);

    TransmitMessage.tx_sfid = SendID;
    TransmitMessage.tx_efid = SendID;
    TransmitMessage.tx_ft = CAN_FT_DATA;
    TransmitMessage.tx_ff = FrameFormat;

    memset(TransmitMessage.tx_data, 0x00, 8);
    TransmitMessage.tx_dlen = SendLength;
    memcpy(TransmitMessage.tx_data, SendData, SendLength);

    if(can_message_transmit(sCanResource[CanPeriph].mCANx, &TransmitMessage)==CAN_NOMAILBOX)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief 获取CAN错误类型
 *
 * @param CanPeriph     CAN设备
 */
uint32_t DrvGetCanError(CAN_PERIPH CanPeriph)
{
	can_error_enum  ErrNum;
	uint32_t 			CanErrorType = 0x0;

	ErrNum = (can_error_enum)((CAN_ERR(sCanResource[CanPeriph].mCANx) & CAN_ERR_ERRN) >> 4);

	if((CAN_ERROR_BITRECESSIVE  == ErrNum) || (CAN_ERROR_BITDOMINANTER == ErrNum))
	{
		CanErrorType =  DRV_CAN_ERR_BIT;    // 位显性或隐性错误
	}

	if(CAN_ERROR_FILL == ErrNum)
	{
		CanErrorType =  DRV_CAN_ERR_STUFF;  // 位填充错误
	}

	if(CAN_ERROR_CRC == ErrNum)
	{
		CanErrorType =  DRV_CAN_ERR_CRC;	// CRC 校验错误
	}
	
	if(CAN_ERROR_FORMATE == ErrNum)
	{
		CanErrorType =  DRV_CAN_ERR_FORM;   // 格式错误
	}

	return CanErrorType;
}



#if(defined(GD32F30X_HD) || defined(GD32F30X_XD))
/**
 * @brief CAN0发送中断
 *
 * @param 
 */
void USBD_HP_CAN0_TX_IRQHandler(void)
{
	if(can_interrupt_flag_get(CAN0, CAN_INT_FLAG_MTF2) != RESET)
	{	
		can_interrupt_flag_clear(CAN0, CAN_INT_FLAG_MTF2);
	}

	if(can_interrupt_flag_get(CAN0, CAN_INT_FLAG_MTF1) != RESET)
	{	
		can_interrupt_flag_clear(CAN0, CAN_INT_FLAG_MTF1);
	}

	if(can_interrupt_flag_get(CAN0, CAN_INT_FLAG_MTF0) != RESET)
	{	
		can_interrupt_flag_clear(CAN0, CAN_INT_FLAG_MTF0);
	}

	if(CAN0TxInterruptCallBack)CAN0TxInterruptCallBack();
}

/**
 * @brief CAN0过滤器0接收中断
 *
 * @param 
 */
void USBD_LP_CAN0_RX0_IRQHandler(void)
{
	
}

/**
 * @brief CAN0过滤器1接收中断
 *
 * @param 
 */
void CAN0_RX1_IRQHandler(void)
{
	static uint8_t Can0RxBuf[13] = {0};

	if(can_interrupt_flag_get(CAN0, CAN_INT_FLAG_RFL1) != RESET)
    {
		can_interrupt_flag_clear(CAN0, CAN_INT_FLAG_RFL1);
	}

	if(can_interrupt_flag_get(CAN0, CAN_INT_FLAG_RFO1) != RESET)
    {
		can_interrupt_flag_clear(CAN0, CAN_INT_FLAG_RFO1);
	}

	if(can_interrupt_flag_get(CAN0, CAN_INT_FLAG_RFF1) != RESET)
    {
		can_interrupt_flag_clear(CAN0, CAN_INT_FLAG_RFF1);
	}

	memset(ReceiveMessageCan0.rx_data, 0x00, 8);	
	can_message_receive(CAN0, CAN_FIFO1, &ReceiveMessageCan0);

	Can0RxBuf[0] = (uint8_t)(ReceiveMessageCan0.rx_efid>>24);
	Can0RxBuf[1] = (uint8_t)(ReceiveMessageCan0.rx_efid>>16);
	Can0RxBuf[2] = (uint8_t)(ReceiveMessageCan0.rx_efid>>8);
	Can0RxBuf[3] = (uint8_t)ReceiveMessageCan0.rx_efid;
	memcpy(&Can0RxBuf[4], ReceiveMessageCan0.rx_data, 8);
	Can0RxBuf[12]= ReceiveMessageCan0.rx_dlen+4;

	if(CAN0RxInTerruptCallBack)CAN0RxInTerruptCallBack(Can0RxBuf, Can0RxBuf[12]);
}

/**
 * @brief CAN0错误和工作模式改变中断
 *
 * @param 
 */
void CAN0_EWMC_IRQHandler(void) 
{

}


#endif 

#ifdef GD32F30X_CL

//CAN过滤器1的接收中断
void CAN0_RX1_IRQHandler(void)
{   
	static uint8_t Can0Rx1Buf[13] = {0};

	if(can_interrupt_flag_get(CAN0, CAN_INT_FLAG_RFL1) != RESET)
    {
		can_interrupt_flag_clear(CAN0, CAN_INT_FLAG_RFL1);
	}

	if(can_interrupt_flag_get(CAN0, CAN_INT_FLAG_RFO1) != RESET)
    {
		can_interrupt_flag_clear(CAN0, CAN_INT_FLAG_RFO1);
	}

	if(can_interrupt_flag_get(CAN0, CAN_INT_FLAG_RFF1) != RESET)
    {
		can_interrupt_flag_clear(CAN0, CAN_INT_FLAG_RFF1);
	}

	memset(ReceiveMessageCan0.rx_data, 0x00, 8);	
	can_message_receive(CAN0, CAN_FIFO1, &ReceiveMessageCan0);

	Can0Rx1Buf[0] = (uint8_t)(ReceiveMessageCan0.rx_efid>>24);
	Can0Rx1Buf[1] = (uint8_t)(ReceiveMessageCan0.rx_efid>>16);
	Can0Rx1Buf[2] = (uint8_t)(ReceiveMessageCan0.rx_efid>>8);
	Can0Rx1Buf[3] = (uint8_t)ReceiveMessageCan0.rx_efid;
	memcpy(&Can0Rx1Buf[4], ReceiveMessageCan0.rx_data, 8);
	Can0Rx1Buf[12]= ReceiveMessageCan0.rx_dlen+4;

	if(CAN0RxInTerruptCallBack)CAN0RxInTerruptCallBack(Can0Rx1Buf, Can0Rx1Buf[12]);
}




void CAN1_RX1_IRQHandler(void)
{	
	static uint8_t Can1Rx1Buf[13] = {0};
	if(can_interrupt_flag_get(CAN1, CAN_INT_FLAG_RFL1) != RESET)
    {
		can_interrupt_flag_clear(CAN1, CAN_INT_FLAG_RFL1);
	}

	if(can_interrupt_flag_get(CAN1, CAN_INT_FLAG_RFO1) != RESET)
    {
		can_interrupt_flag_clear(CAN1, CAN_INT_FLAG_RFO1);
	}

	if(can_interrupt_flag_get(CAN1, CAN_INT_FLAG_RFF1) != RESET)
    {
		can_interrupt_flag_clear(CAN1, CAN_INT_FLAG_RFF1);
	}

	memset(ReceiveMessageCan1.rx_data, 0x00, 8);	
	can_message_receive(CAN1, CAN_FIFO1, &ReceiveMessageCan1);

	Can1Rx1Buf[0] = (uint8_t)(ReceiveMessageCan1.rx_efid>>24);
	Can1Rx1Buf[1] = (uint8_t)(ReceiveMessageCan1.rx_efid>>16);
	Can1Rx1Buf[2] = (uint8_t)(ReceiveMessageCan1.rx_efid>>8);
	Can1Rx1Buf[3] = (uint8_t)ReceiveMessageCan1.rx_efid;
	memcpy(&Can1Rx1Buf[4], ReceiveMessageCan1.rx_data, 8);
	Can1Rx1Buf[12]= ReceiveMessageCan1.rx_dlen+4;

	if(CAN1RxInTerruptCallBack)CAN1RxInTerruptCallBack(Can1Rx1Buf, Can1Rx1Buf[12]);
	
}



/**
 * @brief CAN的过滤器0接收中断
 *
 * @param 
 */
//void CAN0_RX0_IRQHandler(void)
//{
//	static uint8_t Can0RxBuf[13] = {0};

//	if(can_interrupt_flag_get(CAN0, CAN_INT_FLAG_RFL1) != RESET)
//    {
//		can_interrupt_flag_clear(CAN0, CAN_INT_FLAG_RFL0);
//	}

//	if(can_interrupt_flag_get(CAN0, CAN_INT_FLAG_RFO1) != RESET)
//    {
//		can_interrupt_flag_clear(CAN0, CAN_INT_FLAG_RFO0);
//	}

//	if(can_interrupt_flag_get(CAN0, CAN_INT_FLAG_RFF1) != RESET)
//    {
//		can_interrupt_flag_clear(CAN0, CAN_INT_FLAG_RFF0);
//	}

//	memset(ReceiveMessageCan0.rx_data, 0x00, 8);	
//	can_message_receive(CAN0, CAN_FIFO0, &ReceiveMessageCan0);

//	Can0RxBuf[0] = (uint8_t)(ReceiveMessageCan0.rx_efid>>24);
//	Can0RxBuf[1] = (uint8_t)(ReceiveMessageCan0.rx_efid>>16);
//	Can0RxBuf[2] = (uint8_t)(ReceiveMessageCan0.rx_efid>>8);
//	Can0RxBuf[3] = (uint8_t)ReceiveMessageCan0.rx_efid;
//	memcpy(&Can0RxBuf[4], ReceiveMessageCan0.rx_data, 8);
//	Can0RxBuf[12]= ReceiveMessageCan0.rx_dlen+4;

//	if(CAN0RxInTerruptCallBack)CAN0RxInTerruptCallBack(Can0RxBuf, Can0RxBuf[12]);
//}


//void CAN1_RX0_IRQHandler(void)
//{
//	static uint8_t Can0RxBuf[13] = {0};
//	
//	if(can_interrupt_flag_get(CAN1, CAN_INT_FLAG_RFL0) != RESET)
//    {
//		can_interrupt_flag_clear(CAN1, CAN_INT_FLAG_RFL0);
//	}

//	if(can_interrupt_flag_get(CAN1, CAN_INT_FLAG_RFO0) != RESET)
//    {
//		can_interrupt_flag_clear(CAN1, CAN_INT_FLAG_RFO0);
//	}

//	if(can_interrupt_flag_get(CAN1, CAN_INT_FLAG_RFF0) != RESET)
//    {
//		can_interrupt_flag_clear(CAN1, CAN_INT_FLAG_RFF0);
//	}

//	memset(ReceiveMessageCan1.rx_data, 0x00, 8);	
//	can_message_receive(CAN0, CAN_FIFO0, &ReceiveMessageCan1);

//	Can0RxBuf[0] = (uint8_t)(ReceiveMessageCan1.rx_efid>>24);
//	Can0RxBuf[1] = (uint8_t)(ReceiveMessageCan1.rx_efid>>16);
//	Can0RxBuf[2] = (uint8_t)(ReceiveMessageCan1.rx_efid>>8);
//	Can0RxBuf[3] = (uint8_t)ReceiveMessageCan1.rx_efid;
//	memcpy(&Can0RxBuf[4], ReceiveMessageCan1.rx_data, 8);
//	Can0RxBuf[12]= ReceiveMessageCan1.rx_dlen+4;

//	if(CAN0RxInTerruptCallBack)CAN0RxInTerruptCallBack(Can0RxBuf, Can0RxBuf[12]);
//}


#endif

