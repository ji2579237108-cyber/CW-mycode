#ifndef MIDW25N01_H
#define MIDW25N01_H
#include "CWDrv.h"
#include <stdint.h>


#define W25N01_PageSize             2048

#define FLASH_SELECT()							GPIO_SET(PTB,5,0);
#define FLASH_DISELECT()						GPIO_SET(PTB,5,1);

#define Dummy_Byte                	0x00

/* Private define ----------------------------------*/
#define W25N01_WriteEnable		      0x06 
#define W25N01_WriteDisable		      0x04 
#define W25N01_ReadStatusReg		    0x05 
#define W25N01_WriteStatusReg		    0x01 
#define W25N01_ReadData			        0x03 
#define W25N01_FastReadData		      0x0B 
#define W25N01_FastReadDual		      0x3B 
#define W25N01_PageProgram		      0x02 
#define W25N01_BlockErase			      0xD8 
#define W25N01_SectorErase		      0x20 
#define W25N01_ChipErase			      0xC7 
#define W25N01_PowerDown			      0xB9 
#define W25N01_ReleasePowerDown	    0xAB 
#define W25N01_DeviceID			        0xAB 
#define W25N01_ManufactDeviceID   	0x90 
#define W25N01_JedecDeviceID		    0x9F

void W25N01_INIT(void); 
uint16_t W25N01_ReadID(void);
void SPI_FLASH_SectorErase(uint16_t BlockAddr);
void W25N01_Write(uint8_t *pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
void W25N01_Read(uint8_t *pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead);

void W25N01G_Init(void);
uint8_t W25N01G_Write(uint32_t uwAddr, uint8_t *buf, uint16_t count);
uint8_t W25N01G_Read(uint32_t uwAddr, uint8_t *buf, uint16_t count);

#endif
