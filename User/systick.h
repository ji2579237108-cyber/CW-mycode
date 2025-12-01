

#ifndef SYS_TICK_H
#define SYS_TICK_H

#include <stdint.h>

/* configure systick */
void systick_config(void);
/* delay a time in milliseconds */
void delay_1ms(uint32_t count);
/* delay decrement */
void delay_decrement(void);

void vdelay_init(uint8_t SYSCLK);
void vdelay_ms(uint16_t nms);
void vdelay_us(uint32_t nus);

void delay_ms(uint16_t nms);
void TEST_OPEN_Delay_4S(void);



#endif /* SYS_TICK_H */
