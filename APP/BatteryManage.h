#ifndef _BATTERYMANAGE_H_
#define _BATTERYMANAGE_H_

#include "stdtype.h"
#include "stdlib.h"
#include "stdio.h"
#include "systick.h"
#include "CWDrv.h"


#define BAT_STOP_UP_VAL 	15100
#define BAT_UP_LIMIT    	14500
#define BAT_UNDER_LIMIT 	14300
#define BAT_LOWVOL_LIMIT 	10000


#define POWER_24V_LOW_LIMIT  9400


#define BAT_SACAN_MS 		1000

// 电池充电使能控制宏定义
#define BUCK_ENABLE()       DioWriteChannel(BUCK_EN, 1)//高电平有效
#define BUCK_DISABLE()      DioWriteChannel(BUCK_EN, 0)
#define BOOST_ENABLE()      DioWriteChannel(BOOST_EN, 0)//低电平有效
#define BOOST_DISABLE()     DioWriteChannel(BOOST_EN, 1)
#define CHARGE_ENABLE()     do { BUCK_ENABLE(); BOOST_ENABLE(); } while(0)
#define CHARGE_DISABLE()    do { BUCK_DISABLE(); BOOST_DISABLE(); } while(0)

UINT8 GetBatChargeState(void);
void ChargeInit(void);
extern void BatMgrMainProcess(void); 

#endif
