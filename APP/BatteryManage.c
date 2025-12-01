#include "BatteryManage.h"
#include "DiCtrl.h"
#include "DoCtrl.h"
#include "MidAdc.h"
#include "DioCfg.h"
#include "AdcCfg.h"
#include "valvectl.h"


/*
充电引脚
BUCK 拉高充电，拉低停止。
BOOST 拉低充电，拉高停止
先开BUCK，再开BOOST
*/

UINT16 gusBatVol = 0; 
UINT16 gusGet24V = 0;
static UINT8 gStopCharge = 0 ;
static UINT8 __attribute__((unused))gStopWhenOpenFire = 0;

/*************************/
void ChargeInit() //second board change
{
    // DioWriteChannel(BUCK_EN,1);
    // DioWriteChannel(BOOST_EN,0);
    CHARGE_DISABLE();
}


void CapValve()
{

		gusGet24V = AdcDmaValue(SAMPLE_12V);//获取12V电压

		gusBatVol = AdcDmaValue(SAMPLE_BAT);//获取电池电压	
}


#define BAT_CHARG_SET_TIME   30000	//MS
#define BAT_CHARGE_STOP_TIME(X)	  (X/BAT_SACAN_MS)

void ChargeContral()
{
	static UINT16 usCountTime = 0;
    //12V输入电压低于9.4V不能充电
	if(gusGet24V < POWER_24V_LOW_LIMIT  )
	{
		CHARGE_DISABLE();
		gStopCharge = 1;
		return ;
	}
	//电池电压高于15.1V，则停止充电
	if(gusBatVol > BAT_STOP_UP_VAL)
	{
		CHARGE_DISABLE();
		gStopCharge = 1;
	}	
    //电池电压低于最大充电电压且大于14.2，并维持一段时间后停止充电
	else if(gusBatVol < BAT_STOP_UP_VAL && gusBatVol > BAT_UP_LIMIT && gStopCharge == 0)
	{
		usCountTime++;
		if(usCountTime > BAT_CHARGE_STOP_TIME(BAT_CHARG_SET_TIME))
		{			
			gStopCharge		= 1;
			CHARGE_DISABLE();
		}
	}
	else if(gusBatVol < BAT_UNDER_LIMIT)
	{
		if(gStopCharge == 1)
		{
			gStopCharge 	= 0;
			usCountTime 	= 0;
		}
		CHARGE_ENABLE();	
	}
	else
	{
		usCountTime 	= 0;//在窗口期，对于14.2V上下波动计数要清零，保证电压在12.4V上维持十秒钟
	}
	
}






/*******************************************************************
函数名称：BatMgrMainProcess
函数原型：void BatMgrMainProcess(void) 
功能描述：电池充电管理
补充信息：计数为1，停下充电，，下次循环 采集电压值 判断是否充电。充电n秒后，计数清空，重新开始循环
输入1   ：
返回    ：
*********************************************************************/

void BatMgrMainProcess(void) 
{ 
    UINT8 MainPower = DiCtrl_GetPowerState();
    static INT8 MainPower_Flag = 0;//自锁继电器状态
    if(MainPower && MainPower_Flag == 0)
    {
        MainPower_Flag = 2;
        DoCtrl_BatOpen();	//接入电池
		CHARGE_DISABLE();	//首次接入不充电，完成电池开启后再进行充电逻辑
    }
	else if(MainPower_Flag == 2){
		DoCtrl_BatReset(); //复归
		MainPower_Flag = 1;
	}
	else if (MainPower == 0) {
		DoCtrl_BatClose();
		CHARGE_DISABLE();
		MainPower_Flag = -1;
	}
	else if( MainPower_Flag == -1){
		DoCtrl_BatReset(); //复归
		MainPower_Flag = 0;
	}
	else {
		//no action
	}
	/*只有主电在线的情况才管理充电*/
	if(MainPower_Flag == 1)
	{
		if(ValveCtrl_GetStatus() == CONTROL_OPEN)//开阀不允许充电
		{
			CHARGE_DISABLE();
			gStopCharge = 1;
			return ;
		}	
		CapValve();
		ChargeContral();
	}
	else {
		CHARGE_DISABLE();
		return ;
	}
	
	
}




