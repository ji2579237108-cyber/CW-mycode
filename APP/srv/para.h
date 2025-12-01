#ifndef _PARA_H_
#define _PARA_H_

#include "stdtype.h"
#include "stdint.h"
//在UINT8 UINT16 中预留出两个位置用来在未来增加系统参数，如果预留位不够，就在最后的位置增加数据。
#pragma pack(2)
typedef struct
{
	UINT8 ucPCBACode[23];  // PCBA二维码
	UINT8 ucPDCode[20];	   // 产品二维码
	UINT8 ucBootVer[4];	   // bootloader版本号
	UINT8 ucLibraryVer[4]; // ubootloader LIBRARY_VERSION
	UINT8 ucSerialNum[5];  // 产品序列号
	UINT8  	ucCanBaud;				    //CAN波特率
	UINT8  	ucDeviceID;					//设备地址
	UINT8  	ucValveFault;				//阀门故障位
	UINT8  	ucEPType;					//can规约
	UINT8	ucWakeUpSource;				//低功耗唤醒源
	UINT8	ucDelay;					//报警延时
	UINT8	ucSmokeInit;				//烟雾初始化	
	UINT8	Reserve_2;
	UINT32  ucUartBaud;					//UART波特率
	UINT32 	ucMsgSndCyc;				//报文发送周期

	UINT32 uwRev1[57];

	UINT32 uwCrc; // CRC

} STR_SYS_PARA; // 300 byte
#pragma pack()

extern STR_SYS_PARA * const pSysPara; 
void ParaWrite(void);
void ParaRead(void);

#endif /* _PARA_H_ */
