#ifndef __ALGO_H__
#define	__ALGO_H__
#include "DataTypeDef.h"
#include "stdint.h"
#include "sp_trend.h"
#include "stdtype.h"




typedef struct
{
	float ntc1;
	float ntc2;
	INT16 SmokeValueA;
	INT16 SmokeValueB;
	UINT8 SmokeFlag;
}STR_SensorValue;

typedef union
{
    struct
    {
        UINT8 Fault_NTC    : 1;  // NTC故障标志位
        UINT8 Fault_Power  : 1;  // 电源故障标志位
        UINT8 Fault_SMOKE  : 1;  // 烟雾故障标志位
        UINT8 Fault_Press  : 1;  // 压力故障标志位
        UINT8 Fault_HSD    : 1;  // HSD故障标志位
        UINT8 reserved     : 3;  // 保留位
    } bits;
    UINT8 all;  // 整体状态字节
} STR_FaultResult;

extern STR_FaultResult FaultResult;
extern STR_SensorValue SensorValue;
extern UINT8 SysLevel;
void GetSensorValue(void);
void SysFaultDiag(void);
void AlgoProcessing(void);

#endif


