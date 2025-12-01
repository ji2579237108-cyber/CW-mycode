#include "MidM24C64.h"

#define  MAXDATA      0x20
#define  MAX_M24M     0x2000 
#define IIC_WRITE     0xA0
#define IIC_READ	  0xA1

void M24M_Init(void)
{
    I2C_GPIO_Config();
}
/*************************************************
函数名：M24M_WriteBuff
入口：add:写地址；ptr:写入数据的起始地址；writelen:写入数据长度
出口：无
功能：在add连续写writelen的数据（预防跨页）
*************************************************/
BOOL M24M_WriteBuff(UINT32 add, UINT8 *ptr, UINT16 writelen)
{
  unsigned int  PageNum;
  unsigned int  i;
  unsigned int DataLen;
  unsigned long  addrtemp;
  
  addrtemp = add;
  
  if((addrtemp + writelen) > MAX_M24M)
  {
    return 0;
  }
  PageNum = (unsigned int)(((addrtemp + writelen) / MAXDATA) - (addrtemp / MAXDATA));

  for(i=0;i<PageNum;i++)
  {
     DataLen = MAXDATA -(unsigned int) (addrtemp % MAXDATA);
     
     I2cWriteBuff(addrtemp,ptr,DataLen);
     ptr += DataLen;
     writelen -= DataLen;
     addrtemp += DataLen;     
  }
  I2cWriteBuff(addrtemp,ptr,writelen);
  return 1;
}

BOOL M24M_ReadBuff(UINT32 add, UINT8 *ptr, UINT16 writelen)
{
  unsigned int  PageNum;
  unsigned int  i;
  unsigned int DataLen;
  unsigned long addrtemp;
  
  addrtemp = add;
  if((add + writelen) > MAX_M24M)
  {
    return 0;
  }
  PageNum = (unsigned int)(((addrtemp + writelen) / MAXDATA) - (addrtemp / MAXDATA));
  
  for(i=0;i<PageNum;i++)
  {
     DataLen = MAXDATA -(unsigned int) (addrtemp % MAXDATA);
     
     I2cReadBuff(addrtemp,ptr,DataLen);
     ptr += DataLen;
     writelen -= DataLen;
     addrtemp += DataLen;     
  }
#if 0 //2.0.2.0[1]-
  I2cReadBuff(addrtemp,ptr,writelen);
#else //2.0.2.0[1]+
  if (writelen > 0) {
      I2cReadBuff(addrtemp,ptr,writelen);
  }
#endif
  return 1;
}

