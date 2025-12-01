#include "gd32f30x.h"
#include "systick.h"

volatile static uint32_t delay;
static uint8_t  ucFac_us=0;							//us延时倍乘数
static uint16_t usFac_ms=0;							//ms延时倍乘数,在ucos下,代表每个节拍的ms数



//#define SysTick_LOAD_RELOAD_Msk            (0xFFFFFFUL)
#define MAX_SYSTICK_COUNT (SysTick_LOAD_RELOAD_Msk + 1)

/*************进系统时钟中断的延时**********************************/
/*!
    \brief      configure systick
    \param[in]  none
    \param[out] none
    \retval     none
*/
void systick_config(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / 1000U)){
        /* capture error */
        while (1){
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}


void delay_1ms(uint32_t count)
{
    delay = count;

    while(0U != delay){
    }
}


void delay_decrement(void)
{
    if (0U != delay){
        delay--;
    }
}




/**************************************************/


void SysTick_Handler(void)
{
   
}
			   
void vdelay_init(uint8_t SYSCLK)
{
	if (SysTick_Config(SystemCoreClock / 1000U)){
        /* capture error */
        while (1){}
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
	
 	SysTick->CTRL&=~(1<<2);							//SYSTICK使用外部时钟源	 
	ucFac_us=SYSCLK/8;								//不论是否使用OS,fac_us都需要使用

	usFac_ms=(uint16_t)ucFac_us*1000;				//非OS下,代表每个ms需要的systick时钟数   

}								    



////延时nus
////nus为要延时的us数.		    								   
void vdelay_us(uint32_t nus)
{		
	uint32_t temp;	    	 
	SysTick->LOAD=nus*ucFac_us; 			//时间加载	  		 
	SysTick->VAL=0x00;        				//清空计数器
	SysTick->CTRL=0x01 ;      				//开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
	SysTick->CTRL=0x00;      	 			//关闭计数器
	SysTick->VAL =0X00;       				//清空计数器	 
}

//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864 
void vdelay_ms(uint16_t nms)
{	 		  	  
	uint32_t temp;		   
	SysTick->LOAD=(uint32_t)nms*usFac_ms;	//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           			//清空计数器
	SysTick->CTRL=0x01 ;          			//开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
	SysTick->CTRL=0x00;      	 			//关闭计数器
	SysTick->VAL =0X00;       				//清空计数器	  	    
} 




void TEST_OPEN_Delay_4S()
{
	vdelay_ms(1000);
	vdelay_ms(1000);
	vdelay_ms(1000);
	vdelay_ms(1000);
	
}


/************************************
函数功能：延时毫秒
参数说明： nms：毫秒
注意:nus的值,nms:0~65535        
函数返回： 无
************************************/
void delay_ms(uint16_t nms)
{	 	 	
	uint8_t repeat=(nms*15)/2097;						//120MHz时钟频率下的计算值
											
	uint16_t remain=(nms*15)%2097;
	while(repeat)
	{
		vdelay_ms(2097);
		repeat--;
	}
	if(remain)vdelay_ms(remain);
} 















