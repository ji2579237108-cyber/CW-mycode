#ifndef _DOCTRL_H_
#define _DOCTRL_H_

#include "MidDio.h"
#include "stdtype.h"
#include "DioCfg.h"

// DO输出通道数量
#define DO_PIN_COUNT 6

// 引脚索引宏（与初始化顺序对应）
#define DO_ALARM_INDEX      0  //报警输出
#define DO_FAULT_INDEX      1  //故障输出
#define DO_SPARY_INDEX      2  //
#define DO_LEVEL2_INDEX     3  // 
#define DO_BAT1_INDEX       4  // 
#define DO_BAT2_INDEX       5  // 

// DO状态定义
#define DO_OFF   0x00U
#define DO_ON    0x01U

/**
 * @brief DO输出控制结构体
 */
typedef struct
{
    UINT8 pin_num;              // DO引脚号
    UINT8 current_state;        // 当前期望状态 (DO_OFF/DO_ON)
    UINT8 last_state;           // 上次状态
} ST_DO_CTRL;

// 全局DO控制结构体数组
extern ST_DO_CTRL g_stDoCtrl[DO_PIN_COUNT];

// 函数声明
void DoCtrl_Init(void);
void DoCtrl_Update(void);
void DoCtrl_SetState(ENU_GPIO_INDEX do_index, UINT8 state);
UINT8 DoCtrl_GetState(ENU_GPIO_INDEX do_index);

// BAT控制函数
#define DoCtrl_SetBat1State(state)    DoCtrl_SetState(DO_BAT1, state)
#define DoCtrl_SetBat2State(state)    DoCtrl_SetState(DO_BAT2, state)

// BAT状态获取函数
#define DoCtrl_GetBat1State()         DoCtrl_GetState(DO_BAT1)
#define DoCtrl_GetBat2State()         DoCtrl_GetState(DO_BAT2)

// BAT快速控制函数
#define DoCtrl_EnableBat1()           DoCtrl_SetState(DO_BAT1, DO_ON)
#define DoCtrl_DisableBat1()          DoCtrl_SetState(DO_BAT1, DO_OFF)
#define DoCtrl_EnableBat2()           DoCtrl_SetState(DO_BAT2, DO_ON)
#define DoCtrl_DisableBat2()          DoCtrl_SetState(DO_BAT2, DO_OFF)

// BAT切换控制函数
#define DoCtrl_BatOpen()            do { DoCtrl_EnableBat1(); DoCtrl_DisableBat2(); } while(0)
#define DoCtrl_BatClose()           do { DoCtrl_DisableBat1(); DoCtrl_EnableBat2(); } while(0)
#define DoCtrl_BatReset()           do { DoCtrl_DisableBat1(); DoCtrl_DisableBat2(); } while(0)

#endif
