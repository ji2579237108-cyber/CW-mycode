#include "canboot.h"
#include "stdint.h"
#include "nvm.h"
#include "inflash.h"
#include "MidCan.h"

#include "DrvWdgt.h"
/**********************************************
函数功能：boot 擦除扇区
参数说明：
函数返回：无
***********************************************/
 void  CAN_BOOT_ErasePage(uint32_t sect)
 {   	 
	InFlash_ErasePage(sect);
 }
  
/**********************************************
函数功能：boot报文处理（重启至boot区）
参数说明：
函数返回：无
***********************************************/
 BOOL CanBootHandler(STR_CAN_STD_TYPE *pCanf) 
{
	STR_CAN_STD_TYPE stCanf = { 0 };
	 if ((pCanf->CANEXDID & 0xFFFFFF00) == (0x1B000000 + ((UINT32)BOOT_ID << 8)) )
	 {
		if((pCanf->data[0] == 0X02) &&(pCanf->data[1] == 0X5A) &&(pCanf->data[2] == 0X4E))
		{		
			  CAN_BOOT_ErasePage(APPVALID_STORE_ADDR); 
				stCanf.CANEXDID = pCanf->CANEXDID;
				stCanf.J1939SA = pCanf->J1939PS;
				stCanf.J1939PS = pCanf->J1939SA;
				stCanf.exd = 1;
				stCanf.len = 1;
				stCanf.data[0] = 0XA0;				
				ExCan0Send(&stCanf);
			   DrvFwdgtInit(10,FWDGT_PSC_DIV64);
			   
			return TRUE;
		} 
		else 
		{
			return FALSE;
		}
		
	 }
	 return FALSE;
	
}

