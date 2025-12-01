/*
 * @Author: jwh 
 * @Date: 2025-05-28 15:53:01
 * @LastEditors: jwh 
 * @LastEditTime: 2025-06-03 11:31:49
 * @FilePath: \CW1160-07A-HW-V100.0.1.0\Pack\Sensor\ANCHIP_BM22S2301\BM22S2301.c
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved. 
 */
#include "BM22S2301.h"
#include "stdtype.h"
#include <string.h>
#include "para.h"
#include "time.h"
#include "MidUsart.h"

/*全局宏定义*/
#define SMOKE_WINDOW_SIZE 120 // 计算斜率的窗口大小（60秒）
#define MAX_WINDOW_SIZE     180  // 烟雾数据历史记录的最大窗口大小
#define MIN_WINDOW_SIZE     10  // 动态窗口的初始大小
#define TARGET_ALARM_TIME 150 // 目标报警时间（秒）
#define DELAY_TIME 			60
#define BASE_ALARM_THRESHOLD 680 // 基准报警阈值
// 为慢速斜率逻辑定义常量
#define SLOW_SLOPE_MIN 1.0f
#define SLOW_SLOPE_MAX 5.0f

/*全局变量定义*/
STR_USART_BM22S2301_TYPE g_BM22S2301;
UINT16 g_usReceiveLength = 0;
UINT8 g_Rx_Flag = 0; //接收标志位
UINT8 g_Rx_BUFFER[64]; //最大接受64个字节，手册一条报文为41字节
UINT8 g_Tx_BUFFER[64]; //最大发送64个字节
UINT8 g_Offline_Count = 0;//离线计数器
UINT8 g_FireFlag = 0;
UINT16 slope = 0;
UINT8 Slow_count =0;
typedef enum {
    STATE_IDLE,
    STATE_WAIT_FOR_DELAY,
    STATE_ALARMING
} AlarmState;
#if 1
const STR_Alarm_Status Test_data[] = 
{
	{390,	1.743589744},
	{380,	1.789473684},
	{354,	1.920903955},
	{349,	1.948424069},
	{319,	2.131661442},
	{305,	2.229508197},
	{286,	2.377622378},
	{267,	2.546816479},
	{253,	2.687747036},
	{252,	2.698412698},
	{241,	2.821576763},
	{225,	3.022222222},
	{223,	3.049327354},
	{217,	3.133640553},
	{210,	3.238095238},
	{207,	3.285024155},
	{206,	3.300970874},
	{205,	3.317073171},
	{201,	3.383084577},
	{180,	3.777777778},
	{172,	3.953488372},
	{163,	4.171779141},
	{154,	4.415584416},
	{147,	4.62585034 },
	{145,	4.689655172},
	{136,	5          },
	{131,	5.190839695},
	{126,	5.396825397},
	{126,	5.396825397},
	{126,	5.396825397},
	{124,	5.483870968},
	{120,	5.666666667},
};
const int TEST_DATA_SIZE = sizeof(Test_data) / sizeof(STR_Alarm_Status);
#endif
/*静态函数定义*/
static UINT8 BM22S2301_Check(UINT8 *data,UINT8 len);
static inline int BM22S2301_GetStatus(void);

/*静态变量定义*/
static UINT8 s_ucPower_State = 0; //电源状态
static AlarmState g_alarm_state = STATE_IDLE;
static int g_smoke_history[MAX_WINDOW_SIZE] = {0}; // 存储历史烟雾值的环形缓冲区
static UINT16 g_history_idx = 0;
static UINT16 g_delay_countdown = 0; 	// 延时倒计时（秒）
static UINT32 g_slow_rise_timer = 0;	//慢速斜率累积计时器

void BM22S2301_Init(void)
{
    g_Rx_Flag = 0;
    memset(g_Rx_BUFFER,0,sizeof(g_Rx_BUFFER));
    memset(g_Tx_BUFFER,0,sizeof(g_Tx_BUFFER));
}

static inline int BM22S2301_GetStatus(void) {
    return DioReadChannel(SMOKE_STATUS);
}
/**
 * @brief 生成BM22S2301数据包的CRC校验码
 * @param data 数据包指针
 * @param len 数据包长度（不包括CRC字节）
 * @return 生成的CRC校验码
 */
static UINT8 BM22S2301_GenerateCRC(UINT8 *data, UINT8 len)
{
    UINT32 checksum = 0;
    
    // 计算数据包前len个字节的和
    for (UINT8 i = 0; i < len; i++)
    {
        checksum += data[i];
    }
    
    // 取反加1得到CRC校验码
    return (~(UINT8)(checksum & 0xFF)) + 1;
}
static UINT8 BM22S2301_Check(UINT8 *tempBuffer,UINT8 len)
{
    UINT8 ret = 0; 
    UINT32 checksum = 0;
    UINT8 CRC1 = 0; //校验码
    if(len == BM22S2301_LENGTH)
    {
        if (tempBuffer[0] == 0xAA && tempBuffer[1] == 0x29 && tempBuffer[2] == 0x11)
        {
            for (UINT8 i = 0; i < BM22S2301_LENGTH-1; i++)
            {
                checksum += tempBuffer[i];
            }
            CRC1 = (~(UINT8)(checksum & 0xFF)) + 1;
            if (CRC1 == tempBuffer[BM22S2301_LENGTH-1])
            {
                ret = 1; //校验成功
            }
            else
            {
                ret = 3; //校验失败
            } 
        }
        else
        {
            ret = 3; //校验失败
        }
    }
    else
    {
        ret = 2; //长度不对
    }
    return ret;
}
void BM22S2301_Handle(void)
{
    static UINT8 Init_Flag = 0;
    static UINT32 delay_timer = 0;
	UINT8 tempBuffer[BM22S2301_LENGTH] = {0};
	UINT8 ret = 0;
	memcpy(tempBuffer,g_Rx_BUFFER,BM22S2301_LENGTH);
    //10s后标定完成
    if(Init_Flag == 1 && tSysTime > delay_timer + 10)
    {
//		SEGGER_RTT_printf(0,"parawrite1\r\n");
        pSysPara->ucSmokeInit = 1;
        ParaWrite();
        Init_Flag = 0;
		g_Rx_Flag = 0;		//改变收发状态
        return ;
    }
    //延时期间不做处理
    else if (Init_Flag == 1 && tSysTime <= delay_timer + 10) 
    {
		g_Rx_Flag = 0;		//改变收发状态
//		SEGGER_RTT_printf(0,"parawrite2\r\n");
        return ;
    }
    //Init_Flag = 0的情况，不做处理
    else {
//		SEGGER_RTT_printf(0,"parawrite3\r\n");
        //no action
    }
    if (pSysPara->ucSmokeInit != 1) {
        g_Tx_BUFFER[0] = 0xAB; 
        g_Tx_BUFFER[1] = 0x00;
        g_Tx_BUFFER[2] = 0x00;
        g_Tx_BUFFER[3] = BM22S2301_GenerateCRC(g_Tx_BUFFER, 3);
		smoke_USART2_send((char *)g_Tx_BUFFER,4);
		
        Init_Flag = 1;
        delay_timer = tSysTime;
		g_Rx_Flag = 0;		//改变收发状态
//		SEGGER_RTT_printf(0,"\r\ncrc = %d，delay_timer = %d\r\n",g_Tx_BUFFER[3],delay_timer);
		return ;
    }
    if(g_Rx_Flag == 1)
    {
        g_Offline_Count = 0; //清零离线计数器
        g_Rx_Flag = 0;
        /*检查数据合法性*/
		ret = BM22S2301_Check(tempBuffer, g_usReceiveLength);
        if (ret == 1)
        {
            //处理接收到的数据
			/*状态位单独处理*/
            g_BM22S2301.BM22S2301_STATUS.status = tempBuffer[5];
            g_BM22S2301.BM22S2301_GetStatus = BM22S2301_GetStatus();
            /*数值使用内存地址复制*/
            UINT16 *ptr = &g_BM22S2301.BM22S2301_ValueA;
            memcpy(ptr,&tempBuffer[16], 12);
//			SEGGER_RTT_printf(0, "A_SAMPLE = %d,A_ZERO = %d,A_alarmlimit = %d\r\n" \
//				,g_BM22S2301.BM22S2301_ValueA,g_BM22S2301.BM22S2301_ZeroA,g_BM22S2301.BM22S2301_AlarmA);
//			SEGGER_RTT_printf(0, "b_SAMPLE = %d,b_ZERO = %d,b_alarmlimit = %d\r\n" \
//				,g_BM22S2301.BM22S2301_ValueB,g_BM22S2301.BM22S2301_ZeroB,g_BM22S2301.BM22S2301_AlarmB);
//			SEGGER_RTT_printf(0, "ALARM = %d, signal = %d\r\n",g_BM22S2301.BM22S2301_STATUS.Bits.BM22S2301_HIGH,g_BM22S2301.BM22S2301_GetStatus);
            memset(g_Rx_BUFFER,0,sizeof(g_Rx_BUFFER));  //清空接收缓冲区
			return;
        }
		else if(ret == 2)
		{
            memset(g_Rx_BUFFER,0,sizeof(g_Rx_BUFFER));  //清空接收缓冲区
			SEGGER_RTT_printf(0, "BM22S2301_Check Length Error\r\n");
			return;
		}
        else if(ret == 3)
        {
            memset(g_Rx_BUFFER,0,sizeof(g_Rx_BUFFER));  //清空接收缓冲区
            SEGGER_RTT_printf(0, "BM22S2301_Check CRC Error\r\n");
			return;
        }
        
    }
}

/*获取报警状态*/
void BM22S2301_GetValue(void* pDat)
{
    /*保证引脚和数据都是产生的报警，避免抖动造成误报*/
    if (g_BM22S2301.BM22S2301_GetStatus == 1 && g_BM22S2301.BM22S2301_STATUS.Bits.BM22S2301_FIRE == 1) 
//    if (g_BM22S2301.BM22S2301_STATUS.Bits.BM22S2301_FIRE == 1)
	{
        // 将指针pDat指向的内存单元赋值为1
        *((UINT16 *)pDat) = 1;
    }
    else
    {
        // 将指针pDat指向的内存单元赋值为0
        *((UINT16 *)pDat) = 0;
    }
    
}

/*获取自检结果*/
BOOL BM22S2301_GetSelfCheck(void)
{
    /*离线计数器自增，若大于三次则没有进过处理函数，即烟雾失效*/
    if (++g_Offline_Count <= 30)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*开启传感器*/
void BM22S2301_Open(void)
{
    s_ucPower_State = 1; //设置电源开启
}

/*关闭传感器*/
void BM22S2301_Close(void)
{
    s_ucPower_State = 0; //设置电源关闭
}

/*获得传感器电源是否开启*/
BOOL BM22S2301_SensorIsPower(void)
{
    return s_ucPower_State;
}
UINT8 Get_Rx_Flag(void)
{
    return g_Rx_Flag;
}
void Set_Rx_Flag(UINT8 flag)
{
    g_Rx_Flag = flag;
}

void Usart_Timer_Handle()
{
	static UINT8 getcount = 0;
	getcount++ ;
	
}
/**
 * @brief  通过查表和线性插值，根据斜率估算报警时间
 * @param  k_current 当前斜率
 * @return 估算的报警时间 (秒)
 */
UINT16 estimate_time_by_slope(float k_current) {
    // 寻找斜率所在区间
    for (int i = 0; i < TEST_DATA_SIZE - 1; ++i) {
        if (k_current >= Test_data[i].k && k_current <= Test_data[i+1].k) {
            // 线性插值: T = T1 + (k - k1) * (T2 - T1) / (k2 - k1)
            float k1 = Test_data[i].k;
            float k2 = Test_data[i+1].k;
            UINT16 t1 = Test_data[i].alarm_time ;
            UINT16 t2 = Test_data[i+1].alarm_time ;
            UINT16 estimated_time = t1 + (k_current - k1) * (t2 - t1) / (k2 - k1);  
            return estimated_time;
        }
    }
    
    // 如果斜率超出范围，返回边界值
    if (k_current <= Test_data[0].k) {
        return Test_data[0].alarm_time - 20;
    } else {
        return Test_data[TEST_DATA_SIZE - 1].alarm_time - 20;
    }
}
// <<<<<<<<<<<<<<<<< 新增的最小二乘法斜率计算函数 >>>>>>>>>>>>>>>>>
float calculate_slope_least_squares(int values[], int n)
{
    long long sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2 = 0;
    if (n < 2) return 0.0f;
    for (int i = 0; i < n; i++) {
        sum_x += i;
        sum_y += values[i];
        sum_xy += (long long)i * values[i];
        sum_x2 += (long long)i * i;
    }
    double denominator = (double)n * sum_x2 - (double)sum_x * sum_x;
    if (denominator == 0) return 0.0f;
    return (float)(((double)n * sum_xy - (double)sum_x * sum_y) / denominator);
}
/**
 * @brief 触发真正的物理报警（例如蜂鸣器、LED）
 */
void trigger_final_alarm() {
    SEGGER_RTT_printf(0,"!!! ALARM !!! BEEP! BEEP! BEEP!\n");
	g_FireFlag = g_FireFlag | (1 << 6);
}
void cancel_final_alarm() {
	SEGGER_RTT_printf(0,"cancel Alarm\n");
	g_FireFlag &= ~(1 << 6);
}

/**
 * @brief  报警延时主处理函数
 * @param  alarm_flag  一个初步的报警标志（例如，烟雾值超过基础阈值）
 * @param  smoke_value 当前的烟雾传感器值
 *
 * @note   这个函数应该被一个1秒定时器周期性调用。
 */
void Alarm_delay_process(STR_USART_BM22S2301_TYPE *SmokeType) {
    if (SmokeType == NULL) {
    return;
    }
	int smoke_value = SmokeType->BM22S2301_ValueB;
	UINT8 alarm_flag = SmokeType->BM22S2301_STATUS.Bits.BM22S2301_FIRE;
	int smoke_base = SmokeType->BM22S2301_ZeroB;
	float current_slope1 = 0;

    // 在文件顶部添加静态变量
    static UINT8 g_valid_data_count = 0;  // 跟踪有效数据数量
	static UINT8 SlowAlarm_flag = 0;
    // ---- 步骤1: 更新历史数据 ----
    g_smoke_history[g_history_idx] = smoke_value;
    g_history_idx = (g_history_idx + 1) % MAX_WINDOW_SIZE;
    if (g_valid_data_count < SMOKE_WINDOW_SIZE) {
        g_valid_data_count++;
        return; // 如果数据量不足，则不进行后续处理
    }
	// 为了后续120秒斜率计算做准备，定义数组
	int ordered_history[SMOKE_WINDOW_SIZE];

    // ---- 步骤2: 状态机处理 ----
    switch (g_alarm_state) {
        case STATE_IDLE:
            if (alarm_flag) {
                // 已经有报警条件满足，如果g_slow_rise_timer小于120延时60s
				if(g_slow_rise_timer < 120)
				{
					g_delay_countdown = 40;
					g_alarm_state = STATE_WAIT_FOR_DELAY;
				}
				else
				{
					SEGGER_RTT_printf(0,"Alarm flag triggered after 120s. Direct alarm.\n");
					g_alarm_state = STATE_ALARMING;
					trigger_final_alarm();
				}
            }
			else {
				// 检查烟雾值是否高于基准40
				SEGGER_RTT_printf(0,"smoke %d - base %d .\n", smoke_value , smoke_base);
                if (smoke_value - smoke_base >= 40) {
                    g_slow_rise_timer++;
                    SEGGER_RTT_printf(0,"Above threshold count: %d\n", g_slow_rise_timer);
                    
                    // 累计120秒后计算前120秒的斜率
                    if (g_slow_rise_timer >= 120) {
#if 1
                        // 计算前120秒的斜率
                        int start_idx = (g_history_idx + MAX_WINDOW_SIZE - 120) % MAX_WINDOW_SIZE;
                        for(int i = 0; i < SMOKE_WINDOW_SIZE; ++i) {
                            ordered_history[i] = g_smoke_history[(start_idx + i) % MAX_WINDOW_SIZE];
                        }
                        current_slope1 = calculate_slope_least_squares(ordered_history, SMOKE_WINDOW_SIZE);
                        if (current_slope1 <= 0) current_slope1 = 0;
                        
                        SEGGER_RTT_printf(0,"120s slope calculated: %.2f\n", current_slope1);
                        
                        // 如果斜率小于5就直接报警，为慢速
                        if (current_slope1 < SLOW_SLOPE_MAX) {
                            SEGGER_RTT_printf(0,"Slope < 5, triggering alarm\n");
                            g_alarm_state = STATE_ALARMING;
                            SlowAlarm_flag = 10;
                            trigger_final_alarm();
                        }
                        // 斜率≥5时继续累计，如果超过150秒直接报警
                        else {
                            SEGGER_RTT_printf(0,"Slope >= 5, continuing count: %d\n", g_slow_rise_timer);
                            if (g_slow_rise_timer >= TARGET_ALARM_TIME) {
                                SEGGER_RTT_printf(0,"Over 150s without alarm_flag, triggering alarm\n");

                                g_alarm_state = STATE_ALARMING;
                                SlowAlarm_flag = 10;
                                trigger_final_alarm();
                            }
                        }
                    }
                } else {
                    // 不符合增长状态，重置计数器
                    if (g_slow_rise_timer > 0) {
                        SEGGER_RTT_printf(0,"Below threshold. Resetting timer.\n");
                        g_slow_rise_timer = 0;
                    }
#else
				
						g_alarm_state = STATE_ALARMING;
						SlowAlarm_flag = 10;
						trigger_final_alarm();
						}
#endif
                }
            }
            break;

        case STATE_WAIT_FOR_DELAY:
			SEGGER_RTT_printf(0,"Delay countdown: %d\n", g_delay_countdown);
			g_delay_countdown--;
            if(g_delay_countdown == 0)
			{
				SEGGER_RTT_printf(0,"Delay finished. Triggering alarm.\n");
				g_alarm_state = STATE_ALARMING;
				trigger_final_alarm();
			}
            break;

        case STATE_ALARMING:
			/*缓慢导致报警*/
			if(SlowAlarm_flag)
			{
                /*报警时与base差值小于100，说明base已变动，取消报警 */
				if((smoke_base > smoke_value ? smoke_base - smoke_value : smoke_value - smoke_base) < 100)
				{
					SlowAlarm_flag = SlowAlarm_flag <= 1 ? 0 : --SlowAlarm_flag;
				}
				if(SlowAlarm_flag == 0)
				{
					g_alarm_state = STATE_IDLE;
					SlowAlarm_flag = 0;
					g_slow_rise_timer = 0;
					cancel_final_alarm();
				}
			}
			else
			{
				// 保持报警状态，直到有手动复位或烟雾消失等逻辑
				if (!alarm_flag) {
					g_alarm_state = STATE_IDLE;
					g_delay_countdown = 0;
					g_slow_rise_timer = 0;
					cancel_final_alarm();
				}
			}
            break;
    }
	slope = (UINT16)current_slope1 | (UINT16)(g_slow_rise_timer << 8); // 只使用120秒的斜率
	Slow_count = SlowAlarm_flag;
	g_FireFlag = (g_FireFlag & 0xF0) | (g_alarm_state + 1);
	if(alarm_flag) //1
	{
		g_FireFlag = g_FireFlag | (1 << 7);
	}
	else //0
	{
		g_FireFlag &= ~(1 << 7);
	}
	SEGGER_RTT_printf(0,"g_FireFlag = %d\r\n",g_FireFlag);
	
}
void test_somke(void)
{
	static UINT8 count1 = 0;
	if(count1 < 5)//前5s不检测
	{
		count1++;
		return;
	}
#if 0
	static UINT16 testCount = 0;
	static STR_USART_BM22S2301_TYPE c;
	UINT16 alarm,smoke = 0;
	if(testCount > 350)
	{
		smoke = 20000;
	}
	else
	{
		smoke = 20000+4*testCount;
	}
	c.BM22S2301_ValueB = smoke;
	testCount++;
	if(testCount > 170 && testCount < 350)
	{
		alarm = 1;
	}
	else{
		alarm = 0;
	}
	c.BM22S2301_STATUS.Bits.BM22S2301_FIRE = alarm;
	c.BM22S2301_ZeroB = 20000;
	Alarm_delay_process(&c);
	estimate_time_by_slope(4.0);
#else
	Alarm_delay_process(&g_BM22S2301);
#endif
}

