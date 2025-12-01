#include "inflash.h"
#include "gd32f30x_fmc.h"
#include "DrvFlash.h"
/*
*********************************************************************************************************
*	函 数 名: InFlash_Read_HalfWord
*	功能说明: 从指定Flash地址读取数据
*	形    参: 地址
*	返 回 值: 数据
*********************************************************************************************************
*/
UINT16 InFlash_Read_HalfWord(UINT32 Address) 
{
	UINT16 temp = 0;

	temp=*(__IO uint16_t*)Address;

	return temp;
}

/*
*********************************************************************************************************
*	函 数 名: InFlash_Read_Word
*	功能说明: 从指定Flash地址读取数据
*	形    参: 地址
*	返 回 值: 数据
*********************************************************************************************************
*/
UINT32 InFlash_Read_Word(UINT32 Address) 
{
	UINT32 temp = 0;

	temp=*(__IO UINT32*)Address;

	return temp;
}



/*
*********************************************************************************************************
*	函 数 名: InFlash_Write_HalfWord
*	功能说明: 将数据写入指定Flash地址，半字
*	形    参: Data 准备写入的数据
*	返 回 值: 数据写入状态，0-写入数据成功，1-写入数据失败
*********************************************************************************************************
*/
UINT8 InFlash_Write_HalfWord(UINT32 Address, UINT16 Data)
{
	fmc_state_enum  FLASHStatus=FMC_READY;
	fmc_unlock();
	fmc_flag_clear(FMC_FLAG_BANK0_END|FMC_FLAG_BANK0_PGERR|FMC_FLAG_BANK0_WPERR);
	FLASHStatus = fmc_halfword_program(Address,Data);
	if(FLASHStatus!=FMC_READY){
		fmc_lock();
		return	1;	
	}else{
		fmc_lock();
		return	0;	
	}
}


/*
*********************************************************************************************************
*	函 数 名: InFlash_Write_Word
*	功能说明: 将数据写入指定Flash地址，全字，fmc_word_reprogram无需检查此处是否需要擦除，这个函数不好用，老老实实的先擦再写
			  fmc_word_program 需要先擦除
*	形    参: Data 准备写入的数据
*	返 回 值: 数据写入状态，0-写入数据成功，1-写入数据失败
*********************************************************************************************************
*/
UINT8 InFlash_Write_Word(UINT32 Address, UINT32 Data)
{
	fmc_state_enum FLASHStatus=FMC_READY;
	fmc_unlock();
	fmc_flag_clear(FMC_FLAG_BANK0_END|FMC_FLAG_BANK0_PGERR|FMC_FLAG_BANK0_WPERR);
	FLASHStatus = fmc_word_program(Address,Data);
	     
	if(FLASHStatus!=FMC_READY){
		fmc_lock();
		return	1;	
	}else{
		fmc_lock();
		return	0;	
	}
}


/*
*********************************************************************************************************
*	函 数 名: InFlash_ErasePage
*	功能说明: 擦除flash，擦一个页面
*	形    参: 该页面内的任意一个地址
*	返 回 值: 数据写入状态，0-写入数据成功，1-写入数据失败
*********************************************************************************************************
*/
UINT8 InFlash_ErasePage(UINT32 StartAddr)
{
	fmc_state_enum FLASHStatus=FMC_READY;
	fmc_unlock();
	fmc_flag_clear(FMC_FLAG_BANK0_END|FMC_FLAG_BANK0_PGERR|FMC_FLAG_BANK0_WPERR);
	FLASHStatus = fmc_page_erase(StartAddr);

	if(FLASHStatus!=FMC_READY)
	{
		fmc_lock();
		return	1;	
	}
		fmc_lock();
	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: InFlash_ParaWrite_Update
*	功能说明: 擦除flash
*	形    参: 地址, boot参数
*	返 回 值: 数据写入状态，0-写入数据成功，1-写入数据失败
*********************************************************************************************************
*/
UINT8 InFlash_ParaWrite_Update(UINT32 add, UINT8 *ppara, UINT16 uslen)
{
	fmc_state_enum FLASHStatus=FMC_READY;
	UINT16 i;
	UINT16 *puspara;

	puspara = (UINT16 *)ppara;
	for(i=0; i<uslen; i++)
	{
		if(puspara[i]!=InFlash_Read_HalfWord(add+2*i))
		{
			break;
		}
		if(i==uslen-1)
		{
			return 0;
		}
	}
	
	fmc_lock();
	fmc_flag_clear(FMC_FLAG_BANK0_END|FMC_FLAG_BANK0_PGERR|FMC_FLAG_BANK0_WPERR);
	FLASHStatus = fmc_page_erase(add);
	if(FLASHStatus!=FMC_READY)
	{
		fmc_lock();
		return	1;	
	}
	for(i=0; i<uslen; i++)
	{
		FLASHStatus = fmc_halfword_program(add+2*i,puspara[i]);
		if(FLASHStatus!=FMC_READY)
		{
			fmc_lock();
			return	1;	
		}
	}

	return 0;
}


/*
*********************************************************************************************************
*	函 数 名: InFlash_ParaRead_Update
*	功能说明: 读取参数
*	形    参: 地址, 参数,长度
*	返 回 值: 数据写入状态，0-写入数据成功，1-写入数据失败
*********************************************************************************************************
*/
UINT8 InFlash_ParaRead_Update(UINT32 add, UINT8 *ppara, UINT16 uslen)
{
	UINT16 i;
	UINT16 *puspara;

	puspara = (UINT16 *)ppara;
	for(i=0; i<uslen; i++)
	{
		puspara[i] = InFlash_Read_HalfWord(add+2*i);
	}
	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: InFLASH_Read_byte
*	功能说明: 读取参数
*	形    参: 读取的起始地址, 读取值存放数组,读取长度
*	返 回 值: 数据写入状态，0-写入数据成功，1-写入数据失败
*********************************************************************************************************
*/
UINT8 InFLASH_Read_byte(UINT32 readAddr, UINT8 *dataBuffer, UINT16 dataLength)
{
    UINT16 i,time;
	union Data
	{
		UINT8 databyte[4];
		UINT32 dataword;
	}data;
	
	UINT8 remain;
	time =dataLength/4;
	

    for (i = 0; i < time; i++)
    {
        data.dataword = InFlash_Read_Word(readAddr);
	
		*(dataBuffer+4*i)=data.databyte[0];
		*(dataBuffer+4*i+1)=data.databyte[1];
		*(dataBuffer+4*i+2)=data.databyte[2];
		*(dataBuffer+4*i+3)=data.databyte[3];
        readAddr += 4;
		dataLength -=4;
    }
	
	remain = dataLength%4;
	switch(remain)
	{
		case 0:			
			break;
		case 1:
			data.dataword = InFlash_Read_Word(readAddr);
			*(dataBuffer+4*i)=data.databyte[0];
			break;
		case 2:
			data.dataword = InFlash_Read_Word(readAddr);
		
			*(dataBuffer+4*i)=data.databyte[0];
			*(dataBuffer+4*i+1)=data.databyte[1];
		
			break;
		case 3:
			data.dataword = InFlash_Read_Word(readAddr);
			*(dataBuffer+4*i)=data.databyte[0];
			*(dataBuffer+4*i+1)=data.databyte[1];
			*(dataBuffer+4*i+2)=data.databyte[2];
			break;
		default:
			break;
	}
    return TRUE;
	
}
/*
*********************************************************************************************************
*	函 数 名: InFLASH_PageProgram_byte
*	功能说明: 页编程
*	形    参: 页地址, 写入数据的数组,数据长度
*	返 回 值: 数据写入状态，0-写入数据成功，1-写入数据失败
*********************************************************************************************************
*/
UINT8 InFLASH_PageProgram_byte(UINT32 pageAddress, UINT8 *dataBuffer, UINT16 dataLength)
{
	UINT32 data;
	UINT16 time,i;
	UINT8 remain,ret;
	
	time =dataLength/4;
	for(i=0;i<time;i++){
		data=*(dataBuffer+4*i)|*(dataBuffer+4*i+1)<<8\
				|*(dataBuffer+4*i+2)<<16|*(dataBuffer+4*i+3)<<24;
		dataLength-=4;
		ret = InFlash_Write_Word(pageAddress, data);
		pageAddress+=4;
		
		if(ret!=0)
			return FALSE;
	}
	
	remain = dataLength%4;	
	switch(remain )
	{
		case 0:
			break;
		case 1:
			data=*(dataBuffer+4*i) ;
			ret = InFlash_Write_Word(pageAddress, data);
			if(ret!=0)
			return 1;
			break;
		case 2:
			data=*(dataBuffer+4*i)|*(dataBuffer+4*i+1)<<8;
			ret = InFlash_Write_Word(pageAddress, data);
			
			if(ret!=0)
			return 1;
			break;
		case 3:
			data=*(dataBuffer+4*i)|*(dataBuffer+4*i+1)<<8|*(dataBuffer+4*i+2)<<16;
			ret = InFlash_Write_Word(pageAddress, data);
			if(ret!=0)
			return 1;
			break;
		default:
			break;
	}
	 return 0;
}












