/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：auto_addressing.h
作者: JH
版本号:1.0
生成日期:2023.4.20
概述: 自动编码
修改日志：
*********************************************************************/
#ifndef _AUTO_ADDRESSING_H_
#define _AUTO_ADDRESSING_H_

#include "stdtype.h"
#include "para.h"

#define DEVICE_ADDR_DP     0x1A
#define DEVICE_ADDR_PF     0xD5
#define DEVICE_ADDR_PS     0xF5
#define DEVICE_ADDR_SA     0x00     //设备ID

#define ADDR_MODE_ENTER    0xCE20
#define ASK_TOKEN          0xCE21
#define ADDR_SET           0xCE22
#define TRANS_TOKEN        0xCE23

typedef void (*AddressingCanSend)(STR_CAN_STD_TYPE *ptr);
//typedef void (*AddressingCanProcess)(STR_CAN_STD_TYPE *ptr);
typedef UINT8 (*GetGpioInput)(void);
typedef UINT8 (*SetGpioOutput)(BOOL Set);

typedef struct
{
    AddressingCanSend     AutoAddressingCanSend;
    //AddressingCanProcess  AutoAddressingCanProcess;
    GetGpioInput          GetTokenState;
    SetGpioOutput         SetTokenState;
    //UINT8 VirtualID;             ///< 虚拟地址
    UINT8 DevID;                 ///< 实际地址
    UINT8 TokenTransState : 1;   ///< 令牌传递状态 0:尚未传递
    UINT8 TokenState      : 1;   ///< 令牌状态     0:未持有
    UINT8 AddrResault     : 2;   ///< 编码结果     0:尚未编码，1：编码成功  2：编码错误
    UINT8 AddrMode        : 1;   ///< 编码模式     1：进入编码模式
}AUTO_ADDRESSING;

extern void AutoAddressingGetTokenState(void);
extern void AutoAddressingCanProcess(STR_CAN_STD_TYPE *ptr);
extern void AutoAddressingSetAddr(UINT8 ID);
extern void AutoAddressingSaveId(void);
extern void AutoAddressingRegister(AddressingCanSend SendPtr, GetGpioInput InPtr, SetGpioOutput OutPtr);

BOOL CanAutoCodeHandler(STR_CAN_STD_TYPE *pCanf);

#endif /* _AUTO_ADDRESSING_H_ */

