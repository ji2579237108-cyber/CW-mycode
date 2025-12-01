#include "DrvIIC.h"
#include "systick.h"

#define IIC_WRITE     0xA0
#define IIC_READ	  0xA1
//===============================================================
//   端口初始化
void I2C_GPIO_Config(void)
{
	gpio_init(GPIOB,GPIO_PIN_7,GPIO_OSPEED_50MHZ,GPIO_MODE_OUT_OD);
	gpio_init(GPIOB,GPIO_PIN_6,GPIO_OSPEED_50MHZ,GPIO_MODE_OUT_OD);
	gpio_init(GPIOB,GPIO_PIN_4,GPIO_OSPEED_50MHZ,GPIO_MODE_OUT_PP);

}


void I2C_delay(unsigned char nus)
{	
   unsigned long i=0; //这里可以优化速度	，经测试最低到5还能写入
   while(nus--) 
   { 
     for(i=0;i<16;i++)
	 {
	     ;
	 } 
   } 
}
//==========================================
//I2C起始条件
//==========================================
void I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay(4);
	SDA_L;
	I2C_delay(1);
	SDA_L;
	I2C_delay(4);
}
//==========================================
//I2C停止条件
//==========================================
void I2C_Stop(void)
{
	SCL_L;
	I2C_delay(1);
	SDA_L;
	I2C_delay(4);
	SCL_H;
	I2C_delay(1);
	SDA_H;
	I2C_delay(1);
}
//==========================================
//发ACK应答
//==========================================
void I2C_Ack(void)
{	
	
	SCL_L;
	I2C_delay(1);
	SDA_L;
	I2C_delay(1);
	SCL_H;
	I2C_delay(1);
	SCL_L;
	I2C_delay(1);


}
//==========================================
//发NOACK应答
//==========================================
void I2C_NoAck(void)
{	

	SCL_L;
	I2C_delay(1);
	SDA_H;
	I2C_delay(1);
	SCL_H;
	I2C_delay(1);
	SCL_L;
	I2C_delay(1);
	
}
//==========================================
//等待应答
//==========================================
unsigned char I2C_WaitAck(void) 	 //返回为:=1有ACK,=0无ACK
{
	unsigned char i;
	SCL_L;
	I2C_delay(1);
	SDA_H;			
	I2C_delay(1);
	SCL_H;
	I2C_delay(1);
	
	for(i=0; i<30; i++)
	{
	    if(!SDA_read)
	    {
            SCL_L;
			I2C_delay(1);
        	return 1;
	    }
	}
	SCL_L;
	return 0;
}

//===============================================================
//I2C写一字节数据
//===============================================================
void I2cWriteByte(unsigned char SendByte) //数据从高位到低位//
{
    unsigned char i=8;
    
	while(i--)
    {
        SCL_L;
        I2C_delay(1);
        if(SendByte&0x80)
		{
			SDA_H;
		}    
        else 
		{
			SDA_L;
		}      
        SendByte<<=1;
        I2C_delay(1);
		SCL_H;
        I2C_delay(1);
    }
    SCL_L;

}

//===========================================================
//I2C读一字节数据
//===========================================================
unsigned char I2cReadByte(void)  //数据从高位到低位//
{ 
	unsigned char i=8;
    unsigned char ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
		ReceiveByte<<=1;      
		SCL_L;
		I2C_delay(1);
		SCL_H;
		I2C_delay(1);	
		if(SDA_read)
		{
		    ReceiveByte|=0x01;
		}
    }
    SCL_L;
    return ReceiveByte;

}

//========================================================================================
//  函数功能   ：将数组的数据保存到指定起始地址
//  I2cAddress : 写数据的起始地址
//  I2cData    : 数据的保存数组
//  DataLen    : 读出数据的长度
//=========================================================================================
void I2cWriteBuff(unsigned short I2cAddress,unsigned char *I2cData,unsigned short DataLen)
{
    IIC_WP_L ;                    //取消写保护
	delay_ms(1);	
	I2C_Start();                  //启动总线
    I2cWriteByte(IIC_WRITE);      //发送器件地址
    I2C_WaitAck();
	I2cWriteByte((I2cAddress >> 8) & 0x7F);	//发送器件子地址
	I2C_WaitAck();
	I2cWriteByte(I2cAddress & 0xFF);
	I2C_WaitAck();
    while(DataLen --)
	{
        I2cWriteByte(*I2cData);  //发送数据
        I2C_WaitAck();
		I2cData ++;
    }
    I2C_Stop();
	delay_ms(1);
	IIC_WP_H;                    //写保护
	delay_ms(10);
}
//=======================================================================================
//  函数功能：I2C读指定起始地址和长度的数据到数组中
//  I2cAddress : 读数据的起始地址
//  I2cData    : 数据的保存数组
//  DataLen    : 读出数据的长度
//=======================================================================================
void I2cReadBuff(unsigned short I2cAddress,unsigned char *I2cData,unsigned short DataLen)
{
    unsigned short i;
    I2C_Start();               //启动总线
	delay_ms(1);
    I2cWriteByte(IIC_WRITE);   //发送器件地址
    I2C_WaitAck();
	I2cWriteByte((I2cAddress >> 8) & 0x7F);	//发送器件子地址
	I2C_WaitAck();
	I2cWriteByte(I2cAddress & 0xFF);
	I2C_WaitAck();
    I2C_Start();                            //启动总线
    I2cWriteByte(IIC_READ);                 //发送器件地址
	I2C_WaitAck();
    for(i = 0 ; i < DataLen ; i ++)
	{
	    *I2cData = I2cReadByte();           //读取数据
	    if(i == DataLen-1) I2C_NoAck();     //发送NoAck应答
        else I2C_Ack();                     //发送Ack应答
        
		I2cData ++;
    }
    I2C_Stop();
	delay_ms(10);
}
