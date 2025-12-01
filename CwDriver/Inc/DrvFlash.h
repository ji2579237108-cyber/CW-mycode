/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DrvFlash.h
作者: JH
版本号:1.0
生成日期:2023.4.4
概述: Flash驱动
修改日志：
*********************************************************************/
#ifndef _DRVFLASH_H
#define _DRVFLASH_H
#include "DataTypeDef.h"
#include "gd32f30x.h"

#if (defined GD32F30X_HD) || (defined GD32F30X_XD) || (defined GD32F30X_CL)

#define FLASH_BANK0_SIZE         ((uint32_t)0x80000U)
#define FMC_BANK0_PAGE_SIZE      ((U16)0x800U)
#define FMC_BANK1_PAGE_SIZE      ((U16)0x1000U)

#define FLASH_OFFSET                24*1024                                     //偏移地址，boot区大小24K
#define FLASH_START					FLASH_BASE + FLASH_OFFSET					///< 主程序FLASH的起始地址
#define FLASH_SIZE					(U16)(*((volatile uint32_t *)0x1FFFF7E0))*1024	///< FLASH 大小, *1024

#define FLASH_BLK0_START			FLASH_BASE
#define FLASH_BLK1_START			(FLASH_BASE + FLASH_BANK0_SIZE)
#define FLASH_END					(FLASH_BASE + FLASH_SIZE )		            ///< FLASH结束地址	

#define FLASH_DI                    __disable_irq()
#define FLASH_EI                    __enable_irq()
#endif

#define MAIN_APP_SIZE               228*1024
#define APP_STORE_ENDADDR           FLASH_BASE + 252*1024                       
#define PARA_STORE_ADDR             0x0803F000                                  ///<  256K后4K区域
#define APPVALID_STORE_ADDR         (0x0803F000 + 2048)                         ///<  256K后2K区域

extern S32 DrvOnChipFlashErase(const uint32_t Addr, const uint32_t Length);
extern S32 DrvOnChipFlashWordWrite(const uint32_t Addr, const uint32_t* Data, const uint32_t Length);
extern S32 DrvOnChipFlashHalfWordWrite(const uint32_t Addr, const U16* Data, const uint32_t Length);
extern S32 DrvOnChipFlashRead(const uint32_t FlashAddr, uint8_t *const ReadBuf, const uint32_t Length);

#endif

 
