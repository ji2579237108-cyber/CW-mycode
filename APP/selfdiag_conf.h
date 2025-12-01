#ifndef _SELFDIAG_OCNF_H_
#define _SELFDIAG_OCNF_H_

#include "stdtype.h"
/**
 * configuration1: the ssDTCFaultDetectionCounter's typ
 * configuration2: SelfDiagMainProcess's period(ms)
 * configuration3: Slow diag
 **/
  
/* -- configuration0 ------------------------------------------*/
/* -- configuration1 ------------------------------------------*/

#define T_DETCT_COUNT_TYPE INT16_TYPE
#if T_DETCT_COUNT_TYPE == INT8_TYPE 
    #define T_DETCT_COUNT INT8
    #define T_DETCT_MIN   INT8_MIN
    #define T_DETCT_MAX   INT8_MAX
#elif T_DETCT_COUNT_TYPE == INT16_TYPE
    #define T_DETCT_COUNT INT16
    #define T_DETCT_MIN   INT16_MIN
    #define T_DETCT_MAX   INT16_MAX
#else
    #error "T_DETCT_COUNT_TYPE"
#endif

/* -- configuration2 ------------------------------------------*/
#define DIAG_PERIOD_CHECK(_FAST_PERIOD_,_SLOW_PERIOD_) \
    (_FAST_PERIOD_ == 0 || _SLOW_PERIOD_ == 0 || \
     _FAST_PERIOD_ > _SLOW_PERIOD_)
     
#define SELFDIAG_FAST_PERIOD 50                       
#define SELFDIAG_SLOW_PERIOD 100 //ms

#if DIAG_PERIOD_CHECK(SELFDIAG_FAST_PERIOD,SELFDIAG_SLOW_PERIOD)
    #error "DIAG_PERIOD"
#endif
//自诊断配置快速执行规则检测
#define SELFDIAG_FAST_CHECK(_FAIL_LIMIT_, _PASS_LIMIT_, _DETCT_PERIOD_) \
    (_FAIL_LIMIT_ == 0 || _FAIL_LIMIT_ > T_DETCT_MAX || \
     _PASS_LIMIT_ == 0 || (_PASS_LIMIT_ * -1 ) < T_DETCT_MIN || \
     _DETCT_PERIOD_ == 0 || \
     _DETCT_PERIOD_ % SELFDIAG_FAST_PERIOD != 0)
//自诊断配置常规规则检测
#define SELFDIAG_CHECK(_FAIL_LIMIT_, _PASS_LIMIT_, _DETCT_PERIOD_) \
    (_FAIL_LIMIT_ == 0 || _FAIL_LIMIT_ > T_DETCT_MAX || \
     _PASS_LIMIT_ == 0 || (_PASS_LIMIT_ * -1 ) < T_DETCT_MIN || \
     _DETCT_PERIOD_ == 0 || \
     _DETCT_PERIOD_ % SELFDIAG_FAST_PERIOD != 0 || _DETCT_PERIOD_ % SELFDIAG_SLOW_PERIOD != 0)    
     
#define PWR_FAIL_LIMT       1
#define PWR_PASS_LIMT      10 
#define PWR_DETCT_PERIOD  100  //ms                       
#if SELFDIAG_CHECK(PWR_FAIL_LIMT,PWR_PASS_LIMT,PWR_DETCT_PERIOD) 
    #error "SELFDIAG_CHECK_PWR"
#endif

//-----------------------------------------------------------------------
//按键自检和自诊断
//-----------------------------------------------------------------------
#define FRKEY_DETCT_PERIOD 1000 //1000ms
//按键自诊断
#define FRKEY_FAIL_LIMT    1800 //30m,1800000ms/1000ms
#define FRKEY_PASS_LIMT      10 //10s,10000ms/1000ms
#if SELFDIAG_CHECK(FRKEY_FAIL_LIMT,FRKEY_PASS_LIMT,FRKEY_DETCT_PERIOD) 
    #error "SELFDIAG_CHECK_FRKEY"
#endif
//按键自检
#define FRKEY_SELFCHK_FAIL_LIMT      1 //1s
#define FRKEY_SELFCHK_PASS_LIMT   3600 //1h,3600000ms/1000ms
#if SELFDIAG_CHECK(FRKEY_SELFCHK_FAIL_LIMT,FRKEY_SELFCHK_PASS_LIMT,FRKEY_DETCT_PERIOD) 
    #error "SELFDIAG_CHECK_FRKEY"
#endif

//-----------------------------------------------------------------------
//电池自检和自诊断
//-----------------------------------------------------------------------
#define BATLOW_DETCT_PERIOD 1000 //1000ms
//电池自诊断
#define BATLOW_FAIL_LIMT      30 //10min,600000ms/1000ms
#define BATLOW_PASS_LIMT      10 //10s,10000ms/1000ms
#if SELFDIAG_CHECK(BATLOW_FAIL_LIMT,BATLOW_PASS_LIMT,BATLOW_DETCT_PERIOD) 
    #error "SELFDIAG_CHECK_LEVEL"
#endif
                             

//-----------------------------------------------------------------------
//液位自检和自诊断
//-----------------------------------------------------------------------
#define LEVEL_DETCT_PERIOD 1000 //1000ms
//液位自诊断
#define LEVEL_FAIL_LIMT      30 //10min,600000ms/1000ms
#define LEVEL_PASS_LIMT      10 //10s,10000ms/1000ms

#if SELFDIAG_CHECK(LEVEL_FAIL_LIMT,LEVEL_PASS_LIMT,LEVEL_DETCT_PERIOD) 
    #error "SELFDIAG_CHECK_LEVEL"
#endif
//液位自检
#define LEVEL_SELFCHK_FAIL_LIMT        1 //1s
#define LEVEL_SELFCHK_PASS_LIMT     	3600 //1h,3600000ms/1000ms
#if SELFDIAG_CHECK(LEVEL_SELFCHK_FAIL_LIMT,LEVEL_SELFCHK_PASS_LIMT,LEVEL_DETCT_PERIOD) 
    #error "SELFDIAG_CHECK_LEVEL"
#endif


//阀门开路的诊断
#define VAL_OPENCIR_FAIL_LIMT      30 //10min,600000ms/1000ms  阀门开路
#define VAL_OPENCIR_PASS_LIMT      10 //10s,10000ms/1000ms

//探测器短路诊断
#define FD_SHORTCIR_FAIL_LIMT      30 //10min,600000ms/1000ms  阀门开路
#define FD_SHORTCIR_PASS_LIMT      10 //10s,10000ms/1000ms


//-----------------------------------------------------------------------
//24v自诊断
//-----------------------------------------------------------------------
#define PWR24V_DETCT_PERIOD  100 //100ms
//24v自诊断
#define PWR24V_FAIL_LIMT      20 //2s,2000ms/100ms
#define PWR24V_PASS_LIMT      20 //2s,2000ms/100ms
#if SELFDIAG_CHECK(PWR24V_FAIL_LIMT,PWR24V_PASS_LIMT,PWR24V_DETCT_PERIOD) 
    #error "SELFDIAG_CHECK_PWR24V"
#endif
#define PWR24V_H_VOL          16000 //mV
#define PWR24V_L_VOL          9000 //mV

//-----------------------------------------------------------------------
//3v自诊断
//-----------------------------------------------------------------------
#define PWR3V_DETCT_PERIOD  100 //100ms
//3v自诊断
#define PWR3V_FAIL_LIMT      20 //2s,2000ms/100ms
#define PWR3V_PASS_LIMT      20 //2s,2000ms/100ms
#if SELFDIAG_CHECK(PWR3V_FAIL_LIMT,PWR3V_PASS_LIMT,PWR3V_DETCT_PERIOD) 
    #error "SELFDIAG_CHECK_PWR12V"
#endif
#define PWR3V_H_VOL          4000 //V
#define PWR3V_L_VOL          3000 //V

//-----------------------------------------------------------------------
//5v自诊断
//-----------------------------------------------------------------------
#define PWR5V_DETCT_PERIOD  100 //100ms
//24v自诊断
#define PWR5V_FAIL_LIMT      20 //2s,2000ms/100ms
#define PWR5V_PASS_LIMT      20 //2s,2000ms/100ms
#if SELFDIAG_CHECK(PWR5V_FAIL_LIMT,PWR5V_PASS_LIMT,PWR5V_DETCT_PERIOD) 
    #error "SELFDIAG_CHECK_PWR5V"
#endif
#define PWR5V_H_VOL          5300 //mV
#define PWR5V_L_VOL          4800 //mV
//-----------------------------------------------------------------------
#define NVM_DETCT_PERIOD   1000 //1s
//-----------------------------------------------------------------------
#define EXCAN_DETCT_PERIOD 1000 //1s
#define EXCAN_FAIL_LIMT      60 //1m,60000ms/1000ms
#define EXCAN_PASS_LIMT       1 //1s,1000ms/1000ms
//-----------------------------------------------------------------------
#define EXCAN_DETCT_PERIOD 1000 //1s
#define BALLVALVE_FAIL_LIMT      10 //1m,60000ms/1000ms
#define BALLVALVE_PASS_LIMT       1 //1s,1000ms/1000ms

//NTC故障检测配置
#define NTC_CRITICAL_MIN_VOL    50
#define NTC_CRITICAL_MAX_VOL     3250 
#define NTC_SIGHL_FAIL_LIMIT      5
#define NTC_SIGHL_PASS_LIMIT      40
#define NTC_SIGHL_DETCT_PERIOD   1000 //ms
#if SELFDIAG_CHECK(EXCAN_FAIL_LIMT,EXCAN_PASS_LIMT,EXCAN_DETCT_PERIOD) 
    #error "SELFDIAG_CHECK_EXCAN"
#endif

#endif /* _SELFDIAG_OCNF_H_ */ 
