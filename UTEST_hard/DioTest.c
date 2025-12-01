/********************************************************************
版权(C),烟台创为新能源科技有限公司
文件名：DioTest.c
作者: LYM
版本号:1.0
生成日期:2023.5.4
概述:  针对GD32F30X的GPIO驱动测试:
			进行输入输出引脚的 时钟使能，引脚初始化。
			控制引脚输出高低电平
		注意：PB4  PA15引脚需要做重映射。
修改日志：
*********************************************************************/
#include "DioTest.h"

uint8_t gucWorkMode = 0; 


void CheckModeLED(void)
{
	DioWriteChannel(RUN_LED,  1);
}



/**
 * @brief 引脚输出测试
 *
 * @note 根据需求修改引脚输出状态 
 */
void TestDioAllUp()
{
//	DioWriteChannel(DRY_POINT3,1);
//	DioWriteChannel(DRY_POINT2,1);
//	DioWriteChannel(DRY_POINT1,1);
//	DioWriteChannel(DIR_485_1,0);
//	DioWriteChannel(DIR_485_2,0);
//	DioWriteChannel(BUCK_EN,1);//降压充电
//	DioWriteChannel(BOOST_EN,0);//升压充电

//	DioWriteChannel(DRY_CODE_OUT,1);//编码线
//	DioWriteChannel(OUT1_24V,1);
//	DioWriteChannel(HSD_VAL_IN,0);
//	DioWriteChannel(HSD_FD_IN,0);
//	DioWriteChannel(PUMP_POW,1);
}



