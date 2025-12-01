#include "valvectl.h"
#include "MidCan.h"
#include "para.h"
#include "MidDio.h"
#include "time.h"
#include "DoCtrl.h"
#include "DiCtrl.h"
#include "algo.h"

STR_TYPE_VALVE g_stValveCtl;
// 延时相关变量
static UINT32 s_delay_duration = 0;     // 延时持续时间

/**
 * @brief 初始化阀门控制
 */
void ValveCtrlInit(void)
{
    g_stValveCtl.control_status = CONTROL_NORMAL;
    g_stValveCtl.ucValveType = VALVE_TYPE_NONE;
    g_stValveCtl.uiValveTime = 0x00000000;
    g_stValveCtl.uiValveDelayStart = 0x00000000;
    // 初始化延时相关变量
    s_delay_duration = pSysPara->ucDelay;
}

/**
 * @brief 设置延时参数
 * @param duration 延时时间（毫秒）
 */
void ValveCtrl_SetDelay(UINT32 duration)
{
    s_delay_duration = duration;
}


/**
 * @brief 复位阀门状态
 */
void ValveCtrl_Reset(void)
{
	g_stValveCtl.control_status = CONTROL_NORMAL;
    g_stValveCtl.ucValveType = VALVE_TYPE_NONE;
    g_stValveCtl.uiValveTime = 0x00000000;
    g_stValveCtl.uiValveDelayStart = 0x00000000;
    // 初始化延时相关变量
    s_delay_duration = pSysPara->ucDelay;
}

/**
 * @brief 获取当前阀门状态
 * @return 当前阀门状态
 */
ENU_VAL_CONTROL ValveCtrl_GetStatus(void)
{
    return g_stValveCtl.control_status;
}

void ValveCtrl_SetValveType(ENU_VAL_TYPE type) {
    //can报文启阀，直接开启
    if(type == VALVE_TYPE_CAN)
    {
        DioWriteChannel(HSD_IN, TRUE);
        FaultResult.bits.Fault_HSD = 1 ;// HSD故障标志位
        pSysPara->ucValveFault = FaultResult.bits.Fault_HSD;//写入参数
        ParaWrite();
    }
    else {
        return;//非can启动调用直接退出
    }
}

/**
 * @brief 阀门控制主处理函数
 * @note 需要在主循环中定期调用
 * @note 每个状态都需要对两个按钮进行判断，不然按钮状态不处理会导致错误
 */
void ValveCtrl_Process(void)
{  
    // 根据当前状态执行相应的操作
    switch (g_stValveCtl.control_status)
    {
        case CONTROL_NORMAL:
            // 正常状态下关闭阀门
            DioWriteChannel(HSD_IN, FALSE);
            /*停止按钮按下*/
            if (DiCtrl_GetStopToggle() == STD_LOW)
            {
                // 已经是正常状态，只需重置按钮状态
                DiCtrl_ResetStopToggle();
            }
            // 检测启动按钮
			if (DiCtrl_GetStartToggle() == STD_LOW) {
				g_stValveCtl.control_status = CONTROL_DELAY;
				g_stValveCtl.uiValveDelayStart = tSysTime;
                g_stValveCtl.ucValveType = VALVE_TYPE_OPEN;
				DiCtrl_ResetStartToggle();
			}

            break;
            
        case CONTROL_DELAY:
            // 检查延时是否结束
            if ((tSysTime - g_stValveCtl.uiValveDelayStart) >= s_delay_duration)
            {
                g_stValveCtl.control_status = CONTROL_OPEN;
                g_stValveCtl.uiValveTime = tSysTime;
                FaultResult.bits.Fault_HSD = 1 ;// HSD故障标志位
                pSysPara->ucValveFault = FaultResult.bits.Fault_HSD;//写入参数
                ParaWrite();
                //todo：喷洒启动记录，记录前后30s
            }
            /*延时时启动按钮按下，基本不做处理*/
            if (DiCtrl_GetStartToggle() == STD_LOW)
            {
                DiCtrl_ResetStartToggle();
            }
            /*延时时停止按钮按下*/
			if (DiCtrl_GetStopToggle() == STD_LOW) {
				g_stValveCtl.control_status = CONTROL_NORMAL;
                g_stValveCtl.ucValveType = VALVE_TYPE_NONE;
				DiCtrl_ResetStopToggle();
			}
            break;            
        case CONTROL_OPEN:
            // 开启状态下打开阀门（假设阀门连接到HSD_IN引脚）
            DioWriteChannel(HSD_IN, TRUE);
            g_stDoCtrl[DO_SPARY_INDEX].current_state = DO_ON; //喷洒输出，一直输出，无消除该故障的手段
            /*启动按钮按下*/
            if (DiCtrl_GetStartToggle() == STD_LOW)
            {
                DiCtrl_ResetStartToggle();
            }
            /*停止按钮按下*/
            if (DiCtrl_GetStopToggle() == STD_LOW)
            {
                g_stValveCtl.control_status = CONTROL_NORMAL;
                g_stValveCtl.uiValveTime = tSysTime - g_stValveCtl.uiValveTime;
                DiCtrl_ResetStopToggle();
            }
            break;
    }
}




