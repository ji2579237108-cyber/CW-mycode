#ifndef _RX8010_H_
#define _RX8010_H_ 

#include "stdtype.h"


#define RX8010_ADDR       0x32
#define RX8010_WRITE_ADDR 0x64
#define RX8010_READ_ADDR  0x65

#define RX8010_SEC     0x10
#define RX8010_MIN     0x11
#define RX8010_HOUR    0x12
#define RX8010_WDAY    0x13
#define RX8010_MDAY    0x14
#define RX8010_MONTH   0x15
#define RX8010_YEAR    0x16
#define RX8010_RESV17  0x17
#define RX8010_ALMIN   0x18
#define RX8010_ALHOUR  0x19
#define RX8010_ALWDAY  0x1A
#define RX8010_TCOUNT0 0x1B
#define RX8010_TCOUNT1 0x1C
#define RX8010_EXT     0x1D
#define RX8010_FLAG    0x1E
#define RX8010_CTRL    0x1F
/* 0x20 to 0x2F are user registers */
#define RX8010_RESV30  0x30
#define RX8010_RESV31  0x31
#define RX8010_IRQ     0x32

extern void  RX8010_Init(void);
extern UINT8 RX8010_ReadYearWithBCD(UINT8 *calptr);
extern UINT8 RX8010_ReadMonWithBCD(UINT8 *calptr);
extern UINT8 RX8010_ReadMdayWithBCD(UINT8 *calptr);
extern UINT8 RX8010_ReadHourWithBCD(UINT8 *calptr);
extern UINT8 RX8010_ReadMinWithBCD(UINT8 *calptr);
extern UINT8 RX8010_ReadSecWithBCD(UINT8 *calptr);
extern UINT8 RX8010_WriteYearWithBCD(UINT8 *calptr);
extern UINT8 RX8010_WriteMonWithBCD(UINT8 *calptr);
extern UINT8 RX8010_WriteMdayWithBCD(UINT8 *calptr);
extern UINT8 RX8010_WriteHourWithBCD(UINT8 *calptr);
extern UINT8 RX8010_WriteMinWithBCD(UINT8 *calptr);
extern UINT8 RX8010_WriteSecWithBCD(UINT8 *calptr);


#endif
