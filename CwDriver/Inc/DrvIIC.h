#ifndef _DRVIIC_H_
#define _DRVIIC_H_

#include "stdtype.h"
#include "gd32f30x.h"
#include "gd32f30x_gpio.h"

#define SCL_H         gpio_bit_set(GPIOB,GPIO_PIN_6);
#define SCL_L         gpio_bit_reset(GPIOB,GPIO_PIN_6);
   
#define SDA_H         gpio_bit_set(GPIOB,GPIO_PIN_7);
#define SDA_L         gpio_bit_reset(GPIOB,GPIO_PIN_7);

#define IIC_WP_H      gpio_bit_set(GPIOB,GPIO_PIN_4);
#define IIC_WP_L      gpio_bit_reset(GPIOB,GPIO_PIN_4);

#define SCL_read      gpio_input_bit_get(GPIOB ,GPIO_PIN_6)
#define SDA_read      gpio_input_bit_get(GPIOB ,GPIO_PIN_7)


void I2cReadBuff(unsigned short I2cAddress,unsigned char *I2cData,unsigned short DataLen);
void I2cWriteBuff(unsigned short I2cAddress,unsigned char *I2cData,unsigned short DataLen);
unsigned char I2cReadByte(void);
void I2cWriteByte(unsigned char SendByte);
unsigned char I2C_WaitAck(void);
void I2C_NoAck(void);
void I2C_Ack(void);
void I2C_Stop(void);
void I2C_Start(void);
void I2C_delay(unsigned char nus);
void I2C_GPIO_Config(void);

#endif /* _IIC_H_ */

