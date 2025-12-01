#ifndef LEDCTRL_H
#define LEDCTRL_H 

#include "DrvGpio.h"
#include "GpioMap.h"
#include "stdtype.h"
#include "algo.h"
#include "para.h"

// LED状态定义
#define LED_OFF   0x00U
#define LED_ON    0x01U

// LED数量
#define LED_COUNT       4

/**
 * @brief LED控制单元状态结构体
 */
typedef struct
{
    UINT8 pin_num;              // LED引脚号
    UINT8 current_state;        // 当前期望状态 (LED_OFF/LED_ON)
    UINT8 last_state;           // 上次状态
    const char* led_name;       // LED名称（用于调试）
} ST_LED_PIN;

/**
 * @brief LED控制管理结构体
 */
typedef struct
{
    ST_LED_PIN leds[LED_COUNT];    // LED控制数组
    UINT32 update_count;           // 更新计数器
} ST_LED_CTRL;

// 全局变量声明
extern ST_LED_CTRL g_stLedCtrl;

// 核心接口函数
void LedCtrl_Init(void);
void LedCtrl_Update(void);

// 通过索引控制LED（0-4）- 主要API
void LedCtrl_SetState(UINT8 led_index, UINT8 state);
UINT8 LedCtrl_GetState(UINT8 led_index);
UINT8 LedCtrl_IsStateChanged(UINT8 led_index);
const char* LedCtrl_GetLedName(UINT8 led_index);
void LedCtrl_ConditionTask(void);

// 直接GPIO控制（类似MidDio）
void LedCtrl_WriteChannel(UINT8 pin_num, UINT8 state);

// 辅助函数
void LedCtrl_Reset(void);
UINT32 LedCtrl_GetUpdateCount(void);

// ==================== 便捷宏定义（易于维护） ====================

// LED索引定义（与初始化顺序对应）
#define LED_NORMAL_INDEX   0  // 正常LED索引
#define LED_ALARM_INDEX    1  // 报警LED索引
#define LED_SPRAY_INDEX    2  // 喷洒LED索引
#define LED_FAULT_INDEX    3  // 故障LED索引

// 便捷访问宏（推荐使用）
#define LedCtrl_SetRunLed(state)      LedCtrl_SetState(LED_RUN_INDEX, state)
#define LedCtrl_SetNormalLed(state)   LedCtrl_SetState(LED_NORMAL_INDEX, state)
#define LedCtrl_SetAlarmLed(state)    LedCtrl_SetState(LED_ALARM_INDEX, state)
#define LedCtrl_SetSprayLed(state)    LedCtrl_SetState(LED_SPRAY_INDEX, state)
#define LedCtrl_SetFaultLed(state)    LedCtrl_SetState(LED_FAULT_INDEX, state)

// 便捷状态获取宏
#define LedCtrl_GetRunState()         LedCtrl_GetState(LED_RUN_INDEX)
#define LedCtrl_GetNormalState()      LedCtrl_GetState(LED_NORMAL_INDEX)
#define LedCtrl_GetAlarmState()       LedCtrl_GetState(LED_ALARM_INDEX)
#define LedCtrl_GetSprayState()       LedCtrl_GetState(LED_SPRAY_INDEX)
#define LedCtrl_GetFaultState()       LedCtrl_GetState(LED_FAULT_INDEX)

// 便捷状态变化检测宏
#define LedCtrl_IsRunChanged()        LedCtrl_IsStateChanged(LED_RUN_INDEX)
#define LedCtrl_IsNormalChanged()     LedCtrl_IsStateChanged(LED_NORMAL_INDEX)
#define LedCtrl_IsAlarmChanged()      LedCtrl_IsStateChanged(LED_ALARM_INDEX)
#define LedCtrl_IsSprayChanged()      LedCtrl_IsStateChanged(LED_SPRAY_INDEX)
#define LedCtrl_IsFaultChanged()      LedCtrl_IsStateChanged(LED_FAULT_INDEX)

// 直接GPIO操作宏（备用方式）
#define LedCtrl_WriteRun(state)       LedCtrl_WriteChannel(RUN_LED_PIN, state)
#define LedCtrl_WriteNormal(state)    LedCtrl_WriteChannel(NORMAL_LED_PIN, state)
#define LedCtrl_WriteAlarm(state)     LedCtrl_WriteChannel(ALARM_LED_PIN, state)
#define LedCtrl_WriteSpray(state)     LedCtrl_WriteChannel(SPRAY_LED_PIN, state)
#define LedCtrl_WriteFault(state)     LedCtrl_WriteChannel(FAULT_LED_PIN, state)

#endif
