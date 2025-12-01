#include "appconf.h"
#include "cf.h"
#include "para.h"
#include "nvm.h"
#include "pfh_pump.h"
#include "selfchk.h"
static STR_SYS_PARA gstSysPara;
STR_SYS_PARA * const pSysPara = &gstSysPara;

 void ParaWrite(void)
{

	pSysPara->uwCrc = CjCrc32CalBuffer(pSysPara, sizeof(STR_SYS_PARA) - sizeof(pSysPara->uwCrc)); 
	(void)NVMErase(DIDEX_PFLASH_SYSPARA);
	(void)NVMWrite(DIDEX_PFLASH_SYSPARA, pSysPara, sizeof(STR_SYS_PARA));
}

static void _ParaReset(void)
{      
    pSysPara->ucCanBaud 	= 0x03;			//3-250k
    pSysPara->ucDeviceID 	= 0x01; 		//默认内部通讯ID 
    pSysPara->ucDelay 		= 30;			//喷洒延时30s
	pSysPara->ucValveFault 	= 0;			//默认阀门无故障

//-----------------------------------------------------------------
}

void ParaRead(void)
{
    UINT32 uwCrc;
    UINT16 usLen;

    usLen = sizeof(STR_SYS_PARA);
 
	c_bzero(pSysPara,usLen);
    (void)NVMRead(DIDEX_PFLASH_SYSPARA, pSysPara, usLen); 
    uwCrc = CjCrc32CalBuffer(pSysPara, usLen - sizeof(pSysPara->uwCrc));
    if (uwCrc != pSysPara->uwCrc) { 
        _ParaReset();		
        ParaWrite();    
    }
}



