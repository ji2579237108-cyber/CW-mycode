#ifndef _BM22S2301_H
#define _BM22S2301_H

#include "stdtype.h"
#include "MidDio.h"
#define BM22S2301_LENGTH 41

//#pragma pack(1)
typedef struct BM22S2301
{
    union 
    {
        UINT8 status;
        struct {
            UINT8 BM22S2301_Calibration :1; //bit0校准状态
            UINT8 BM22S2301_ALARM       :1; //bit1预警状态
            UINT8 BM22S2301_FIRE        :1; //bit2报警状态
            UINT8 BM22S2301_ERROR       :1; //bit3故障状态
            UINT8 BM22S2301_INTERFERE   :1; //bit4干扰状态
            UINT8 BM22S2301_RESERVED1   :1; //bit5保留
            UINT8 BM22S2301_HIGH        :1; //bit6高报状态
            UINT8 BM22S2301_RESERVED2   :1; //bit7保留
        }Bits;
    }BM22S2301_STATUS;
    UINT8  BM22S2301_GetStatus; //传感器报警状态
    UINT16 BM22S2301_ValueA;    //传感器A值
    UINT16 BM22S2301_ValueB;    //传感器B值
    UINT16 BM22S2301_ZeroA;     //零点值A
    UINT16 BM22S2301_ZeroB;     //零点值B
    UINT16 BM22S2301_AlarmA;    //报警阈值A
    UINT16 BM22S2301_AlarmB;    //报警阈值B
}__attribute__((packed)) STR_USART_BM22S2301_TYPE;

typedef struct 
{
	UINT16 alarm_time;
	float k;
}STR_Alarm_Status;
extern UINT16 g_usReceiveLength;    //接收长度
extern UINT8 g_Rx_BUFFER[64]; //接收缓冲区
extern UINT8 g_Tx_BUFFER[64]; //发送缓冲区
extern UINT8 g_Rx_Flag; //接收标志位
extern UINT8 g_Offline_Count;//离线计数器
extern STR_USART_BM22S2301_TYPE g_BM22S2301;//全局结构体变量
extern UINT8 g_FireFlag;
extern UINT16 slope;
extern UINT8 Slow_count;

extern void BM22S2301_Init(void);
extern void BM22S2301_Handle(void);
extern UINT8 Get_Rx_Flag(void);
extern void Set_Rx_Flag(UINT8 flag);
extern void BM22S2301_GetValue(void* pDat);
extern BOOL BM22S2301_GetSelfCheck(void);
extern void BM22S2301_Open(void);
extern void BM22S2301_Close(void);
extern BOOL BM22S2301_SensorIsPower(void);
void test_somke(void);

#endif
