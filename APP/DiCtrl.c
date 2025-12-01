#include "DiCtrl.h"

// 全局数字输入控制结构体
ST_DI_CTRL g_stDiCtrl;

/**
 * @brief 数字输入控制初始化
 * @note 引脚配置只在这一个地方维护，简化维护工作
 */
void DiCtrl_Init(void)
{
    // 引脚配置表 - 只需要在这里维护一次
    const struct {
        UINT8 pin_num;
        const char* name;
    } pin_config[DI_PIN_COUNT] = {
        {DI_ALARM, "ALARM_INPUT"},   // 索引0
        {DI_STOP,  "STOP_INPUT"},    // 索引1
        {DI_START, "START_INPUT"},   // 索引2
        {DI_MUTE,  "MUTE_INPUT"},    // 索引3
        {DI_POWER, "POWER_INPUT"},   // 索引4
        {DI_PRESS, "PRESS_INPUT"}    // 索引5
    };
    
    // 初始化所有引脚状态
    for (UINT8 i = 0; i < DI_PIN_COUNT; i++)
    {
        g_stDiCtrl.pins[i].pin_num = pin_config[i].pin_num;
        g_stDiCtrl.pins[i].pin_name = pin_config[i].name;
        g_stDiCtrl.pins[i].current_state = STD_HIGH;
        g_stDiCtrl.pins[i].last_state = STD_HIGH;
        g_stDiCtrl.pins[i].stable_state = STD_HIGH;
        g_stDiCtrl.pins[i].debounce_count = 0;
        g_stDiCtrl.pins[i].toggle_state = STD_HIGH;  // 初始化触发式状态

    }
    
    g_stDiCtrl.scan_count = 0;
    
    // GPIO引脚已经在DioInit()中初始化，这里不需要重复初始化
}

/**
 * @brief 数字输入扫描函数
 * @note 应该定期调用此函数进行状态更新
 */
void DiCtrl_Scan(void)
{
    UINT8 raw_state;
    ST_DI_PIN *pPin;
    
    // 扫描所有引脚
    for (UINT8 i = 0; i < DI_PIN_COUNT; i++)
    {
        pPin = &g_stDiCtrl.pins[i];
        
        // 使用g_gpio_ptr读取引脚原始状态（与MidDio相同方式）
        raw_state = (UINT8)DrvGpioGetInputBit(g_gpio_ptr[pPin->pin_num].uwPort, 
                                              g_gpio_ptr[pPin->pin_num].uwPin);
        pPin->current_state = raw_state;
        
        // 消抖处理
        if (raw_state == pPin->stable_state)
        {
            // 状态相同，重置计数器
            pPin->debounce_count = 0;
        }
        else
        {
            // 状态不同，增加计数器
            pPin->debounce_count++;
            
            // 达到消抖阈值，更新稳定状态
            if (pPin->debounce_count >= DEBOUNCE_THRESHOLD)
            {
                pPin->last_state = pPin->stable_state;  // 保存上一个稳定状态
                pPin->stable_state = raw_state;         // 更新为新的稳定状态
                // 触发式处理：检测到上升沿时切换状态
                if (pPin->last_state == STD_HIGH && pPin->stable_state == STD_LOW)
                {
                    pPin->toggle_state = (pPin->toggle_state == STD_HIGH) ? STD_LOW : STD_HIGH;
                }
                pPin->debounce_count = 0;               // 重置计数器
            }

        }
    }
    
    g_stDiCtrl.scan_count++;
}

/**
 * @brief 获取指定引脚的稳定状态
 * @param pin_index 引脚数组索引 (0-5)
 * @return 引脚稳定状态 (STD_LOW 或 STD_HIGH)
 */
UINT8 DiCtrl_GetPinState(UINT8 pin_index)
{
    if (pin_index >= DI_PIN_COUNT)
    {
        return STD_HIGH;  // 无效索引返回低电平
    }
    
    return g_stDiCtrl.pins[pin_index].stable_state;
}

/**
 * @brief 检查指定引脚状态是否发生了变化
 * @param pin_index 引脚数组索引 (0-5)
 * @return 1: 状态发生变化, 0: 状态未变化
 */
UINT8 DiCtrl_IsStateChanged(UINT8 pin_index)
{
    if (pin_index >= DI_PIN_COUNT)
    {
        return 0;  // 无效索引返回未变化
    }
    
    ST_DI_PIN *pPin = &g_stDiCtrl.pins[pin_index];
    return (pPin->stable_state != pPin->last_state) ? 1 : 0;
}

/**
 * @brief 获取指定引脚的名称
 * @param pin_index 引脚数组索引 (0-5)
 * @return 引脚名称字符串
 */
const char* DiCtrl_GetPinName(UINT8 pin_index)
{
    if (pin_index >= DI_PIN_COUNT)
    {
        return "UNKNOWN";
    }
    
    return g_stDiCtrl.pins[pin_index].pin_name;
}

/**
 * @brief 直接读取GPIO状态（类似MidDio的DioReadChannel）
 * @param pin_num 物理引脚号
 * @return 引脚当前状态 (STD_LOW 或 STD_HIGH)
 * @note 此函数不经过消抖处理，直接读取GPIO状态
 */
UINT8 DiCtrl_ReadChannel(UINT8 pin_num)
{
    return (UINT8)DrvGpioGetInputBit(g_gpio_ptr[pin_num].uwPort, g_gpio_ptr[pin_num].uwPin);
}

/**
 * @brief 获取指定引脚的触发式状态
 * @param pin_index 引脚数组索引 (0-5)
 * @return 触发式状态 (STD_LOW 或 STD_HIGH)
 */
UINT8 DiCtrl_GetToggleState(UINT8 pin_index)
{
    if (pin_index >= DI_PIN_COUNT)
    {
        return STD_HIGH;  // 无效索引返回低电平
    }
    
    return g_stDiCtrl.pins[pin_index].toggle_state;
}

/**
 * @brief 重置指定引脚的触发式状态
 * @param pin_index 引脚数组索引 (0-5)
 */
void DiCtrl_ResetToggleState(UINT8 pin_index)
{
    if (pin_index < DI_PIN_COUNT)
    {
        g_stDiCtrl.pins[pin_index].toggle_state = STD_HIGH;
    }
}

/**
 * @brief 设置指定引脚的触发式状态
 * @param pin_index 引脚数组索引 (0-5)
 * @param state 要设置的状态 (STD_LOW 或 STD_HIGH)
 */
void DiCtrl_SetToggleState(UINT8 pin_index, UINT8 state)
{
    if (pin_index < DI_PIN_COUNT)
    {
        g_stDiCtrl.pins[pin_index].toggle_state = state;
    }
}

/**
 * @brief 重置所有引脚的状态
 */
void DiCtrl_Reset(void)
{
    for (UINT8 i = 0; i < DI_PIN_COUNT; i++)
    {
        g_stDiCtrl.pins[i].current_state = STD_HIGH;
        g_stDiCtrl.pins[i].last_state = STD_HIGH;
        g_stDiCtrl.pins[i].stable_state = STD_HIGH;
        g_stDiCtrl.pins[i].debounce_count = 0;
        g_stDiCtrl.pins[i].toggle_state = STD_HIGH;  // 重置触发式状态
    }
    
    g_stDiCtrl.scan_count = 0;
}

/**
 * @brief 获取总扫描次数
 * @return 扫描次数
 */
UINT32 DiCtrl_GetScanCount(void)
{
    return g_stDiCtrl.scan_count;
}
