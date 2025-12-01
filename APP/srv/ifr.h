#ifndef __IFR__H__
#define __IFR__H__

#include "stdtype.h"
//#include "serv.h"

//#define _CROSS_COMPLIER_

#ifdef _CROSS_COMPLIER_
    #define _IFR_SW_REBOOT()
    #define _IFR_HW_REBOOT()
    #define _IFR_ERASE_APP_VALID() 
    #define _IFR_CAN_IN_SEND(_CAN_DATA)
    #define _IFR_CAN_EX_SEND(_CAN_DATA)
    #define _IFR_MSLEEP(N)
    #define _IRF_DISABLE_IRQ_
    #define _IRF_ENABLE_IRQ_    
#else
    #define _IFR_SW_REBOOT() irfWdt->stop()
    #define _IFR_HW_REBOOT() do{irfWdt.enable();irfWdt.stop();}while(0) 
    
#if 0 //KP-WAIT   
    #define _IFR_ERASE_APP_VALID() do { \
        EepromErase(0x100000);\
        EepromProg(0x100000, (UINT16*)(&usBootFlag), 1);\
        COPCTL = 0x03U;\
        } while(0)
#else
    #define _IFR_ERASE_APP_VALID() do { \
        EepromErase(0x100000);\
        EepromProg(0x100000, (UINT16*)(&usBootFlag), 1);\
        } while(0)
#endif
    #define _IFR_CAN_IN_SEND(_CAN_DATA) (void)InCan1Send(_CAN_DATA) 
    #define _IFR_CAN_EX_SEND(_CAN_DATA) (void)InCan0Send(_CAN_DATA)
    #define _IFR_MSLEEP(N) 				delay_ms(N)
	
    #define _IRF_DISABLE_IRQ_ 	__disable_irq()
    #define _IRF_ENABLE_IRQ_  	__enable_irq()
#endif

#endif /* __IFR__H__ */
