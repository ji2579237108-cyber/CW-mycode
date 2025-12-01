#ifndef _STDTYPE_H_
#define _STDTYPE_H_

#include <stdio.h>
#include <string.h>


/* bootloader library version */														
#define LIBRARY_VERSION_MAJOR   (1)
#define LIBRARY_VERSION_MINOR	(0)
#define LIBRARY_VERSION_LAST	(0)
#define LIBRARY_VERSION_ALONE	(0)

typedef unsigned char    BOOL;
typedef unsigned char    UINT8;  /* unsigned 8-bit */
typedef unsigned short   UINT16; /* unsigned 16-bit */
typedef unsigned int     UINT32; /* unsigned 32-bit */
typedef signed char      INT8;   /* signed 8-bit */
typedef short            INT16;  /* signed 16-bit */
typedef int              INT32;  /* signed 32-bit */

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif



#define UNKNOWN_ENDIAN   0
#define LITTLE_ENDIAN    1
#define BIG_ENDIAN       2
#define CHIP_ENDIAN      LITTLE_ENDIAN


typedef union {
    UINT32 exdid;
    UINT16 stdid; 
    struct {
#if (CHIP_ENDIAN == BIG_ENDIAN)
        UINT8 p_r_dp;
        UINT8 pf;
        UINT8 ps;
        UINT8 sa;
#elif (CHIP_ENDIAN == LITTLE_ENDIAN)
        UINT8 sa;
        UINT8 ps;
        UINT8 pf;
        UINT8 p_r_dp;
#else
    #error "J1939Id"
#endif        
    } J1939Id;
} CanIdStr;

typedef struct 
{
    CanIdStr  canid;
    UINT8     rtr;
    UINT8     exd; /* 0-Standard, 1-Extend */
    UINT8     prty;
    UINT8     len;
	UINT8     Channel;  //LHJ新增CAN通道
    UINT8     data[8];
    #define   J1939ID   canid.J1939Id
    #define   J1939PRDP canid.J1939Id.p_r_dp
    #define   J1939PF   canid.J1939Id.pf
    #define   J1939SA   canid.J1939Id.sa
    #define   J1939PS   canid.J1939Id.ps
    #define   CANEXDID  canid.exdid
    #define   CANSTDID  canid.stdid
}STR_CAN_STD_TYPE;      

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))

#ifndef INT8_MIN
    #define INT8_MIN         (-127-1)   
#endif
#ifndef INT8_MAX
    #define INT8_MAX              127
#endif
#ifndef INT16_MIN
    #define INT16_MIN      (-32767-1)  
#endif
#ifndef INT16_MAX
    #define INT16_MAX           32767
#endif
#ifndef UINT8_MAX
    #define UINT8_MAX      (INT8_MAX-INT8_MIN)
#endif
#ifndef UINT16_MAX
    #define UINT16_MAX     (INT16_MAX-INT16_MIN)  
#endif
#define INT8_TYPE      0
#define INT16_TYPE     1
#define UINT8_TYPE     2
#define UINT16_TYPE    3

#ifndef NULL
#define NULL ((void *) 0)
#endif

#define PRINTF (void)printf
	
//CAN滤波定义
#define  CAN0_BOOT_ACPID 		0x1B00FD00 
#define  CAN0_MSKID             0xFF00FF00     //0表示不关心，1表示过滤
/* CAN update  */
#define BOOT_ID 0xE9            //探测器ID

/*************************************************************************************/
/* bootloader library version */														
#define BOOT_VERSION_MAJOR   (10)
#define BOOT_VERSION_MINOR	 (0)
#define BOOT_VERSION_LAST	 (0)
#define BOOT_VERSION_ALONE	 (0)
/********************************************************/
/*		 		
The following is the MCU configuration
*/
/********************************************************/
/* mcu flash page size */
#define MCU_PAGE_SIZE 2048
/* mcu flash total size */
#define MCU_PROGRAM_FLASH_TOTAL_SIZE  256
/* mcu flash start addr */
#define MCU_PROGRAM_FLASH_STATT_ADDR 0x0000U
/* mcu application start address , must % 4096 (4K) */
#define MCU_APP_START_ADDR  0x4000
/**************************************************************************************/

#endif
