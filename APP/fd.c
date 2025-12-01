#include "fd.h"
#include "appconf.h"
#include "c_mem.h"
#include "c_convert.h"
#include "MidCan.h"

STR_FD_DATA gstFdDataBuffer[PACK_FD_NUM];   //电池包内探测器数量

 
// 定义16位数据组合宏
#define MAKE_U16(high, low) ((uint16_t)((high) << 8) | (low))

void FdB0hHandler(STR_CAN_STD_TYPE *canf)
{
    /* 有效性检查 */
    if (canf->J1939SA > PACK_FD_NUM) return;
    
    STR_FD_DATA *data = &gstFdDataBuffer[canf->J1939SA]; // 获取数据缓存指针
    
    switch (canf->J1939PF)
    {
    case 0xB0: 
        /* 处理B0帧数据 */
        data->ucTemp1 = canf->data[0];
        data->ucTemp2 = canf->data[1];
        data->ucCo    = MAKE_U16(canf->data[2], canf->data[3]);
        data->ucVoc   = MAKE_U16(canf->data[4], canf->data[5]);

        /* 烟雾状态解析 */
        data->ucSmoken = (canf->data[6] == 0xAA) ? 0x00 : 
                        (canf->data[6] == 0x55) ? 0x05 : data->ucSmoken;
        break;
//    case 0xBA: 
//        /* 处理BA帧数据 */
//        data->ucH2    = MAKE_U16(canf->data[0], canf->data[1]);
//        data->ucCo    = MAKE_U16(canf->data[2], canf->data[3]);
//        data->ucTemp1 = canf->data[4];
//        data->ucTemp2 = canf->data[5];
//        data->ucVoc   = MAKE_U16(canf->data[6], canf->data[7]);

//        /* 烟雾状态位解析（修正位检测）*/
//        const uint8_t status = canf->data[6];
//        data->ucSmoken = (status & 0x80) ? 0x05 : 0x00; // 假设第二位检测应为其他位
//        break;
    default:
        return; // 无效PF直接返回
    }

    /* 公共字段处理（两个case都会执行）*/
    data->ucAlarm = canf->data[7] >> 4;   // 高4位报警
    
    data->ucFault = canf->data[7] & 0x0F; // 低4位故障
            /* 故障状态覆盖检查 */
    if(data->ucCheckLine == 0x01 && data->ucFault != 0x04 && data->ucFault != 0x01){    // 检测到短线 并且探测器未离线的情况下
        data->ucFault = 5; // 强制故障码
    }
    data->ucOffLineCount = 0;
}


void Fd93hHandler(STR_CAN_STD_TYPE *canf)
{
    if (canf->J1939SA > PACK_FD_NUM)
    {
        return;
    }
    switch (canf->J1939PF)
    {
    	case 0x93:
            gstFdDataBuffer[canf->data[0]].ucCheckLine = canf->data[1];
    		break;
    	default:
    		break;
    }
}



void FdInit(void) 
{	
	FdReset(); 
}

void FdReset(void)
{
    c_memset(gstFdDataBuffer, 0, sizeof(gstFdDataBuffer));
}



