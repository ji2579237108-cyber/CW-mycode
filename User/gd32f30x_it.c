#include "gd32f30x_it.h"
#include "systick.h"



extern FlagStatus can0_receive_flag;
extern FlagStatus can1_receive_flag;


//extern BOOL  ucBackWakeup_flag;
//extern uint16_t usCou_30S;


//void canrx_convert(can_receive_message_struct *canRxMsg, STR_CAN_STD_TYPE *canf)
//{
//    int i;
//    canf->len = canRxMsg->rx_dlen;
//    
//    if (canRxMsg->rx_ff == CAN_FF_EXTENDED) 
//		{
//        canf->exd = 1;
//		} 
//	else {
//        canf->exd = 0;
//		}
//    canf->rtr = canRxMsg->rx_ft;//CAN_FT_DATA=0 or CAN_FT_REMOTE=2
//    if (canf->exd) 
//		{
//        canf->canid.exdid = canRxMsg->rx_efid;
//		} 
//	else 
//		{
//        canf->canid.stdid = canRxMsg->rx_sfid;
//		}
//	for (i = 0; i < canRxMsg->rx_dlen; i++) {
//        canf->data[i] = canRxMsg->rx_data[i];
//	}
//}





void SysTick_Handler(void)
{
    delay_decrement();
}

void CAN0_RX0_IRQHandler(void)
{
//    
//	static STR_CAN_STD_TYPE stCanf;
//	static can_receive_message_struct receive_message;
//    can_message_receive(CAN0, CAN_FIFO0, &receive_message);
//	canrx_convert(&receive_message, &stCanf);
//	OtCanBufferAppend(&stCanf);
   
    
}
void CAN1_RX0_IRQHandler(void)
{
//    
//	static STR_CAN_STD_TYPE stCanf;
//	static can_receive_message_struct receive_message;
//    can_message_receive(CAN1, CAN_FIFO0, &receive_message);
//	canrx_convert(&receive_message, &stCanf);
//	CanBufferAppend(&stCanf);
 
}


void TIMER1_IRQHandler(void)
{
		
}



void TIMER2_IRQHandler(void)
{

}

