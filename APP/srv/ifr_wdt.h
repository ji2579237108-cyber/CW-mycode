#ifndef __IRF__WDT__H__
#define __IRF__WDT__H__

#include "stdtype.h"
#include "mcu.h"

typedef struct{
    void(*init)(void); //初始化看门狗
    void(*enable)(void); //使能看门狗
    void(*feed)(void); //喂看门狗，参数为内部/外部看门狗
    void(*disable)(void); //关闭看门狗
    void(*stop)(void);
} IRF_WDT;

extern const IRF_WDT irfWdt;

#endif /* __IRF__WDT__H__ */
