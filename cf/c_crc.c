#include "c_crc.h"
#include "c_mem.h"

  UINT8 CrcAccNegation(const void *buffer, int count)
{
    UINT8 res = 0;
    int i;

    const unsigned char *p = buffer;

    for (i = 0; i < count; i++) {
        res += p[i];
    }
    res = ~res;
    return res;
}


  UINT16 CRC16(const void *buffer, int count) 
{
    int i;
    UINT16 usCrc = 0;
    const unsigned char *p = buffer;

    for (i = 0; i < count; i++) {
        usCrc += p[i];    
    }
    return (UINT16)(~usCrc);
}


#define    CLL  0x01
#define    CHH  0xA0
/********************************************************
函数名称：CallSumCRC16
入口：len:长度；buf:计算数据的首地址
出口：CRC：CRC结果
功能：计算CRC
*******************************************************/
  UINT16 CallSumCRC16(const void *buffer, int count)
{
    int i, j;
    UINT8 SaveHi,SaveLo,CRC16Hi,CRC16Lo;
    UINT16 usCrc;
    const unsigned char *p = buffer;
    
    CRC16Lo = 0XFF;
    CRC16Hi = 0XFF;

    for (i = 0; i < count; i++){
        CRC16Lo = CRC16Lo ^ p[i];
        for (j = 0; j <= 7; j++)
        {
            SaveHi = CRC16Hi;
            SaveLo = CRC16Lo;
            CRC16Hi = CRC16Hi>>1;
            CRC16Lo = CRC16Lo>>1;
            if(SaveHi & 0x01)
            {
                CRC16Lo = CRC16Lo | 0x80;
            }

            if(SaveLo & 0x01)
            {
                CRC16Hi = CRC16Hi ^ CHH;
                CRC16Lo = CRC16Lo ^ CLL;
            }
        }
    }
    usCrc =  (CRC16Hi << 8) + CRC16Lo;
    return usCrc;
}

