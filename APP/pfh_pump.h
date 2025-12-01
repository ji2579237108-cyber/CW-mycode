#ifndef _PFH_PUMP_H_
#define _PFH_PUMP_H_

#include "cf.h"
#include "CWDrv.h"
//#include "board.h"
//#include "serv.h"
//#include "app.h"

#define DBUG_PHF_PUMP

//#define DBUG_PVOUTFIRE


#define PV3_FIRE_TYPE   0x13 //3公斤全氟己酮泵组式
#define PV6_FIRE_TYPE   0x16 //6公斤全氟己酮泵组式
#define PV8_FIRE_TYPE   0x18 //8公斤全氟己酮泵组式
#define PVA_FIRE_TYPE   0x1A //10公斤全氟己酮泵组式

#define PV_IDLE         0   //泵阀未开启

#define VALVE_OPEN      1   //阀开启
#define PUMP_OPEN       2   //泵开启
#define PUMP_CLOSE      3   //泵关闭
#define VALVE_CLOSE     4   //阀关闭
#define PV_STOP         5   //泵阀关闭

#define FIRE_READY_OPEN		1
#define FIRE_NO_READY_OPEN  0


#define FIRE_NOT_OPEN       0  //灭火器未启动
#define FIRE_OPEN_RUNING    1    //灭火器启动
#define FIRE_OPEN_SUC       2
#define FIRE_OPEN_FAIL      3

#define PUMPSIZE(x) (sizeof(x)/sizeof(x[0]))

#ifdef PVOUTFIRE_FUN_ALL
typedef struct PV_RUN_TIME
{
    UINT8  ucPOpenTime;  //泵开启时间
    UINT8  ucVOpenTime;  //阀开启时间 
    UINT8  ucVCloseTime; //阀关闭时间
    UINT8  ucPCloseTime; //泵关闭时间
}STR_PV_RUN_TIME; //泵组式运行时间
#else
#define VALE_OPEN_SEC   1
#define VALVE_CLOSE_SEC 1
typedef struct PV_RUN_TIME
{
    UINT8  ucVOpenTime;  //阀开启时间 
    UINT8  ucPCloseTime; //泵关闭时间
}STR_PV_RUN_TIME; //泵组式运行时间
#endif

typedef struct {
    const STR_PV_RUN_TIME *pPvRunTime; //时间表
    UINT8 ucSumPeriod; // 喷射周期数
}STR_PV_TYPE;

typedef struct PV_RUN_PARA
{
    UINT32 uwTimeStamp;  //4时间
    UINT8  ucReqType;    //5请求类型
    UINT8  ucPvModel;    //6泵组规格
    UINT8  ucFireOpen;   //7灭火器启动    
    UINT8  ucLevel;      //8液位
    UINT8  ucPresOkNum;  //9建压成功次数
    UINT8  ucRunPeriod;  //10周期数
    UINT8  ucRunEnd;     //11启动结束
    UINT8  ucRev;        //12预留
    UINT32 ucCrc;        //16CRC校验
} STR_PV_RUN_PARA;

const STR_PV_RUN_PARA * PfhPumpGetPara(void); 
void PfhPumpRunProcess(void);
void PfhPumpInit(void);
void PfhPumpRequest(UINT8 ucReqType);
void PvTypeSet(UINT8 ucType);
void PfhPumpClr(void);
UINT8 PfhPumpStatus(void);

#endif /* _PFH_PUMP_H_ */
