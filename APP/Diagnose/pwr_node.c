#include "selfdiag.h"
#include "fault.h"

typedef struct 
{
	UINT8 PowerFault_12H;
	UINT8 PowerFault_12L;
	UINT8 PowerFault_5H;
	UINT8 PowerFault_5L;
	UINT8 PowerFault_3H;
	UINT8 PowerFault_3L;
	
}STR_POWER_FAULT;

STR_POWER_FAULT gstPowerFault;


UINT8 PowerFaultIs12H() 
{
    return gstPowerFault.PowerFault_12H ;	
}
UINT8 PowerFaultIs12L() 
{
    return gstPowerFault.PowerFault_12L ;	
}
UINT8 PowerFaultIs5H() 
{
    return gstPowerFault.PowerFault_5H  ;	
}
UINT8 PowerFaultIs5L() 
{
    return gstPowerFault.PowerFault_5L  ;	
}
UINT8 PowerFaultIs3H() 
{
    return gstPowerFault.PowerFault_3H  ;	
}
UINT8 PowerFaultIs3L()
{
    return gstPowerFault.PowerFault_3L ;	
}



//---------------------------------------------
// 24V
//---------------------------------------------
static T_DETCT_COUNT t12vHCounter = 0;
static T_DETCT_COUNT t12vLCounter = 0;

  void Diag12v(void)
{  
    uint32_t diag12v = AdcDmaValue(SAMPLE_12V);
	
    if (diag12v > PWR24V_H_VOL) { 
//		 printf("CAP_24V isHHHHH %d \n",diag12v); 
        CalcFaultDetctCounter(&t12vHCounter, TRUE);
    } else {
			
        CalcFaultDetctCounter(&t12vHCounter, FALSE);        
    }

    if (diag12v < PWR24V_L_VOL) {
        CalcFaultDetctCounter(&t12vLCounter, TRUE);
    } else {
        CalcFaultDetctCounter(&t12vLCounter, FALSE);        
    }    
}

  ENU_DETCT_RESULT Detct24vH(void)
{
    return DetctFunction(t12vHCounter, PWR24V_PASS_LIMT, PWR24V_FAIL_LIMT); 
}   

  ENU_DETCT_RESULT Detct24vL(void)
{
    return DetctFunction(t12vLCounter, PWR24V_PASS_LIMT, PWR24V_FAIL_LIMT); 
} 

  BOOL Confirm24vH(BOOL bFault) 
{
    gstPowerFault.PowerFault_12H = bFault ;	
    return TRUE;
}

  BOOL Confirm24vL(BOOL bFault) 
{
    gstPowerFault.PowerFault_12L = bFault ;	

    return TRUE;
} 

 
//---------------------------------------------
// 5V
//---------------------------------------------
static T_DETCT_COUNT t5vLCounter = 0;
static T_DETCT_COUNT t5vHCounter = 0;

  void Diag5v(void)
{   
    uint32_t diag5v = AdcDmaValue(SAMPLE_5V);
	
   // printf("CAP_5V is %d \n",diag5v);  
	
    if (diag5v > PWR5V_H_VOL) {
        CalcFaultDetctCounter(&t5vHCounter, TRUE);
    } else {
        CalcFaultDetctCounter(&t5vHCounter, FALSE);        
    }

    if (diag5v < PWR5V_L_VOL) {
        CalcFaultDetctCounter(&t5vLCounter, TRUE);
    } else {
        CalcFaultDetctCounter(&t5vLCounter, FALSE);        
    }    
}

  ENU_DETCT_RESULT Detct5vH(void)
{
    return DetctFunction(t5vHCounter, PWR5V_PASS_LIMT, PWR5V_FAIL_LIMT); 
} 
  
  ENU_DETCT_RESULT Detct5vL(void)
{
    return DetctFunction(t5vLCounter, PWR5V_PASS_LIMT, PWR5V_FAIL_LIMT); 
}

  BOOL Confirm5vH(BOOL bFault) 
{
	gstPowerFault.PowerFault_5H = bFault ;	

    return TRUE;
}

  BOOL Confirm5vL(BOOL bFault) 
{
	gstPowerFault.PowerFault_5L = bFault ;	
    return TRUE;
}

#if 0
//---------------------------------------------
// 3V
//---------------------------------------------
static T_DETCT_COUNT t3vLCounter = 0;
static T_DETCT_COUNT t3vHCounter = 0;

  void Diag3v() 
{
   uint32_t diag3v = AdcDmaValue(SAMPLE_3V);
    
//	printf("CAP_3V is %d \n",diag3v); 
	
    if (diag3v > PWR3V_H_VOL) {
        CalcFaultDetctCounter(&t3vHCounter, TRUE);
    } else {
        CalcFaultDetctCounter(&t3vHCounter, FALSE);        
    }

    if (diag3v < PWR3V_L_VOL) {
        CalcFaultDetctCounter(&t3vLCounter, TRUE);
    } else {
        CalcFaultDetctCounter(&t3vLCounter, FALSE);        
    }    
}

  ENU_DETCT_RESULT Detct3vH(void)
{
    return DetctFunction(t3vHCounter, PWR3V_PASS_LIMT, PWR3V_FAIL_LIMT); 
}

  ENU_DETCT_RESULT Detct3vL(void)
{
    return DetctFunction(t3vLCounter, PWR3V_PASS_LIMT, PWR3V_FAIL_LIMT); 
}

  BOOL Confirm3vH(BOOL bFault) 
{
	gstPowerFault.PowerFault_3H = bFault ;	

    return TRUE;
}

  BOOL Confirm3vL(BOOL bFault) 
{
	gstPowerFault.PowerFault_3L = bFault ;	
    return TRUE;
}

#endif
