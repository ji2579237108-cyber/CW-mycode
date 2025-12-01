#ifndef _NVM_H_
#define _NVM_H_

#include "stdtype.h"

#define NVM0_INDEX 0
#define NVM0_ADDR(addr) ((UINT32)addr+ ((UINT32)NVM0_INDEX << 30))
#define NVM0_INDEX_BASE_ADDR (UINT32)0

#define NVM1_INDEX 1
#define NVM1_ADDR(addr) ((UINT32)addr + ((UINT32)NVM1_INDEX << 30))//在地址前边加入序号1或2 ，在进行读写的时候，右移三十位得到序号1或者2
#define NVM1_INDEX_BASE_ADDR (UINT32)0x40000000

#define NVM2_INDEX 2
#define NVM2_ADDR(addr) ((UINT32)addr + ((UINT32)NVM2_INDEX << 30))
#define NVM2_INDEX_BASE_ADDR (UINT32)0x80000000

//////内部FLASH存储地址扇区////////

//密钥存储
#define PFLASH_KEYIN_ADD 120*4096

//B0报文逐条存储在内部FLASH：预警
#define PFLASH_BLKBOX_ALARM_VAL  50*4096

//BOOT_ID APPVALID_ADDR ：127扇区


typedef enum
{
//NVM0	inflash
	DIDEX_PFLASH_SYSPARA  = 0,
	DIDEX_PFLASH_KEY,
	

//NVM2

  DINDEX_MAX_NUM //DON'T DELETE
} ENU_DATA_INDEX;


typedef struct DATA_INDEX
{
    UINT8 ucDataIndex; //数据索引
    UINT32 uwAddr; //数据地址
    UINT32 uwLen;  //数据长度
} STR_DATA_INDEX; //数据索引结构体

typedef struct NVM_OPERATIONS
{ 
    void (*pfnInit)(void); //NVM初始化指针
	UINT8 (*pflash_erase)(UINT32 sect);//flash 擦
    UINT8 (*pfnRead)(UINT32, UINT8*, UINT16);  //NVM读指针
    UINT8 (*pfnWrite)(UINT32, UINT8*, UINT16); //NVM写指针      
} STR_NVM_OPERATIONS; //非易失存储器操作接口


typedef struct 
{ 
	UINT8 (*pNvmConfig)(void);
	UINT8 (*pNVMErase)(UINT8 ucIndex);
	UINT8 (*pNVMRead)(UINT8 ucIndex, void *ptr, UINT16 usLength);
	UINT8 (*pNVMWrite)(UINT8 ucIndex, void *ptr, UINT16 usLength);
	UINT32 (*pNVMGetLengthByIndex)(UINT8 ucIndex);
	UINT32 (*pNVMGetAddrByIndex)(UINT8 ucIndex);
	UINT8 (*pNVMEraseByOffset)(UINT8 ucIndex, UINT32 uwOffset);
	UINT8 (*pNVMReadByOffset)(UINT8 ucIndex, UINT32 uwOffset, void *ptr, UINT16 usLength);
	UINT8 (*pNVMWriteByOffset)(UINT8 ucIndex, UINT32 uwOffset, void *ptr, UINT16 usLength);
	
} NVM_INTERFACE; //NVM文件的函数操作接口






//这两个函数包含在里nvm文件里通过NvmConfig函数调用
//UINT8 NVMRegist(UINT8 ucNvmIndex, const STR_NVM_OPERATIONS *ptr);
//UINT8 NVMInit(const STR_DATA_INDEX *ptr);


UINT8 NvmConfig(void);
UINT8 NVMErase(UINT8 ucIndex);
UINT8 NVMRead(UINT8 ucIndex, void *ptr, UINT16 usLength);
UINT8 NVMWrite(UINT8 ucIndex, void *ptr, UINT16 usLength);
UINT32 NVMGetLengthByIndex(UINT8 ucIndex);
UINT32 NVMGetAddrByIndex(UINT8 ucIndex);
UINT8 NVMEraseByOffset(UINT8 ucIndex, UINT32 uwOffset);
UINT8 NVMReadByOffset(UINT8 ucIndex, UINT32 uwOffset, void *ptr, UINT16 usLength);
UINT8 NVMWriteByOffset(UINT8 ucIndex, UINT32 uwOffset, void *ptr, UINT16 usLength);



extern NVM_INTERFACE StrNvmOperaTable;

#endif /* _NVM_H_*/


