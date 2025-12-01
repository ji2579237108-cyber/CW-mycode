/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DrvFlash.c
作者: JH
版本号:1.0
生成日期:2023.4.4
概述: Flash驱动
修改日志：
*********************************************************************/
#include "DrvFlash.h"
#include "SEGGER_RTT.h"

/**
 * @brief 片上flash擦除
 *
 * @param Addr擦除地址，Length擦除长度
 * @brief 擦除长度小于页大小，整页擦除
 * @return 0成功
 */
S32 DrvOnChipFlashErase(const uint32_t Addr, const uint32_t Length)
{
    uint32_t EraseCounter = 0;
    S32 EraseWaitCnt = 10;
    uint32_t EraseCurAddr = Addr;
    uint32_t EraseCurLen  = Length;
    uint32_t EraseBankSize= 0;

    fmc_flag_enum BANK_END;
    fmc_flag_enum BANK_WPERR;
    fmc_flag_enum BANK_PGERR;

    if(EraseCurAddr<FLASH_START || EraseCurAddr>FLASH_END)
    {
        return -1;
    }

    fmc_unlock();

    if(EraseCurAddr<FLASH_BLK1_START)
    {
        BANK_END      = FMC_FLAG_BANK0_END;
        BANK_WPERR    = FMC_FLAG_BANK0_WPERR;
        BANK_PGERR    = FMC_FLAG_BANK0_PGERR;
        EraseBankSize = FMC_BANK0_PAGE_SIZE;
    }
    else
    {
        BANK_END      = FMC_FLAG_BANK1_END;
        BANK_WPERR    = FMC_FLAG_BANK1_WPERR;
        BANK_PGERR    = FMC_FLAG_BANK1_PGERR;
        EraseBankSize = FMC_BANK1_PAGE_SIZE;
    }
    
    EraseCurAddr = FLASH_BASE + ((EraseCurAddr-FLASH_BASE)/EraseBankSize)*EraseBankSize;

    if(EraseCurLen%EraseBankSize)
    {
        EraseCurLen = EraseCurLen/EraseBankSize + 1;
    }
    else
    {
        EraseCurLen = EraseCurLen/EraseBankSize;
    }

    for(EraseCounter=0; EraseCounter<EraseCurLen; EraseCounter++)
    {
        EraseCurAddr +=  EraseBankSize*EraseCounter;

        while(EraseWaitCnt)
        {
            fmc_flag_clear(BANK_END);
            fmc_flag_clear(BANK_WPERR);
            fmc_flag_clear(BANK_PGERR);
            FLASH_DI;
            if(FMC_READY == fmc_page_erase(EraseCurAddr))
            {
                FLASH_EI;
                break;
            }
            FLASH_EI;
            EraseWaitCnt--;
        }
        
        if(!EraseWaitCnt)
        {
            return -1;
        }
    }

    fmc_lock();

    return 0;
}
 
/**
 * @brief 片上flash全字写
 *
 * @param Addr写地址，Length写长度, 字为单位
 * @brief 
 * @return 0成功
 */
S32 DrvOnChipFlashWordWrite(const uint32_t Addr, const uint32_t* Data, const uint32_t Length)
{
    uint32_t WriteLength  = Length/4;
    uint32_t WriteWaitCnt = 10;
    uint32_t WriteCurAddr = Addr;

    fmc_flag_enum BANK_END;
    fmc_flag_enum BANK_WPERR;
    fmc_flag_enum BANK_PGERR;

    if((WriteCurAddr < FLASH_START) || ((WriteCurAddr + Length) > FLASH_END) || (NULL == Data))
    {
        return -1;
    }

    fmc_unlock();

    if(WriteCurAddr<FLASH_BLK1_START)
    {
        BANK_END      = FMC_FLAG_BANK0_END;
        BANK_WPERR    = FMC_FLAG_BANK0_WPERR;
        BANK_PGERR    = FMC_FLAG_BANK0_PGERR;
    }
    else
    {
        BANK_END      = FMC_FLAG_BANK1_END;
        BANK_WPERR    = FMC_FLAG_BANK1_WPERR;
        BANK_PGERR    = FMC_FLAG_BANK1_PGERR;
    }

    while(WriteLength--)
    {
        while(WriteWaitCnt)
        {
            fmc_flag_clear(BANK_END | BANK_WPERR | BANK_PGERR);
            FLASH_DI;

            if(FMC_READY == fmc_word_program(WriteCurAddr, *Data))
            {
                WriteCurAddr += 4;
                Data++;
                FLASH_EI;
                break;
            }

            FLASH_EI;

            WriteWaitCnt--;
        }

        if(!WriteWaitCnt)
        {
            return -1;
        }
    }

    fmc_lock();

    return 0;
}

/**
 * @brief 片上flash半字写
 *
 * @param Addr写地址，Length写长度, 半字为单位
 * @brief 
 * @return 0成功
 */
S32 DrvOnChipFlashHalfWordWrite(const uint32_t Addr, const U16* Data, const uint32_t Length)
{
    uint32_t WriteLength  = Length/2;
    uint32_t WriteWaitCnt = 10;
    uint32_t WriteCurAddr = Addr;

    fmc_flag_enum BANK_END;
    fmc_flag_enum BANK_WPERR;
    fmc_flag_enum BANK_PGERR;
	
	//DEBUG_LOG_INFO(DEBUG_LEVEL,"Flash Write Length: %d\n", WriteLength);
	
    if((WriteCurAddr < FLASH_START) || ((WriteCurAddr + Length) > FLASH_END) || (NULL == Data))
    {
        return -1;
    }

    fmc_unlock();

    if(WriteCurAddr<FLASH_BLK1_START)
    {
        BANK_END      = FMC_FLAG_BANK0_END;
        BANK_WPERR    = FMC_FLAG_BANK0_WPERR;
        BANK_PGERR    = FMC_FLAG_BANK0_PGERR;
    }
    else
    {
        BANK_END      = FMC_FLAG_BANK1_END;
        BANK_WPERR    = FMC_FLAG_BANK1_WPERR;
        BANK_PGERR    = FMC_FLAG_BANK1_PGERR;
    }

    while(WriteLength--)
    {
        while(WriteWaitCnt)
        {
            fmc_flag_clear(BANK_END | BANK_WPERR | BANK_PGERR);
            FLASH_DI;

            if(FMC_READY == fmc_halfword_program(WriteCurAddr, *Data))
            {
                WriteCurAddr += 2;
                Data++;
                FLASH_EI;
                break;
            }

            FLASH_EI;

            WriteWaitCnt--;
        }

        if(!WriteWaitCnt)
        {
            return -1;
        }
    }

    fmc_lock();

    return 0;
}

/**
* @brief	Flash读操作
*
* @param	FlashAddr   Flash地址
* @param	ReadBuf     读出数据缓冲区
* @param	Length      读出数据长度
* @note     
* @return	0成功，负数失败
*
*/
S32 DrvOnChipFlashRead(const uint32_t FlashAddr, uint8_t *const ReadBuf, const uint32_t Length)
{
	uint32_t i;
	uint8_t  *Addr = (uint8_t *)FlashAddr;	
	
	if((FlashAddr < FLASH_START) || ((FlashAddr + Length) > FLASH_END) || (NULL == ReadBuf))
	{
		return -1;
	}
	
	for(i = 0x0; i < Length; i++)
	{
		ReadBuf[i] = *Addr++;										///< 按字节读取
	}
	
	return 0;
}
