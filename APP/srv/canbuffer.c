#include "canbuffer.h"
#include "c_mem.h"
 /********************************************************************
版权（C），烟台创为新能源科技有限公司
文 件 名：canbuffer.c
作    者：chungway
版    本：1.0
生成日期：
概    述：CAN数据缓冲区模块
*********************************************************************/
#ifndef CAN_BUFFER_SIZE
#define CAN_BUFFER_SIZE 64
#endif
#define CAN_BUFFER_BOUND (CAN_BUFFER_SIZE-1)

#if (CAN_BUFFER_SIZE > 128)
    #error "CAN1_BUFFER_SIZE > 128"
#elif (CAN_BUFFER_SIZE ==0)
    #error "CAN_BUFFE_SIZE = 0"
#elif ((CAN_BUFFER_SIZE != 2) && \
       (CAN_BUFFER_SIZE != 8) && \
       (CAN_BUFFER_SIZE != 16) && \
       (CAN_BUFFER_SIZE != 32) && \
       (CAN_BUFFER_SIZE != 64) && \
       (CAN_BUFFER_SIZE != 128))
    #error "CAN_BUFFE_SIZE"
#endif

typedef struct
{ 
    STR_CAN_STD_TYPE stCanFrame[CAN_BUFFER_SIZE] ; //缓冲区
  	INT16 ssPr ; //读指针                         
  	INT16 ssPw ; //写指针
} STR_CAN_BUFFER ; //CAN接收缓冲区
 
static STR_CAN_BUFFER stCanBuffer; //缓冲区变量

/*******************************************************************
函数名称：CanBufferInit
函数原型：void CanBufferInit(void)
功能描述：CAN数据缓冲区初始化
输入1   ：无
返回值  ：无
补充信息：无  
*********************************************************************/
void CanBufferInit(void) 
{
    c_bzero(&stCanBuffer, sizeof(STR_CAN_BUFFER));    
}

/*******************************************************************
函数名称：CanBufferAppend
函数原型：void CanBufferAppend(STR_CAN_STD_TYPE *ptr)
功能描述：向CAN接收缓冲区追加数据
输入1   ：ptr-CAN报文指针
返回值  ：无
补充信息：一般供CAN接收中断调用 
*********************************************************************/

 void CanBufferAppend(STR_CAN_STD_TYPE *ptr)
{ 
   stCanBuffer.stCanFrame[stCanBuffer.ssPw] = *ptr;       
   stCanBuffer.ssPw = (stCanBuffer.ssPw + 1) & CAN_BUFFER_BOUND;
}

/*******************************************************************
函数名称：CanBufferGetFirst
函数原型：void CanBufferGetFirst(STR_CAN_STD_TYPE *ptr) 
功能描述：CAN缓冲区取一个CAN报文
输入1   ：ptr-CAN数据指针
输出1   ：ptr-将CAN数据指针指向的报文赋值为首先接收的报文
返回值  ：无
补充信息：使用前需要调用CanBufferIsEmpty判断是否为空 
*********************************************************************/
void CanBufferGetFirst(STR_CAN_STD_TYPE *ptr) 
{
    *ptr = stCanBuffer.stCanFrame[stCanBuffer.ssPr] ;
    stCanBuffer.ssPr = (stCanBuffer.ssPr +1) & CAN_BUFFER_BOUND;
}

/*******************************************************************
函数名称：CanBufferIsEmpty
函数原型：BOOL CanBufferIsEmpty(void) 
功能描述：判断CAN缓冲区是否为空
输入1   ：无
返回值  ：TRUE-为空，FALSE-非空
补充信息：无 
*********************************************************************/
BOOL CanBufferIsEmpty(void) 
{
    return stCanBuffer.ssPw == stCanBuffer.ssPr ? TRUE : FALSE; 
}

//2.0.0.0[12]+
static STR_CAN_BUFFER stOtCanBuffer;
void OtCanBufferInit(void) 
{
    c_bzero(&stOtCanBuffer, sizeof(stOtCanBuffer));    
}
void OtCanBufferAppend(STR_CAN_STD_TYPE *ptr)
{ 
   stOtCanBuffer.stCanFrame[stOtCanBuffer.ssPw] = *ptr;       
   stOtCanBuffer.ssPw = (stOtCanBuffer.ssPw + 1) & CAN_BUFFER_BOUND;
}
void OtCanBufferGetFirst(STR_CAN_STD_TYPE *ptr) 
{
    *ptr = stOtCanBuffer.stCanFrame[stOtCanBuffer.ssPr] ;
    stOtCanBuffer.ssPr = (stOtCanBuffer.ssPr +1) & CAN_BUFFER_BOUND;
}
BOOL OtCanBufferIsEmpty(void) 
{
    return stOtCanBuffer.ssPw == stOtCanBuffer.ssPr ? TRUE : FALSE; 
}
//2.0.0.0[12]END
