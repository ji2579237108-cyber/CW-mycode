#include "DoCtrl.h"

// 全局DO控制结构体数组
ST_DO_CTRL g_stDoCtrl[DO_PIN_COUNT];

/**
 * @brief DO控制模块初始化
 * @note 配置信息只在一个地方维护，降低维护成本
 */
void DoCtrl_Init(void)
{
    // DO配置表 - 只需要在这里维护一份
    const struct {
        ENU_GPIO_INDEX do_enum;
    } do_config[DO_PIN_COUNT] = {
        {DO_ALARM   },      // 报警输出
        {DO_FAULT   },      // 故障输出
        {DO_SPARY   },      // 喷洒输出
        {DO_LEVEL2  },      // 火警输出
        {DO_BAT1    },      // 电池1输出
        {DO_BAT2    },      // 电池2输出
    };
    
    // 初始化所有DO的状态
    for (UINT8 i = 0; i < DO_PIN_COUNT; i++)
    {
        g_stDoCtrl[i].pin_num = do_config[i].do_enum;
        g_stDoCtrl[i].current_state = DO_OFF;
        g_stDoCtrl[i].last_state = DO_OFF;
    }
}

/**
 * @brief DO状态更新函数
 * @note 应该定期调用此函数，统一更新DO输出状态
 * @note 这是放到任务中循环调用的核心函数
 */
void DoCtrl_Update(void)
{
    ST_DO_CTRL *pDo;
    
    // 遍历所有DO，更新物理输出状态
    for (UINT8 i = 0; i < DO_PIN_COUNT; i++)
    {
        pDo = &g_stDoCtrl[i];
        
        // 检查状态是否发生变化
        if (pDo->current_state != pDo->last_state)
        {
            // 使用g_gpio_ptr数组访问GPIO，与MidDio保持一致的方式
            DrvGpioOperat(g_gpio_ptr[pDo->pin_num].uwPort, 
                         g_gpio_ptr[pDo->pin_num].uwPin, 
                         pDo->current_state);
                         
            // 更新上次状态
            pDo->last_state = pDo->current_state;
        }
    }
}

/**
 * @brief 设置指定DO的状态
 * @param do_index DO枚举索引
 * @param state DO状态 (DO_OFF 或 DO_ON)
 */
void DoCtrl_SetState(ENU_GPIO_INDEX do_index, UINT8 state)
{
    // 遍历查找对应的DO索引
    for (UINT8 i = 0; i < DO_PIN_COUNT; i++)
    {
        // 根据枚举值查找对应的DO通道
        if (g_stDoCtrl[i].pin_num == do_index)
        {
            // 只更新期望状态，实际硬件操作在DoCtrl_Update()中统一处理
            g_stDoCtrl[i].current_state = (state != DO_OFF) ? DO_ON : DO_OFF;
            return;
        }
    }
}

/**
 * @brief 获取指定DO的当前状态
 * @param do_index DO枚举索引
 * @return DO当前状态 (DO_OFF 或 DO_ON)
 */
UINT8 DoCtrl_GetState(ENU_GPIO_INDEX do_index)
{
    // 遍历查找对应的DO索引
    for (UINT8 i = 0; i < DO_PIN_COUNT; i++)
    {
        // 根据枚举值查找对应的DO通道
        if (g_stDoCtrl[i].pin_num == do_index)
        {
            return g_stDoCtrl[i].current_state;
        }
    }
    
    return DO_OFF;  // 未找到对应DO，返回关闭状态
}
