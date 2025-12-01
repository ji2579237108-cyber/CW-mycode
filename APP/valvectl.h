#ifndef __VALVECTL_H__
#define	__VALVECTL_H__

#include "stdtype.h"

typedef enum
{
	CONTROL_NORMAL = 0, //正常
	CONTROL_DELAY, //延时
	CONTROL_OPEN, //喷洒
}ENU_VAL_CONTROL;

typedef enum
{
	VALVE_TYPE_NONE = 0,
	VALVE_TYPE_LEVEL4,
	VALVE_TYPE_OPEN,
	VALVE_TYPE_CAN,
}ENU_VAL_TYPE;
typedef struct
{
	ENU_VAL_CONTROL control_status;	//控制状态
	ENU_VAL_TYPE 	ucValveType;	//启阀类型
	UINT32 			uiValveTime;	//启阀时间
	UINT32			uiValveDelayStart;	//延时开始时间
}STR_TYPE_VALVE;
extern STR_TYPE_VALVE g_stValveCtl;

extern void ValveCtrlInit(void);
extern void ValveCtrl_Reset(void);
extern void ValveCtrl_SetDelay(UINT32 duration);
extern ENU_VAL_CONTROL ValveCtrl_GetStatus(void);
extern void ValveCtrl_Process(void);
extern void ValveCtrl_SetValveType(ENU_VAL_TYPE type);


#endif
