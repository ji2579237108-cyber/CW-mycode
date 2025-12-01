#include "MidW25n01.h"

/*************************************************
函数名：W25_WriteReadReg
功能：读写函数，返回值为读到的数据
函数参数：cmd  :要写的数据
函数返回： 返回的数据                      
***************************************************/ 

#define SPI_FLASH_CS_LOW()       DrvSpiCsSet(DRV_SPI_ID_0,DRV_SPI_CS_0, 0)
#define SPI_FLASH_CS_HIGH()      DrvSpiCsSet(DRV_SPI_ID_0,DRV_SPI_CS_0, 1)
#define W25X_FLASH_SPI 			 SPI0

uint8_t W25_WriteReadReg(uint8_t cmd)
{
    return DrvSpiSendByte(DRV_SPI_ID_0, cmd);  //SPI_FLASH_SendByte(cmd);
}
/*******************************************************************************
* Function Name  : SPI_FLASH_WaitForWriteEnd
* Description    : Polls the status of the Write In Progress (WIP) flag in the
*                  FLASH's status  register  and  loop  until write  opertaion
*                  has completed.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WaitForWriteEnd(void)
{
  uint8_t FLASH_Status = 0;

  // Select the FLASH: Chip Select low 
  SPI_FLASH_CS_LOW();

   /* Send "Read Status Register" instruction */
  W25_WriteReadReg(W25N01_ReadStatusReg);
  
  W25_WriteReadReg(0xC0);
  /* Loop as long as the memory is busy with a write cycle */
  do
  {
    /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    FLASH_Status = W25_WriteReadReg(Dummy_Byte);
  }
  while ((FLASH_Status & 0x01) ==1); /* Write in progress */ 

  // Deselect the FLASH: Chip Select high 
  SPI_FLASH_CS_HIGH();
}

//地址：Axh
//BIT7  6   5   4   3   2   1   0
//SPR0 BP3 BP2 BP1 BP0 TB WP-E SPR1   
//SPR0/1:默认0,状态寄存器保护位,配合WP使用
//TB,BP3,BP2,BP1,BP0:FLASH区域写保护设置
//WP-E:写保护使能位，默认为0

//读取SPI_FLASH的状态寄存器-2(Configuration Register)
//地址：Bxh
//BIT7    6     5     4    3    2    1    0
//OTP-L OTP-E SR1-L ECC-E BUF  (R)  (R)  (R)
//OTP-L:一次性编程锁定位，默认0，置1锁定整个OTP区域
//OTP-E:OTP使能位，默认0，置1进入OTP模式
//SR1-L:状态寄存器-1锁定位
//ECC-E:ECC(纠错码)使能位，默认1
//BUF:Buffer Read Mode (BUF=1) and Continuous Read Mode (BUF=0)
//R: Reserved
//读取SPI_FLASH的状态寄存器-3(Status Only)
//地址：Cxh
//BIT7  6     5     4       3      2     1     0
//(R)  LUT-F ECC-1 ECC-0 P-FAIL E-FAIL  WEL   BUSY
//LUT-F:默认0,完全查表位。20个bad memory blocks被利用后置1 
//ECC-1/0:ECC状态位
//P/E-FAIL:Program/Erase Failure位
//WEL:写使能锁定位
//BUSY:忙标记位（1，忙；0，空闲）

void SPI_Flash_ReadSR(uint8_t sra)   
{         
	SPI_FLASH_CS_LOW();                            //使能器件   

	W25_WriteReadReg(W25N01_ReadStatusReg);
	W25_WriteReadReg(sra);                  //发送寄存器的8bit地址

    W25_WriteReadReg(Dummy_Byte);
	
	SPI_FLASH_CS_HIGH();                            //取消片选        
} 
//写SPI_FLASH状态寄存器
//SR1中：SPR1,SPR0,TB,BP3,BP2,BP1,BP0,WP-E(bit 7,6,5,4,3,2,1,0)可以写!!!
//SR2中：OTP-L,OTP-E,SR1-L,ECC-E,BUF(bit 7,6,5,4,3)可以写!!!
//SR3中：Read only.
void SPI_FLASH_WriteSR(uint8_t sr, uint8_t sra)   
{   
	SPI_FLASH_CS_LOW();                            //使能器件   
	W25_WriteReadReg(W25N01_WriteStatusReg);   //发送写取状态寄存器命令 
	W25_WriteReadReg(sra);                  //发送寄存器的地址
	W25_WriteReadReg(sr);               //写入一个字节	
	SPI_FLASH_CS_HIGH();                            //取消片选 
	SPI_FLASH_WaitForWriteEnd();    	      
}
/*******************************************************************************
* Function Name  : SPI_FLASH_WriteEnable
* Description    : Enables the write access to the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WriteEnable(void)
{
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	/* Send "Write Enable" instruction */
	W25_WriteReadReg(W25N01_WriteEnable);
	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();
}
/*******************************************************************************
* Function Name  : SPI_FLASH_WriteDisable
* Description    : Disables the write access to the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WriteDisable(void)
{
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	/* Send "Write Enable" instruction */
	W25_WriteReadReg(W25N01_WriteDisable);
	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();
}
/*******************************************************************************
* Function Name  : SPI_FLASH_SectorErase
* Description    : Erases the specified FLASH sector.
* Input          : SectorAddr: address of the sector to erase.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_SectorErase(uint16_t BlockAddr)
{
	/* Send write enable instruction */
	SPI_FLASH_WriteEnable();
	SPI_FLASH_WaitForWriteEnd();
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	/* Sector Erase */   
	/* Send Sector Erase instruction */
	W25_WriteReadReg(W25N01_BlockErase);
	/* Send SectorAddr high nibble address byte */
	W25_WriteReadReg(Dummy_Byte);
	/* Send SectorAddr medium nibble address byte */
	W25_WriteReadReg((BlockAddr & 0xFF00) >> 8);
	/* Send SectorAddr low nibble address byte */
	W25_WriteReadReg(BlockAddr & 0xFF);
	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();
	/* Wait the end of Flash writing */
	SPI_FLASH_WaitForWriteEnd();
	//SPI_FLASH_WriteDisable();
}
/*******************************************************************************
* Function Name  : SPI_FLASH_PageWrite
* Description    : Writes more than one byte to the FLASH with a single WRITE
*                  cycle(Page WRITE sequence). The number of byte can't exceed
*                  the FLASH page size.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH,
*                    must be equal or less than "SPI_FLASH_PageSize" value.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
	/* Enable the write access to the FLASH */
	SPI_FLASH_WriteEnable();

	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	/* Send "Write to Memory " instruction */
	W25_WriteReadReg(W25N01_PageProgram);
	/* Send WriteAddr high nibble address byte to write to */
	/* Send WriteAddr medium nibble address byte to write to */
	//SPI1_Flash_Send((WriteAddr & 0xFF000000) >> 24);
	/* Send WriteAddr medium nibble address byte to write to */
	//SPI1_Flash_Send((WriteAddr & 0xFF0000) >> 16);
	/* Send WriteAddr medium nibble address byte to write to */
	W25_WriteReadReg((WriteAddr & 0x0F00) >> 8);
	/* Send WriteAddr low nibble address byte to write to */
	W25_WriteReadReg(WriteAddr & 0xFF);

	if(NumByteToWrite > W25N01_PageSize)
	{
	 NumByteToWrite = W25N01_PageSize;
	 //printf("\n\r Err: SPI_FLASH_PageWrite too large!");
	}

	/* while there is data to be written on the FLASH */
	while (NumByteToWrite--)
	{
	//printf("BUF=%x\r\n", *pBuffer);
	/* Send the current byte */
	W25_WriteReadReg(*pBuffer);
	/* Point on the next byte to be written */
	pBuffer++; 
	}
	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();
	/* Wait the end of Flash writing */
	//SPI_FLASH_WaitForWriteEnd();
	//SPI_FLASH_WriteDisable();
}
void SPI_FLASH_BlockWrite(uint16_t WriteAddr) {
	SPI_FLASH_WriteEnable();
	SPI_FLASH_CS_LOW();
	W25_WriteReadReg(0x10);
	W25_WriteReadReg(Dummy_Byte);
	/* Send ReadAddr medium nibble address byte to read from */
	W25_WriteReadReg((WriteAddr& 0xFF00) >> 8);
	/* Send ReadAddr low nibble address byte to read from */
	W25_WriteReadReg(WriteAddr & 0xFF);
		
	SPI_FLASH_CS_HIGH();
	SPI_FLASH_WaitForWriteEnd();
	//SPI_FLASH_WriteDisable(); 
}
/*******************************************************************************
* Function Name  : W25N01_Write
* Description    : Writes bl ock of data to the FLASH. In this function, the
*                  number of WRITE cycles are reduced, using Page WRITE sequence.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
void W25N01_Write(uint8_t *pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
	SPI_FLASH_PageWrite(pBuffer, 0, NumByteToWrite);//写入Cache
	SPI_FLASH_BlockWrite(WriteAddr); //写入对应的FLASH物理内存 
}
/*******************************************************************************
* Function Name  : SPI_FLASH_BufferRead
* Description    : Reads a block of data from the FLASH.
* Input          : - pBuffer : pointer to the buffer that receives the data read
*                    from the FLASH.
*                  - ReadAddr : FLASH's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	/* Send "Read from Memory " instruction */
	W25_WriteReadReg(W25N01_ReadData);
	/* Send ReadAddr high nibble address byte to read from */
	/* Send WriteAddr medium nibble address byte to write to */
	//SPI1_Flash_Send((ReadAddr & 0xFF000000) >> 24);
	/* Send WriteAddr medium nibble address byte to write to */
	//SPI1_Flash_Send((ReadAddr & 0xFF0000) >> 16);
	/* Send ReadAddr medium nibble address byte to read from */
	W25_WriteReadReg((ReadAddr& 0x0F00) >> 8);
	/* Send ReadAddr low nibble address byte to read from */
	W25_WriteReadReg(ReadAddr & 0xFF);
	W25_WriteReadReg(Dummy_Byte);
	while(NumByteToRead--) /* while there is data to be read */
	{
	/* Read a byte from the FLASH */
	*pBuffer = W25_WriteReadReg(Dummy_Byte);
	/* Point to the next location where the byte read will be saved */
	pBuffer++;
	}
	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();
}

void SPI_FLASH_MemRead(uint16_t ReadAddr){
	SPI_FLASH_CS_LOW();

	/* Send "Read from Memory " instruction */
	W25_WriteReadReg(0x13);
	W25_WriteReadReg(Dummy_Byte);
	/* Send ReadAddr medium nibble address byte to read from */
	W25_WriteReadReg((ReadAddr& 0xFF00) >> 8);
	/* Send ReadAddr low nibble address byte to read from */
	W25_WriteReadReg(ReadAddr & 0xFF);
	/* Deselect the FLASH: Chip Select high    */
	SPI_FLASH_CS_HIGH();
	SPI_FLASH_WaitForWriteEnd(); 
}
void W25N01_Read(uint8_t *pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
	SPI_FLASH_MemRead(ReadAddr);
	SPI_FLASH_BufferRead(pBuffer, 0, NumByteToRead);
}

uint16_t W25N01_ReadID(void)
{
	uint16_t Temp = 0;
		  
	SPI_FLASH_CS_LOW();				    
	W25_WriteReadReg(W25N01_JedecDeviceID);//读取ID	    
	W25_WriteReadReg(Dummy_Byte); 	    
	W25_WriteReadReg(Dummy_Byte); 	    
 	 			     
	Temp|=W25_WriteReadReg(Dummy_Byte)<<8;
	Temp|=W25_WriteReadReg(Dummy_Byte);	 
	SPI_FLASH_CS_HIGH();				    
	return Temp;
}

static void spix_init(void)
{
	DrvSpiDeviceInit(DRV_SPI_ID_0);		
}

uint16_t W25N01G_PageAddr(uint16_t usPage)
{
	return ((usPage / 64) << 6) | (usPage % 64);
}

void W25N01G_Init(void)
{
   	spix_init();	/*初始化spi0，8位数据传输 */

	SPI_Flash_ReadSR(0xA0);
	SPI_FLASH_WriteSR(0x00, 0xA0);
	SPI_Flash_ReadSR(0xA0);

	SPI_Flash_ReadSR(0xB0);
	SPI_FLASH_WriteSR(0x18, 0xB0);
	SPI_Flash_ReadSR(0xB0);
}

uint8_t W25N01G_Write(uint32_t uwAddr, uint8_t *buf, uint16_t count)
{
    uint16_t usPages;
    uint16_t usPageAddr;
    
    if (uwAddr % 2048 != 0) 
    {
        return 0;
    }
    
    usPages = uwAddr / 2048;
    usPageAddr = W25N01G_PageAddr(usPages);   
    	
	if (usPageAddr % 64 == 0) 
	{
	    SPI_FLASH_SectorErase(usPageAddr);
	}
    SPI_FLASH_PageWrite(buf, 0, count);//写入Cache
    SPI_FLASH_BlockWrite(usPageAddr); //写入对应的FLASH物理内存 
    return 1;
}

uint8_t W25N01G_Read(uint32_t uwAddr, uint8_t *buf, uint16_t count)
{
    
    uint16_t usPages;
    uint16_t usPageAddr;
    
    if (uwAddr % 2048 != 0) 
    {
        return 0;
    }
    
    usPages = uwAddr / 2048;
    usPageAddr = W25N01G_PageAddr(usPages);   
    
    SPI_FLASH_MemRead(usPageAddr);
    SPI_FLASH_BufferRead(buf, 0, count);
    return 1;
}
