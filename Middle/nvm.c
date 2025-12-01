#include "nvm.h"
#include <stdio.h>

/////添加的存储头文件////////
#include "inflash.h"
#include "MidM24C64.h"
#include "MidW25n01.h"

#define NVM_DEIVICE_MAX_NUMBER 4
#ifndef NVM_DEIVICE_NUMBER
#define NVM_DEIVICE_NUMBER NVM_DEIVICE_MAX_NUMBER
#endif

#if NVM_DEIVICE_NUMBER > NVM_DEIVICE_MAX_NUMBER
    #error "NVM_DEIVICE_NUMBER"
#endif 
     
#define NVM_INDEX_ADDR_MASK (UINT32)0xC0000000
#define USR_ADDR(addr) (UINT32)((addr) & 0x3FFFFFFF)

static const STR_DATA_INDEX* pDataIndexConfg = NULL; 

static STR_NVM_OPERATIONS NVMOperations[NVM_DEIVICE_NUMBER] = { 
    { NULL, NULL, NULL, NULL },    
#if NVM_DEIVICE_NUMBER >= 2
    { NULL, NULL, NULL, NULL }, 
#endif 
#if NVM_DEIVICE_NUMBER >= 3
    { NULL, NULL, NULL, NULL }, 
#endif 
#if NVM_DEIVICE_NUMBER >= 4
    { NULL, NULL, NULL, NULL }, 
#endif 
};

#define DATA_SAVE_PAGEADD_STAR	60		//从主程序后的第60页往后为数据存储区，距离flash基地址为72页
#define INFLASHADD(PageAdd)		(FLASH_START + (DATA_SAVE_PAGEADD_STAR + PageAdd)  * FMC_BANK0_PAGE_SIZE)

//////////对于存储地址的安排,根据需求更改，索引号在头文件里//////////

const STR_DATA_INDEX  gstDataIndexTable[DINDEX_MAX_NUM] = 
{
// NVM0 IN FLASH
	{DIDEX_PFLASH_SYSPARA  , NVM0_ADDR(PARA_STORE_ADDR),2048},
	{DIDEX_PFLASH_KEY , NVM0_ADDR(INFLASHADD(2) ),2048},
	

//	
//// NVM2: NULL      

};

////////////配置 存储的底层驱动函数，填入下方结构体数组里//////////
static const STR_NVM_OPERATIONS gstNvmOperationsTable[] = 
{
    //NVM0: 内部flash
    {   
        NULL,//初始化
		InFlash_ErasePage,//擦写
        InFLASH_Read_byte,
        InFLASH_PageProgram_byte,
    },
    //NVM1: M24C64
    {

        M24M_Init,//初始化
		NULL,//擦
        M24M_ReadBuff,
        M24M_WriteBuff,
    },
    //NVM2: 
    {   			 
        W25N01G_Init,//W25N01G_Init,
		NULL,
        W25N01G_Read,//W25N01G_Read,
        W25N01G_Write,//W25N01G_Write
    },
};






//NVM模块的函数注册
 UINT8 NVMRegist(UINT8 ucNvmIndex, const STR_NVM_OPERATIONS *ptr) 
{
    if (ucNvmIndex >= NVM_DEIVICE_NUMBER) 
    {
        return 1;
    } 
    else 
    {
        NVMOperations[ucNvmIndex] = *ptr;
        return 0;
    }
}

//NVM的索引初始化，送入设置好的索引，每一部分地址都存的什么东西
 UINT8 NVMInit(const STR_DATA_INDEX *ptr)
{
    UINT8 i;
    if (ptr == NULL) 
    {
        return 1;
    } 
    else 
    {
        pDataIndexConfg = ptr;
    }
    for (i = 0; i < NVM_DEIVICE_NUMBER; i++) 
    {
        if (NVMOperations[i].pfnInit != NULL) 
        {
            NVMOperations[i].pfnInit();    
        }
		
    }
    return 0;    
}







/*******************************************************************
函数名称：NvmConfig
函数原型：UINT8 NvmConfig(void)
功能描述：NVM模块配置
补充信息：在外调用该函数对NVM进行初始化
输入1   ：
输出1   ：
输出2   ：
返回    ：
*********************************************************************/
 UINT8 NvmConfig(void)
{
    UINT8 ucRet = 0;
    UINT8 ucDevIndex;
    for (ucDevIndex = 0; ucDevIndex < ARRAY_SIZE(gstNvmOperationsTable); ucDevIndex++) 
    {
        if (!NVMRegist(ucDevIndex, &gstNvmOperationsTable[ucDevIndex])) 
        {
            ucRet = ucDevIndex + 1;
        }
    }
    if (!NVMInit(gstDataIndexTable))
    {
        ucRet = 5;
    }
    return ucRet;
}


UINT8 NVMErase(UINT8 ucIndex)
{
	UINT8 bRet;
    UINT8 ucNvmIndex;
    const STR_DATA_INDEX *p;
    
    p = pDataIndexConfg + ucIndex;
    ucNvmIndex = (UINT8)(p->uwAddr >> 30);
    
    if (ucNvmIndex >= NVM_DEIVICE_NUMBER) 
    {
		printf ("read erro big \n");
        bRet = 1;
    } 
    else if (NVMOperations[ucNvmIndex].pflash_erase == NULL) 
    {
		printf ("read erro NULL \n");
        bRet = 1;
    } 
    else 
    {
        bRet = NVMOperations[ucNvmIndex].pflash_erase(USR_ADDR(p->uwAddr));
    }
    return bRet;
}



//NVM读取
UINT8 NVMRead(UINT8 ucIndex, void *ptr, UINT16 usLength)
{
    UINT8 bRet;
    UINT8 ucNvmIndex;
    const STR_DATA_INDEX *p;
    
    p = pDataIndexConfg + ucIndex;
    ucNvmIndex = (UINT8)(p->uwAddr >> 30);
   
    if (ucNvmIndex >= NVM_DEIVICE_NUMBER) 
    {
		printf ("read erro big \n");
        bRet = 1;
    } 
    else if (NVMOperations[ucNvmIndex].pfnRead == NULL) 
    {
		printf ("read erro NULL \n");
        bRet = 1;
    } 
    else 
    {
        bRet = NVMOperations[ucNvmIndex].pfnRead(USR_ADDR(p->uwAddr), (UINT8*)ptr, usLength);
    }
    return bRet;
}

//NVM写入
UINT8 NVMWrite(UINT8 ucIndex, void *ptr, UINT16 usLength)
{
    UINT8 bRet;
    UINT8 ucNvmIndex;
    const STR_DATA_INDEX *p;
    
    p = pDataIndexConfg + ucIndex;
    ucNvmIndex = (UINT8)(p->uwAddr >> 30);
    
    if (ucNvmIndex >= NVM_DEIVICE_NUMBER) 
    {
		printf ("write erro big \n");
        bRet = 1;
    } 
    else if (NVMOperations[ucNvmIndex].pfnWrite == NULL) 
    {
		printf ("write erro NULL \n");
        bRet = 1;
    } 
    else 
    {
        bRet = NVMOperations[ucNvmIndex].pfnWrite(USR_ADDR(p->uwAddr), (UINT8*)ptr, usLength);
    }
    return bRet;    
}

//获取索引中某一个数据的数据长度
 UINT32 NVMGetLengthByIndex(UINT8 ucIndex)
{
    return (pDataIndexConfg + ucIndex)->uwLen;
}
//获取索引中某一个数据的存放地址
UINT32 NVMGetAddrByIndex(UINT8 ucIndex)
{
    const STR_DATA_INDEX *p;
    p = pDataIndexConfg + ucIndex;
    return USR_ADDR(p->uwAddr);
}



UINT8 NVMEraseByOffset(UINT8 ucIndex, UINT32 uwOffset)
{
    UINT8 ucNvmIndex;
    UINT8 bRet =0;
    const STR_DATA_INDEX *p;
    
    p = pDataIndexConfg + ucIndex;        
    ucNvmIndex = (UINT8)(p->uwAddr >> 30);
    
    if (ucNvmIndex >= NVM_DEIVICE_NUMBER) 
    {
        bRet = 1;
    } 
    else if (NVMOperations[ucNvmIndex].pfnRead == NULL) 
    {
        bRet = 1;
    } else 
    {
        bRet = NVMOperations[ucNvmIndex].pflash_erase(USR_ADDR(p->uwAddr + uwOffset));
    }
    return bRet;
}





//偏移一定的位置去读取
UINT8 NVMReadByOffset(UINT8 ucIndex, UINT32 uwOffset, void *ptr, UINT16 usLength)
{
    UINT8 ucNvmIndex;
    UINT8 bRet =0;
    const STR_DATA_INDEX *p;
    
    p = pDataIndexConfg + ucIndex;        
    ucNvmIndex = (UINT8)(p->uwAddr >> 30);
    
    if (ucNvmIndex >= NVM_DEIVICE_NUMBER) 
    {
        bRet = 1;
    } 
    else if (NVMOperations[ucNvmIndex].pfnRead == NULL) 
    {
        bRet = 1;
    } else 
    {
        bRet = NVMOperations[ucNvmIndex].pfnRead(USR_ADDR(p->uwAddr + uwOffset), (UINT8*)ptr, usLength);
    }
    return bRet;
}

//偏移一定的位置去写入
 UINT8 NVMWriteByOffset(UINT8 ucIndex, UINT32 uwOffset, void *ptr, UINT16 usLength)
{
    UINT8 ucNvmIndex;
    UINT8 bRet;
    const STR_DATA_INDEX *p;
    
    p = pDataIndexConfg + ucIndex;        
    ucNvmIndex = (UINT8)(p->uwAddr >> 30);
    
    if (ucNvmIndex >= NVM_DEIVICE_NUMBER) 
    {
        return 1;
    } 
    else if (NVMOperations[ucNvmIndex].pfnWrite == NULL) 
    {
        return 1;
    } 
    else 
    {
        bRet = NVMOperations[ucNvmIndex].pfnWrite(USR_ADDR(p->uwAddr + uwOffset), (UINT8*)ptr, usLength);
    }
    return bRet;
}


NVM_INTERFACE StrNvmOperaTable =
{
	NvmConfig,
	NVMErase,
	NVMRead,
	NVMWrite,
	NVMGetLengthByIndex,
	NVMGetAddrByIndex,
	NVMEraseByOffset,
	NVMReadByOffset,
	NVMWriteByOffset,
};




