#include "c_convert.h"

/*******************************************************************
函数名称：array_to_msb32
函数原型：void array_to_msb32(const void *buf, UINT32 *ptr)
功能描述：数组转32位
输入1   ：buf
输入2   ：ptr
输出1   ：ptr
返回    ：
*********************************************************************/ 
  void array_to_msb32(const void *buf, UINT32 *ptr)
{
    const UINT8 *p = buf;
    *ptr = ((UINT32)p[0] << 24) + ((UINT32)p[1] << 16) + \
           ((UINT32)p[2] << 8) + (UINT32)p[3];
}

/*******************************************************************
函数名称：array_to_msb16
函数原型：void array_to_msb16(const void *buf, UINT32 *ptr)
功能描述：数组转16位
输入1   ：buf
输入2   ：ptr
输出1   ：ptr
返回    ：
*********************************************************************/ 
  void array_to_msb16(const void *buf, UINT16 *ptr)
{
    const UINT8 *p = buf;
    *ptr = ((UINT16)p[0] << 8) + (UINT16)p[1];
}

/*******************************************************************
函数名称：msb32_to_array
函数原型：void msb32_to_array(unsigned char *array, UINT32 uwData) 
功能描述：32位转数组
输入1   ：array
输入2   ：uwData
输出1   ：array
返回    ：
*********************************************************************/ 
  void msb32_to_array(UINT32 uwData, unsigned char *array) 
{
    array[0] = (UINT8)(uwData >> 24); 
    array[1] = (UINT8)(uwData >> 16);  
    array[2] = (UINT8)(uwData >> 8);  
    array[3] = (UINT8)(uwData & 0xff); 
}

/*******************************************************************
函数名称：msb16_to_array
函数原型：void msb16_to_array(unsigned char *array, UINT32 usData) 
功能描述：16位转数组
输入1   ：array
输入2   ：usData
输出1   ：array
返回    ：
*********************************************************************/ 
  void msb16_to_array(UINT16 usData, unsigned char *array)
{
    array[0] = (UINT8)(usData >> 8); 
    array[1] = (UINT8)(usData & 0xff);         
}

/*******************************************************************
函数名称：dec_to_bcd
函数原型：UINT8 dec_to_bcd(UINT8 data)
功能描述：十进制转BCD，例如0x20->20
输入1   ：data
返回    ：
*********************************************************************/ 
  UINT8 dec_to_bcd(UINT8 data)
{
   return (((data / 10) << 4) & 0xf0) + ((data % 10) & 0x0f); 
}

/*******************************************************************
函数名称：bcd_to_dec
函数原型：UINT8 bcd_to_dec(UINT8 data)
功能描述：BCD转十进制，例如20->0x20
输入1   ：data
返回    ：
*********************************************************************/
  UINT8 bcd_to_dec(UINT8 data)
{
    return  ((data >> 4) * 10 + (data & 0xf));
}

UINT16 u16_frombe(const UINT8 *buf) {
    return (buf[0] << 8) | buf[1];
}
