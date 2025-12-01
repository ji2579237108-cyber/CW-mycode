/********************************************************************
版权（C），烟台创为新能源科技有限公司
文 件 名：message.c
作    者：chungway
版    本：1.0
生成日期：
概    述：消息服务模块 
*********************************************************************/
#include "message.h"

#define MESSAGETABLE_MAX_SIZE 30
#ifndef MESSAGETABLE_SIZE 
#define MESSAGETABLE_SIZE MESSAGETABLE_MAX_SIZE
#endif
#if MESSAGETABLE_SIZE > MESSAGETABLE_MAX_SIZE
#error "MESSAGETABLE_SIZE"
#endif

typedef struct
{
    P_MESSAGE_PROC pMessageProc; //消息处理指针
    UINT16 usIntervalTime; //消息处理间隔时间
    UINT8 ucRunFlag; //1-Run, 0-Not run
} STR_MESSAGE;

volatile static UINT8  ucTableSize = 0;//消息表当前大小
volatile static UINT16 gusMsTicks = 100; //消息毫秒滴答时间
volatile static UINT32 uwTicksCounter = 0;   //消息滴答计数器
static const UINT32 cuwIntervalLcm = 362880000; 
static STR_MESSAGE stMessageTable[MESSAGETABLE_SIZE];

/*******************************************************************
函数名称：MessageConfigTicks
函数原型：BOOL MessageConfigTicks(UINT16 usMsTicks)
功能描述：消息模块设置毫秒滴答时间
输入1   ：usMsTicks：毫秒滴答时间
返回值  ：无
补充信息：放于MessageRegister前进行设置
*********************************************************************/ 
 BOOL MessageConfigTicks(UINT16 usMsTicks)
{
    if (usMsTicks == 0 || usMsTicks % 2 != 0) 
    {
        return FALSE;    
    }
    else 
    {
        gusMsTicks = usMsTicks;
        (void)MessageInit();
        return TRUE;
    }         
}

/*******************************************************************
函数名称：MessageInit
函数原型：BOOL MessageInit(void)
功能描述：消息模块初始化
输入1   ：无
返回值  ：无
补充信息：无顺序要求
*********************************************************************/ 
BOOL MessageInit(void)
{
    UINT8 i;
    
    ucTableSize = 0;
    for (i = 0; i < MESSAGETABLE_SIZE; i++) 
    {
        stMessageTable[i].pMessageProc = NULL;    
    }
    return TRUE;
}

/*******************************************************************
函数名称：MessageRegister
函数原型：BOOL MessageRegister(P_MESSAGE_PROC pMessageProc, UINT16 usIntervalMs)
功能描述：消息模块注册消息函数和运行间隔时间
输入1   ：pMessageProc：消息处理函数
输入2   ：usIntervalMs：消息运行间隔时间
返回值  ：TRUE-成功，FALSE-失败
补充信息：当配置的滴答毫秒与间隔毫秒不是整数倍关系，或间隔毫秒为0，或函数为空返回失败
*********************************************************************/ 
BOOL MessageRegister(P_MESSAGE_PROC pMessageProc, UINT16 usIntervalMs)
{
    if (ucTableSize >= MESSAGETABLE_SIZE) 
    {
        return FALSE;
    }
    else 
    {
            if (pMessageProc == NULL || usIntervalMs == 0 || usIntervalMs % gusMsTicks != 0)
            {
                return FALSE;    
            }
            else
            {
                stMessageTable[ucTableSize].pMessageProc = pMessageProc;
                stMessageTable[ucTableSize].usIntervalTime = usIntervalMs;
            }
    }
    ucTableSize++;
    return TRUE;
}

/*******************************************************************
函数名称：MessageSend
函数原型：void MessageSend(void)
功能描述：消息发送函数
输入1   ：无
返回值  ：无
补充信息：该函数的执行时间应与消息滴答时间一致
*********************************************************************/ 
void MessageSend(void)
{
    UINT8 i =0;
    
    uwTicksCounter += gusMsTicks; 
    if (uwTicksCounter > cuwIntervalLcm) 
    {
        uwTicksCounter = gusMsTicks;    
    }
    for(i = 0 ; i < ucTableSize ; i++) 
    {
        if(stMessageTable[i].ucRunFlag == 0 && (uwTicksCounter % stMessageTable[i].usIntervalTime) == 0) 
        {
            stMessageTable[i].ucRunFlag = 1;   
        }
    }
}

/*******************************************************************
函数名称：MessageProcess
函数原型：void MessageProcess(void)
功能描述：消息处理函数
输入1   ：无
返回值  ：无
补充信息：运行满足时间间隔的消息函数
*********************************************************************/ 
void MessageProcess(void)
{  
    UINT8 i;    
    for (i = 0; i < ucTableSize; i++) 
    {
        if (stMessageTable[i].ucRunFlag == 1) 
        {
            if(stMessageTable[i].pMessageProc != NULL) 
            {
                stMessageTable[i].pMessageProc();
            }            
            stMessageTable[i].ucRunFlag = 0;
        }
    }
}
