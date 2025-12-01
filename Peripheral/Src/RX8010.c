#include "rx8010.h"
#include "time.h"
#include "DrvIIC.h"
#include "systick.h"

#define	REG_SEC				0x10

#if NeverReferenced
static unsigned char bcd_to_hex(unsigned char data)
{
    unsigned char temp;

    temp = ((data>>4)*10 + (data&0x0f));
    return temp;
}

static unsigned char hex_to_bcd(unsigned char data)
{
    unsigned char temp;

    temp = (((data/10)<<4) + (data%10));
    return temp;
}
#endif


void Rx8010Write(unsigned char reg, unsigned char *I2cData,unsigned short DataLen)
{
    IIC_WP_L ;                    //取消写保护
	vdelay_ms(1);	
	I2C_Start();                  //启动总线
    I2cWriteByte(RX8010_WRITE_ADDR);      //发送器件地址
    I2C_WaitAck();
	I2cWriteByte(reg);	//发送器件子地址
	I2C_WaitAck();
    while(DataLen --)
	{
        I2cWriteByte(*I2cData);  //发送数据
        I2C_WaitAck();
		I2cData ++;
    }
    I2C_Stop();
	vdelay_ms(1);
	IIC_WP_H;                    //写保护
	vdelay_ms(10);
}

void RX8010_Init(void)
{
    UINT8 buf[1];
    buf[0] = 0xD8;
    Rx8010Write(RX8010_RESV17, buf, 1);
    
    buf[0] = 0x00;
    Rx8010Write(RX8010_RESV30, buf, 1);
    
    buf[0] = 0x08;
    Rx8010Write(RX8010_RESV31, buf, 1);
    
}

 uint8_t rx8010_reg_read(uint8_t reg_addr, uint8_t *buffer, uint8_t count)
{
    uint8_t i;
    I2C_Start();               //启动总线
	vdelay_ms(1);
    I2cWriteByte(RX8010_WRITE_ADDR);   //发送器件地址
    I2C_WaitAck();
	I2cWriteByte(reg_addr);	//发送器件子地址
	I2C_WaitAck();
    I2C_Start();                            //启动总线
    I2cWriteByte(RX8010_READ_ADDR);                 //发送器件地址
	I2C_WaitAck();
    for(i = 0 ; i < count ; i ++)
	{
	    *buffer++ = I2cReadByte();           //读取数据
	    if(i == count-1) I2C_NoAck();     //发送NoAck应答
        else I2C_Ack();                     //发送Ack应答
        
		buffer ++;
    }
    I2C_Stop();
	vdelay_ms(10);
    return 0;
}

 UINT8 RX8010_ReadYearWithBCD(UINT8 *calptr)
{
    rx8010_reg_read(RX8010_YEAR, calptr, 1);
    return 0;
}

 UINT8 RX8010_WriteYearWithBCD(UINT8 *calptr)
{
    Rx8010Write(RX8010_YEAR, calptr, 1);
    return 0;
}

 UINT8 RX8010_ReadMonWithBCD(UINT8 *calptr)
{
    rx8010_reg_read(RX8010_MONTH, calptr, 1);
    return 0;
}

 UINT8 RX8010_WriteMonWithBCD(UINT8 *calptr)
{
    Rx8010Write(RX8010_MONTH, calptr, 1);
    return 0;
}

 UINT8 RX8010_ReadMdayWithBCD(UINT8 *calptr)
{
    rx8010_reg_read(RX8010_MDAY, calptr, 1);
    return 0;
}

 UINT8 RX8010_WriteMdayWithBCD(UINT8 *calptr)
{
    Rx8010Write(RX8010_MDAY, calptr, 1);
    return 0;
}

 UINT8 RX8010_ReadHourWithBCD(UINT8 *calptr)
{
    rx8010_reg_read(RX8010_HOUR, calptr, 1);
    return 0;
}

 UINT8 RX8010_WriteHourWithBCD(UINT8 *calptr)
{
    Rx8010Write(RX8010_HOUR, calptr, 1);
    return 0;
}


 UINT8 RX8010_ReadMinWithBCD(uint8_t *calptr)
{
    rx8010_reg_read(RX8010_MIN, calptr, 1);
    return 0;
}

 UINT8 RX8010_WriteMinWithBCD(UINT8 *calptr)
{
    Rx8010Write(RX8010_MIN, calptr, 1);
    return 0;
}

 UINT8 RX8010_ReadSecWithBCD(UINT8 *calptr)
{
    rx8010_reg_read(RX8010_SEC, calptr, 1);
    return 0;
}

 UINT8 RX8010_WriteSecWithBCD(UINT8 *calptr)
{
    Rx8010Write(RX8010_SEC, calptr, 1);
    return 0;
}

