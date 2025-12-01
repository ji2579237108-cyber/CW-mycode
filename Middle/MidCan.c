#include "MidCan.h"
#include "systick.h"
#include "canbuffer.h"
#include "MidDio.h"

/**
 * @brief Can0的回调函数
 *
 * @param DataPtr[0]~DataPtr[3]:帧ID
 *		  DataPtr[4]~DataPtr[11]:帧数据
 *		  DataPtr[12]：数据长度+4
 *@note  将接收到的数据再发送回去
 */
static void Can0DataReceiveDeal(uint8_t* DataPtr, uint8_t DataLen)
{	
    STR_CAN_STD_TYPE RecPtr;
	UINT8 DataBuf[12] = {0};
	memcpy((UINT8 *)DataBuf, DataPtr, DataLen);

	RecPtr.canid.exdid = DataBuf[0] + (DataBuf[1]<<8) +(DataBuf[2]<<16) + (DataBuf[3]<<24);
	RecPtr.J1939PRDP   = DataBuf[0];
	RecPtr.J1939PF     = DataBuf[1];
	RecPtr.J1939PS     = DataBuf[2];
	RecPtr.J1939SA     = DataBuf[3];
	RecPtr.exd = 1;
	RecPtr.len = DataLen-4;
	memcpy(RecPtr.data, DataPtr+4, RecPtr.len);
	OtCanBufferAppend(&RecPtr);
}

/**
 * @brief Can1的回调函数
 *
 * @param DataPtr[0]~DataPtr[3]:帧ID
 *		  DataPtr[4]~DataPtr[11]:帧数据
 *		  DataPtr[12]：数据长度+4
 *@note  将接收到的数据再发送回去
 */
static void Can1DataReceiveDeal(uint8_t* DataPtr, uint8_t DataLen)
{
    STR_CAN_STD_TYPE RecPtr;
	UINT8 DataBuf[12] = {0};
	memcpy((UINT8 *)DataBuf, DataPtr, DataLen);

	RecPtr.canid.exdid = DataBuf[0] + (DataBuf[1]<<8) +(DataBuf[2]<<16) + (DataBuf[3]<<24);
	RecPtr.J1939PRDP   = DataBuf[0];
	RecPtr.J1939PF     = DataBuf[1];
	RecPtr.J1939PS     = DataBuf[2];
	RecPtr.J1939SA     = DataBuf[3];
	RecPtr.exd = 1;
	RecPtr.len = DataLen-4;
	memcpy(RecPtr.data, DataPtr+4, RecPtr.len);
	CanBufferAppend(&RecPtr);
	
}


/**
 * @brief Can初始化
 *
 * @note 初始化CAN0 CAN1，使用过滤器1，如需使用过滤器0
 *  需要修改：
 *  DrvCanNvicConfig函数 中的中断配置 CANx_RX1_IRQn 改为 CANx_RX0_IRQn
 *  DrvCanFilterConfig函数 中过滤器序号 CAN_FIFO1 改为 CAN_FIFO0
 *  DrvCanInterruptSet函数 中输入参数：CAN_INT_RFNE1 改为CAN_INT_RFNE0
 *  取消 CAN0_RX0_IRQHandler CAN1_RX0_IRQHandler的注释
 *  注意同一个CAN 的两个接收中断使用的是同一个回调函数
 */  	
void CanInit(UINT8 Can0Baud, UINT8 Can1Baud)
{ 
//    UINT16 Baud_Can0, Baud_Can1 ;
// 
//    Baud_Can0 = Can0Baud;
//    Baud_Can1 = Can1Baud;
	DioWriteChannel(CAN_STB,  1);
	
    DrvCanDeviceInit(CAN_0, 250);
    DrvCanDeviceInit(CAN_1, 250);
 
    DrvCanFilterConfig(CAN_0, 0, 0, 1); //Ex
    DrvCanFilterConfig(CAN_1, 0, 0, 16); //IN
 
    DrvCanInterruptSet(CAN_0, CAN_INT_RFNE1, TRUE);
    DrvCanInterruptSet(CAN_1, CAN_INT_RFNE1, TRUE);//使用滤波器FIFO1

    CAN0RxInTerruptCallBack = Can0DataReceiveDeal;
    CAN1RxInTerruptCallBack = Can1DataReceiveDeal; 
}


#define CAN_EXTENDED 	1
#define CAN_STANDARD	0


void ExCan0Send(STR_CAN_STD_TYPE *can_ptr)
{
	if(can_ptr->exd == CAN_EXTENDED)
	{
		DrvCanSendData(CAN_0, CAN_FF_EXTENDED, can_ptr->CANEXDID, can_ptr->data, can_ptr->len);
	}
	else if(can_ptr->exd == CAN_STANDARD)
	{
		DrvCanSendData(CAN_0, CAN_FF_STANDARD, can_ptr->CANEXDID, can_ptr->data, can_ptr->len);
	}
}



void InCan1Send(STR_CAN_STD_TYPE *can_ptr)
{
	if(can_ptr->exd == CAN_EXTENDED)
	{
		DrvCanSendData(CAN_1, CAN_FF_EXTENDED, can_ptr->CANEXDID, can_ptr->data, can_ptr->len);
	}
	else if(can_ptr->exd == CAN_STANDARD)
	{
		DrvCanSendData(CAN_1, CAN_FF_STANDARD, can_ptr->CANEXDID, can_ptr->data, can_ptr->len);
	}
	delay_ms(3); //DO NOT DELETE  ！！！  内Can自动编码使用
}



// 适用于80A给簇下07A升级使用
void InCan1Send_UpGrade(STR_CAN_STD_TYPE *can_ptr)
{
	if(can_ptr->exd == CAN_EXTENDED)
	{
		DrvCanSendData(CAN_1, CAN_FF_EXTENDED, can_ptr->CANEXDID, can_ptr->data, can_ptr->len);
	}
	else if(can_ptr->exd == CAN_STANDARD)
	{
		DrvCanSendData(CAN_1, CAN_FF_STANDARD, can_ptr->CANEXDID, can_ptr->data, can_ptr->len);
	}
}









