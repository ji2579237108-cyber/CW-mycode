#ifndef DICTRL_H
#define DICTRL_H

#include "DrvGpio.h"
#include "GpioMap.h"
#include "stdtype.h"
#include "DioCfg.h"


// 引脚状态定义
#define STD_LOW   0x00U
#define STD_HIGH  0x01U

// 数字输入引脚总数
#define DI_PIN_COUNT       6

// 消抖计数阈值（连续相同状态的次数）
#define DEBOUNCE_THRESHOLD 3

/**
 * @brief 单个数字输入引脚状态结构体
 */
typedef struct
{
    UINT8 pin_num;              // 物理引脚号
    UINT8 current_state;        // 当前读取的原始状态
    UINT8 last_state;           // 上次的稳定状态
    UINT8 stable_state;         // 消抖后的稳定状态
    UINT8 debounce_count;       // 消抖计数器
    UINT8 toggle_state;         // 触发式状态（锁存状态）
    const char* pin_name;       // 引脚名称（用于调试）
} ST_DI_PIN;

/**
 * @brief 数字输入管理结构体
 */
typedef struct
{
    ST_DI_PIN pins[DI_PIN_COUNT];  // 所有数字输入引脚
    UINT32 scan_count;             // 扫描次数计数
} ST_DI_CTRL;

// 全局变量声明
extern ST_DI_CTRL g_stDiCtrl;

// 简化的函数声明 - 只保留必要的API
void DiCtrl_Init(void);
void DiCtrl_Scan(void);

// 通过数组索引访问（0-5）- 主要API
UINT8 DiCtrl_GetPinState(UINT8 pin_index);
UINT8 DiCtrl_IsStateChanged(UINT8 pin_index);
const char* DiCtrl_GetPinName(UINT8 pin_index);
void DiCtrl_ResetToggleState(UINT8 pin_index);
UINT8 DiCtrl_GetToggleState(UINT8 pin_index);


// 直接GPIO访问（无消抖）- 类似MidDio
UINT8 DiCtrl_ReadChannel(UINT8 pin_num);

// 工具函数
void DiCtrl_Reset(void);
UINT32 DiCtrl_GetScanCount(void);

// ==================== 便捷宏定义（引脚索引） ====================

// 引脚索引宏（与初始化顺序对应）
#define DI_ALARM_INDEX     0  // 报警输入索引
#define DI_STOP_INDEX      1  // 停止按钮索引
#define DI_START_INDEX     2  // 启动按钮索引
#define DI_MUTE_INDEX      3  // 消音按钮索引
#define DI_POWER_INDEX     4  // 电源按钮索引
#define DI_PRESS_INDEX     5  // 压差输入索引

// 便捷访问宏（已消抖）
#define DiCtrl_GetAlarmState()    DiCtrl_GetPinState(DI_ALARM_INDEX)
#define DiCtrl_GetStopState()     DiCtrl_GetPinState(DI_STOP_INDEX)
#define DiCtrl_GetStartState()    DiCtrl_GetPinState(DI_START_INDEX)
#define DiCtrl_GetMuteState()     DiCtrl_GetPinState(DI_MUTE_INDEX)
#define DiCtrl_GetPowerState()    DiCtrl_GetPinState(DI_POWER_INDEX)
#define DiCtrl_GetPressState()    DiCtrl_GetPinState(DI_PRESS_INDEX)

// 便捷变化检测宏
#define DiCtrl_IsAlarmChanged()   DiCtrl_IsStateChanged(DI_ALARM_INDEX)
#define DiCtrl_IsStopChanged()    DiCtrl_IsStateChanged(DI_STOP_INDEX)
#define DiCtrl_IsStartChanged()   DiCtrl_IsStateChanged(DI_START_INDEX)
#define DiCtrl_IsMuteChanged()    DiCtrl_IsStateChanged(DI_MUTE_INDEX)
#define DiCtrl_IsPowerChanged()   DiCtrl_IsStateChanged(DI_POWER_INDEX)
#define DiCtrl_IsPressChanged()   DiCtrl_IsStateChanged(DI_PRESS_INDEX)

// 直接GPIO读取宏（无消抖）
#define DiCtrl_ReadAlarm()        DiCtrl_ReadChannel(ALARM_INPUT_PIN)
#define DiCtrl_ReadStop()         DiCtrl_ReadChannel(STOP_INPUT_PIN)
#define DiCtrl_ReadStart()        DiCtrl_ReadChannel(START_INPUT_PIN)
#define DiCtrl_ReadMute()         DiCtrl_ReadChannel(MUTE_INPUT_PIN)
#define DiCtrl_ReadPower()        DiCtrl_ReadChannel(POWER_INPUT_PIN)
#define DiCtrl_ReadPress()        DiCtrl_ReadChannel(PRESS_INPUT_PIN)

// 触发式状态便捷宏
#define DiCtrl_GetAlarmToggle()    DiCtrl_GetToggleState(DI_ALARM_INDEX)
#define DiCtrl_GetStopToggle()     DiCtrl_GetToggleState(DI_STOP_INDEX)
#define DiCtrl_GetStartToggle()    DiCtrl_GetToggleState(DI_START_INDEX)
#define DiCtrl_GetMuteToggle()     DiCtrl_GetToggleState(DI_MUTE_INDEX)
#define DiCtrl_GetPowerToggle()    DiCtrl_GetToggleState(DI_POWER_INDEX)
#define DiCtrl_GetPressToggle()    DiCtrl_GetToggleState(DI_PRESS_INDEX)

// 触发式状态重置宏
#define DiCtrl_ResetAlarmToggle()  DiCtrl_ResetToggleState(DI_ALARM_INDEX)
#define DiCtrl_ResetStopToggle()   DiCtrl_ResetToggleState(DI_STOP_INDEX)
#define DiCtrl_ResetStartToggle()  DiCtrl_ResetToggleState(DI_START_INDEX)
#define DiCtrl_ResetMuteToggle()   DiCtrl_ResetToggleState(DI_MUTE_INDEX)
#define DiCtrl_ResetPowerToggle()  DiCtrl_ResetToggleState(DI_POWER_INDEX)
#define DiCtrl_ResetPressToggle()  DiCtrl_ResetToggleState(DI_PRESS_INDEX)

#endif
