#ifndef __INFLASH__H__
#define __INFLASH__H__
#include "stdtype.h"
#include "CWDrv.h"

//应用程序起始地址
//#define APP_ADDR			0X8006000
//升级成功标志
//#define SUCCESS_ADDR		0X801F800
//#define BOOT_ADDR 			0X801F800
//#define SUCCESSFLAG   		0x5a4e
//#define PAGE_SIZE			0x400


extern UINT16 InFlash_Read_HalfWord(UINT32 Address);
extern UINT32 InFlash_Read_Word(UINT32 Address);
extern UINT8 InFlash_Write_HalfWord(UINT32 Address, UINT16 Data);
extern UINT8 InFlash_Write_Word(UINT32 Address, UINT32 Data);
extern UINT8 InFlash_ErasePage(UINT32 StartAddr);
extern UINT8 InFlash_ParaWrite_Update(UINT32 add, UINT8 *ppara, UINT16 uslen);
extern UINT8 InFlash_ParaRead_Update(UINT32 add, UINT8 *ppara, UINT16 uslen);
extern UINT8 InFLASH_Read_byte(UINT32 readAddr, UINT8 *dataBuffer, UINT16 dataLength);
extern UINT8 InFLASH_PageProgram_byte(UINT32 pageAddress, UINT8 *dataBuffer, UINT16 dataLength);


#endif





