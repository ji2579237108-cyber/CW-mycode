
/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：IO_Rename.h
作者: LXD
版本号:1.0
生成日期:2023.4.21
概述:  针对使用GD32F307芯片的60B引脚进行封装并命名:
			在其他测试文件内所引用的引脚均使用本文件的引脚封装名。
修改日志：
*********************************************************************/

#ifndef GPIOMAP_H
#define GPIOMAP_H


#include "CWDrv.h"
#include "DataTypeDef.h"

#define USE_GD32F307_VX

#define  GD32F307Zx  144
#define GD32F307Vx	100
#define GD32F307Rx  64

/**
 * @brief 引脚配置参数对应结构表
 *
 */
typedef struct  
{
    uint32_t uwPort; ///< 
	uint32_t uwPin;  ///< 
} STR_GPIO_MAP ; 

extern const STR_GPIO_MAP *g_gpio_ptr;
#if defined(USE_GD32F307_ZX)
extern const STR_GPIO_MAP  GD32ZxGpioList[GD32F307Zx + 1];
#elif defined(USE_GD32F307_VX)
extern const STR_GPIO_MAP  GD32VxGpioList[GD32F307Vx + 1];
#elif defined(USE_GD32F307_RX)
extern const STR_GPIO_MAP  GD32RxGpioList[GD32F307Rx + 1];
#endif


#endif
