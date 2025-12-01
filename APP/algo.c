#include "algo.h"
#include "para.h"
#include "pwr_node.h"
#include "ntc_node.h"
#include "ntc.h"
#include "BM22S2301.h"
#include "DiCtrl.h"
#include "DoCtrl.h"
#include "LEDCtrl.h"
#include "MidDio.h"
#include "para.h"
#include "valvectl.h"
#include "time.h"


STR_SensorValue SensorValue = {0};
STR_FaultResult FaultResult = {0};
UINT8 SysLevel = 0;


void GetSensorValue(void)
{
    static UINT32 StayReadyTime = 0;
    if (tSysTime < StayReadyTime + 300) {//5min预热
        SensorValue.ntc1 = 0;
        SensorValue.ntc2 = 0;
        SensorValue.SmokeValueA = 0;
        SensorValue.SmokeValueB = 0;
        SensorValue.SmokeFlag = 0;
        return ;
    }
    NTC1GetValue(&SensorValue.ntc1);
    NTC2GetValue(&SensorValue.ntc2);
    SensorValue.SmokeValueA = g_BM22S2301.BM22S2301_ValueA;
    SensorValue.SmokeValueB = g_BM22S2301.BM22S2301_ValueB;
    BM22S2301_GetValue(&SensorValue.SmokeFlag);
}

void SysFaultDiag(void)
{
    UINT8 powerFault = 0;
    UINT8 ntcFault = 0;
    UINT8 smokeFault = 0;
    
    // 检查电源故障 - 检查所有电源相关的故障位
    if ((PowerFaultIs12H() != 0) || 
        (PowerFaultIs12L() != 0) || 
        (PowerFaultIs5H() != 0) || 
        (PowerFaultIs5L() != 0))
    {
        powerFault = 1;
    }
    else {
        powerFault = 0;
    }
    
    // 检查NTC故障 - 检查所有NTC相关的故障位
    if ((Ntc1_FaultIsH() != 0) || 
        (Ntc1_FaultIsL() != 0) || 
        (Ntc2_FaultIsH() != 0) || 
        (Ntc2_FaultIsL() != 0) || 
        (Ntc_FaultIsSignal() != 0))
    {
        ntcFault = 1;
    }
    else {
        ntcFault = 0;
    }
    //检查烟雾离线故障
    if (BM22S2301_GetSelfCheck() != 0)
    {
        smokeFault = 1;
    }
    else {
        smokeFault = 0;
    }
    
    // 更新故障结果结构体
    FaultResult.bits.Fault_Power = powerFault;
    FaultResult.bits.Fault_NTC = ntcFault;
    FaultResult.bits.Fault_SMOKE = smokeFault;
    if (g_stDiCtrl.pins[DI_PRESS_INDEX].stable_state == STD_LOW ) {
        FaultResult.bits.Fault_Press = 1;
    }
    else {
        FaultResult.bits.Fault_Press = 0;
    }
}

#define NTC_FILTER_WINDOW_SIZE    10

// NTC滤波缓冲区结构
typedef struct
{
    float buffer[NTC_FILTER_WINDOW_SIZE];
    uint8_t index;
    uint8_t count;
} STR_NtcFilter;
STR_NtcFilter g_stNtc1Filter = {0};

static float AlgoGetMaxTemp(float ntc1, float ntc2) 
{
    return (ntc1 > ntc2) ? ntc1 : ntc2;
}

/**
 * @brief NTC均值滤波函数
 * @param filter 滤波器结构体指针
 * @param new_value 新的采样值
 * @return 滤波后的值
 */
static float NtcMeanFilter(STR_NtcFilter *filter, float new_value)
{
    float sum = 0.0f;
    float average = 0.0f;
    
    // 将新值存入缓冲区
    filter->buffer[filter->index] = new_value;
    
    // 更新索引和计数
    filter->index++;
    if (filter->index >= NTC_FILTER_WINDOW_SIZE)
    {
        filter->index = 0;
    }
    
    if (filter->count < NTC_FILTER_WINDOW_SIZE)
    {
        filter->count++;
    }
    
    // 计算均值
    for (uint8_t i = 0; i < filter->count; i++)
    {
        sum += filter->buffer[i];
    }
    
    average = sum / filter->count;
    
    return average;
}
void AlgoProcessing()
{
    static UINT8 ucOldSysLevel = 0;
    static UINT8 Muteflag = 0;
    // 1. 基于传感器数据进行处理
    float ntc1_temp = SensorValue.ntc1;
    float ntc2_temp = SensorValue.ntc2;
//    INT16 smoke_a = SensorValue.SmokeValueA;
//    INT16 smoke_b = SensorValue.SmokeValueB;
    UINT8 smoke_flag = SensorValue.SmokeFlag;
    
    // 2. 基于故障诊断结果进行处理
    UINT8 ntc_fault = FaultResult.bits.Fault_NTC;
    UINT8 power_fault = FaultResult.bits.Fault_Power;(void)power_fault;//消除编译警告
    UINT8 smoke_fault = FaultResult.bits.Fault_SMOKE;
    /*故障设为30度 */
    if (ntc_fault == 1 || smoke_fault == 1) {
        ntc1_temp = 30;
        ntc2_temp = 30;
    }
    /*烟雾故障设为0*/
    if(smoke_fault == 1 || smoke_fault == 1)
    {
        smoke_flag = 0;
    }
    float max_temp = AlgoGetMaxTemp(ntc1_temp, ntc2_temp);
    float filtered_temp = NtcMeanFilter(&g_stNtc1Filter, max_temp);
    if (filtered_temp >= 80 && smoke_flag == 1)
    {
        // 同时满足条件，SysLevel为4级
        SysLevel = 4;
    }
    else if (filtered_temp >= 80 || smoke_flag == 1)
    {
        // 满足其中一个条件，SysLevel为2级
        SysLevel = 2;
    }
    else
    {
        // 都不满足，SysLevel为0级
        SysLevel = 0;
    }
    
    /*对输入做判断*/
    /*产生报警输入，进行联动报警*/
    if(DiCtrl_GetAlarmState() == STD_LOW)
    {
        SysLevel = 4;
    }
    if(DiCtrl_GetMuteToggle() == STD_LOW)
    {
        Muteflag = 1;
        DiCtrl_ResetMuteToggle();
    }
    /*恢复声音*/
    if(ucOldSysLevel < 4 && SysLevel == 4)
    {
        Muteflag = 0;
    }
    ucOldSysLevel = SysLevel;
    /*报警等级处理*/
    switch (SysLevel)
    {
        case 0:
            //正常状态只重置火警信号，报警信号触发后一直输出
            g_stDoCtrl[DO_LEVEL2_INDEX].current_state = DO_OFF;
            break;
        // 温度过高或烟雾检测到，二级报警
        case 2:
            g_stDoCtrl[DO_LEVEL2_INDEX].current_state = DO_ON;
            break;
        // 温度过高且烟雾检测到，四级报警（严重）
        case 4:
            //todo：报警记录，记录前后60s
            if(Muteflag == 1)
            {
                BEEP_ALARM_OFF();
            }
            else {
                BEEP_ALARM_ON();
            }
            /*进入刺破阀延时状态*/
            g_stValveCtl.control_status = CONTROL_DELAY;
            g_stValveCtl.ucValveType = VALVE_TYPE_LEVEL4;
            g_stValveCtl.uiValveDelayStart = tSysTime;
            /*报警和火警输出置位 */
            g_stDoCtrl[DO_ALARM_INDEX].current_state = DO_ON;
            g_stDoCtrl[DO_LEVEL2_INDEX].current_state = DO_ON;
            break;
        default:
            break;
    }
    /*故障处理*/
    if (FaultResult.all == 0) {
        g_stDoCtrl[DO_FAULT_INDEX].current_state = DO_OFF;
        LedCtrl_SetFaultLed(LED_OFF);
    }
    else {
        g_stDoCtrl[DO_FAULT_INDEX].current_state = DO_ON;
        LedCtrl_SetFaultLed(LED_ON);
    }
//    SEGGER_RTT_printf(0,"ALARM = %d,stop = %d, start = %d, mute = %d,power = %d, press = %d\r\n" \
//	,DiCtrl_GetAlarmState(),DiCtrl_GetStopToggle(),DiCtrl_GetStartToggle(),DiCtrl_GetMuteToggle(), \
//	DiCtrl_GetPowerState(),DiCtrl_GetPressState());
//	SEGGER_RTT_printf(0,"");
}
