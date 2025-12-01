#ifndef __JDDETECTOR_H__
#define	__JDDETECTOR_H__

//#include "swp_types.h"
#include "algo.h"

#define DETECTOR_NBR 8U

#define CTR_ADD(ctr_, ctr_limit_) do {if(ctr_ != ctr_limit_) { ctr_++; }}while(0)
#define JD_OFFLINE_SEC 180

 typedef struct {
    uint8_t ucDeviceID; //温度故障
	int32_t ssTemp; // 算法输出温度
	int32_t ssTemp1; //温度1
	int32_t ssTemp2; //温度2
	int32_t ssTemp3; //温度2
	uint32_t usCo; //一氧化碳浓度
	uint32_t usHumidity; //湿度
	uint32_t usVoc; //有机气体
	uint32_t usSmoke; //烟感
	uint8_t ucTempError; //温度故障
	uint8_t ucCoError;//一氧化碳故障
	ALARM_KIND_T coAlarmKind; //一氧化碳报警类型
	ALARM_KIND_T tempAlarmKind; // 温度报警类型
	STR_ALARM_TICKS stCoTicks; //一氧化碳报警计数
	STR_ALARM_TICKS stTempTicks; //温度报警计数
	uint8_t ucOfflineCtr; // 离线计数
	uint8_t ucOfflineStatus; // 离线
	float ssTempBuf[10]; //����
	uint8_t ucTrend;
	
} STR_JD_DETECTOR;


void JdDetecotrCommDetect (STR_JD_DETECTOR *jdptr);
void JdDtectorOfflineSet (STR_JD_DETECTOR *jdptr);
STR_JD_DETECTOR *JdDetetorPtr (uint8_t ucDeviceID);
void JdCommDetect(void);
void JdDetecotrAsk (void);
void  JdDetectorsInit(void);
#endif