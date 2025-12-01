#include "LEDCtrl.h"
#include "algo.h"
#include "para.h"
#include "DiCtrl.h"
#include "valvectl.h"


// 全局LED控制管理结构体
ST_LED_CTRL g_stLedCtrl;

/**
 * @brief LED控制模块初始化
 * @note 配置信息只在一个地方维护，降低维护成本
 */
void LedCtrl_Init(void)
{
    // LED配置表 - 只需要在这里维护一份
    const struct {
        UINT8 pin_num;
        const char* name;
    } led_config[LED_COUNT] = {
        {LED_GREEN, "NORMAL_LED"},   // 索引1
        {LED_ALARM,  "ALARM_LED"},    // 索引2
        {LED_OPEN	,  "SPRAY_LED"},    // 索引3
        {LED_YELLOW,  "FAULT_LED"}     // 索引4
    };
    
    // 初始化所有LED的状态
    for (UINT8 i = 0; i < LED_COUNT; i++)
    {
        g_stLedCtrl.leds[i].pin_num = led_config[i].pin_num;
        g_stLedCtrl.leds[i].led_name = led_config[i].name;
        g_stLedCtrl.leds[i].current_state = LED_OFF;
        g_stLedCtrl.leds[i].last_state = LED_OFF;
    }
    
    g_stLedCtrl.update_count = 0;
    
    // GPIO配置已经在DioInit()中初始化，这里不需要重复初始化
}

/**
 * @brief LED状态更新函数
 * @note 应该定期调用此函数，统一更新LED状态
 * @note 这是放到任务中循环调用的核心函数
 */
void LedCtrl_Update(void)
{
    ST_LED_PIN *pLed;
    
    // 遍历所有LED，更新物理输出状态
    for (UINT8 i = 0; i < LED_COUNT; i++)
    {
        pLed = &g_stLedCtrl.leds[i];
        
        // 检查状态是否发生变化
        if (pLed->current_state != pLed->last_state)
        {
            // 使用g_gpio_ptr数组访问GPIO，与MidDio保持一致的方式
            DrvGpioOperat(g_gpio_ptr[pLed->pin_num].uwPort, 
                         g_gpio_ptr[pLed->pin_num].uwPin, 
                         pLed->current_state);
                         
            // 更新上次状态
            pLed->last_state = pLed->current_state;
        }
    }
    
    g_stLedCtrl.update_count++;
}

/**
 * @brief 设置指定LED的状态
 * @param led_index LED索引编号 (0-4)
 * @param state LED状态 (LED_OFF 或 LED_ON)
 */
void LedCtrl_SetState(UINT8 led_index, UINT8 state)
{
    if (led_index >= LED_COUNT)
    {
        return;  // 无效索引，直接返回
    }
    
    // 只更新期望状态，实际硬件操作在LedCtrl_Update()中统一处理
    g_stLedCtrl.leds[led_index].current_state = (state != LED_OFF) ? LED_ON : LED_OFF;
}

/**
 * @brief 获取指定LED的当前状态
 * @param led_index LED索引编号 (0-4)
 * @return LED当前状态 (LED_OFF 或 LED_ON)
 */
UINT8 LedCtrl_GetState(UINT8 led_index)
{
    if (led_index >= LED_COUNT)
    {
        return LED_OFF;  // 无效索引，返回关闭状态
    }
    
    return g_stLedCtrl.leds[led_index].current_state;
}

/**
 * @brief 检查指定LED状态是否发生了变化
 * @param led_index LED索引编号 (0-4)
 * @return 1: 状态发生了变化, 0: 状态未变化
 */
UINT8 LedCtrl_IsStateChanged(UINT8 led_index)
{
    if (led_index >= LED_COUNT)
    {
        return 0;  // 无效索引，返回未变化
    }
    
    ST_LED_PIN *pLed = &g_stLedCtrl.leds[led_index];
    return (pLed->current_state != pLed->last_state) ? 1 : 0;
}

/**
 * @brief 获取指定LED的名称
 * @param led_index LED索引编号 (0-4)
 * @return LED名称字符串
 */
const char* LedCtrl_GetLedName(UINT8 led_index)
{
    if (led_index >= LED_COUNT)
    {
        return "UNKNOWN";
    }
    
    return g_stLedCtrl.leds[led_index].led_name;
}

/**
 * @brief 直接写GPIO状态（类似MidDio的DioWriteChannel）
 * @param pin_num LED引脚号
 * @param state 要设置的状态 (LED_OFF 或 LED_ON)
 * @note 此函数绕过状态管理，直接操作GPIO
 */
void LedCtrl_WriteChannel(UINT8 pin_num, UINT8 state)
{
    DrvGpioOperat(g_gpio_ptr[pin_num].uwPort, g_gpio_ptr[pin_num].uwPin, state);
}

/**
 * @brief 重置所有LED的状态
 */
void LedCtrl_Reset(void)
{
    for (UINT8 i = 0; i < LED_COUNT; i++)
    {
        g_stLedCtrl.leds[i].current_state = LED_OFF;
        g_stLedCtrl.leds[i].last_state = LED_OFF;
    }
    
    g_stLedCtrl.update_count = 0;
    
    // 立即更新硬件状态
    LedCtrl_Update();
}

/**
 * @brief 获取LED更新计数
 * @return 更新计数值
 */
UINT32 LedCtrl_GetUpdateCount(void)
{
    return g_stLedCtrl.update_count;
}

/**
 * @brief LED控制任务函数 - 根据外部条件控制LED
 * @note 这个函数应该被定期调用（如每10-50ms调用一次）
 * @note 在这里添加你的判断条件来控制LED
 */
void LedCtrl_ConditionTask(void)
{
    static uint32_t blink_timer = 0;
    static UINT8 SprayDelayFlag = 0;
    // 启动按钮或者4级
    if (SysLevel == 4 || DiCtrl_GetStartToggle() == STD_LOW) {
        LedCtrl_SetNormalLed(LED_OFF); // 正常灯灭
        if (g_stValveCtl.control_status == CONTROL_OPEN) {
            LedCtrl_SetSprayLed(LED_ON);//常亮
        }
        else {
            blink_timer++;
            if(blink_timer > 10)//1s
            {
                SprayDelayFlag = !SprayDelayFlag; // 取反SprayDelayFlag实现闪烁
                LedCtrl_SetSprayLed(SprayDelayFlag);   // 喷洒灯根据SprayDelayFlag状态控制
                blink_timer = 0;
            }
        }
    }
    else if(SysLevel == 2)
    {
        SprayDelayFlag = 0;
        blink_timer = 0;
        LedCtrl_SetNormalLed(LED_OFF); // 正常灯灭
        LedCtrl_SetSprayLed(LED_OFF);   // 喷洒灯灭
        LedCtrl_SetAlarmLed(LED_ON);  
    }
    else {
        SprayDelayFlag = 0;
        blink_timer = 0;
        LedCtrl_SetNormalLed(LED_ON); // 正常灯
        LedCtrl_SetSprayLed(LED_OFF);   // 喷洒灯灭
        LedCtrl_SetAlarmLed(LED_OFF);  
    }
    //故障在algo中处理
    // 关键：统一更新所有LED的硬件状态（必须调用！）
    LedCtrl_Update();
}

/**
 * @brief LED闪烁控制任务 - 支持条件触发的闪烁
 * @note 如果你需要LED闪烁效果，可以使用这个函数
 */
void LedCtrl_BlinkConditionTask(void)
{
//    static uint32_t blink_timer = 0;
//    static uint8_t blink_state = LED_OFF;
    
    // 声明你的外部标志位
    // extern uint8_t alarm_blink_flag;  // 报警闪烁标志位
    
//    blink_timer++;
    
    // 示例：根据外部标志位控制红灯闪烁
    // if (alarm_blink_flag == 1)
    // {
    //     // 500ms周期闪烁
    //     if (blink_timer % 250 == 0)  // 每250ms切换一次
    //     {
    //         blink_state = (blink_state == LED_OFF) ? LED_ON : LED_OFF;
    //         LedCtrl_SetAlarmLed(blink_state);
    //     }
    // }
    // else
    // {
    //     LedCtrl_SetAlarmLed(LED_OFF);  // 标志位清零时，红灯灭
    // }
    
    // 其他LED控制逻辑
    //LedCtrl_SetRunLed(LED_ON);
    
    // 统一更新LED硬件状态
    LedCtrl_Update();
}
