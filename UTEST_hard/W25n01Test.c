/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：W25n01Test.c
作者: LYM
版本号:1.0
生成日期:2023.4.21
概述:  对于外部存储此芯片进行的驱动测试:
			使用GD32F30X芯片驱动W25N01G外部flash芯片.
			
修改日志：
*********************************************************************/
#include "W25n01Test.h"
#include "MidW25n01.h"

uint8_t TEST_CANarr2[8]={5,0X0A,3,2,1, 0X35,0X54,0X45};
uint8_t TEST_CANarr3[8]={0X32,0X33,0X35,0X54,0X45,0,0,0};
uint8_t TEST_CANarr4[8]={0X91, 0X0A, 0X03, 0X02, 0X01, 0X0A, 0X0D, 0XEE};


/**
 * @brief NVM初始化集合函数
 *
 * @param 
 */
void NVM_init()
{
	W25N01G_Init();
}

/**
 * @brief W25N01G 首尾读写测试
 *
 * @param 
 */
void TEST_W25N01G()
{
	uint8_t i;
	W25N01G_Write(0X000000, TEST_CANarr2, 8);	
		W25N01G_Read(0X000000,TEST_CANarr3,8);
			
		for(i=0;i<8;i++)
		{
			printf("first read :%x\n",TEST_CANarr3[i]);
			DEBUG_LOG_INFO(DEBUG_LEVEL_2, " first read :%x \n",TEST_CANarr3[i]);
		}
		
		W25N01G_Write(0x7FE0000U, TEST_CANarr4, 8);	
		W25N01G_Read(0x7FE0000U,TEST_CANarr3,8);
			
		for(i=0;i<8;i++)
		{
			printf("last read :%x\n",TEST_CANarr3[i]);
			DEBUG_LOG_INFO(DEBUG_LEVEL_2, " last read :%x \n",TEST_CANarr3[i]);

		}
}
















