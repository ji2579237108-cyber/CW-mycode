#include "can_ip_handler.h"
#include "para.h"
#include "fdparse.h"
#include "candebug.h"
#include "MidCan.h"
#include "canboot.h"
#include "auto_addressing_token_master.h"
#include "fd_update.h"
#include "fd.h"

#if 0
#define SHIELD_CONDITION(id) ((id) >= pSysPara->ucShieldStId && \
                            (id) < (pSysPara->ucShieldStId + pSysPara->ucShieldNum - 1))

static UINT8 ucStartId = 0;

static void SendRelayRequest(uint8_t PRDP, uint8_t PF, uint8_t PS, uint8_t SA, uint8_t data0, uint8_t data1);
static void InitCanFrame(STR_CAN_STD_TYPE* frame, uint32_t exdid, uint8_t len);
static void PackCommonData(STR_CAN_STD_TYPE* frame, const STR_FD_DATA* sensor);
static inline uint8_t GetNextId(UINT8 start);

static inline BOOL CheckID(UINT8 ucFdID) {
    return (ucFdID >= pSysPara->ucShieldStId && \
            ucFdID <= (pSysPara->ucShieldStId + pSysPara->ucShieldNum - 1));
}


/*上电后将起始ID赋值*/
void CanIP_FDid_Init(void)
{
    ucStartId = pSysPara->ucFdSTid;
}
void CanIpTmrSend(void)
{	
    if(gucFdUpdateFllag == TRANSPARENT_ENABLE) return;
    //------------------------------------------屏蔽ID--------------------------------------------//
    
    if(pSysPara->ucShieldNum!=0xFF && CheckID(ucStartId))
    {
          ucStartId = pSysPara->ucShieldStId + pSysPara->ucShieldNum;
    }

    //---------------------------------------中继器请求报文-----------------------------------------//
    SendRelayRequest(0x18, 0xA2, ucStartId, 0xF6, 0x5A, 0x4E);
    //短线检测功能开发
    SendRelayRequest(0x1A, 0x93, 0xF5, 0xF4, ucStartId, 0);
    //-----------------------------------------主动上送报文-----------------------------------------//
    STR_CAN_STD_TYPE ExstIpCanf;
    const STR_FD_DATA* sensor = &gstFdDataBuffer[ucStartId];
    
    // 第一组数据报文
    InitCanFrame(&ExstIpCanf, 0x18BCE900 + pSysPara->ucDeviceID, 8);
    ExstIpCanf.data[0] = ucStartId;
    PackCommonData(&ExstIpCanf, sensor);
    ExCan0Send(&ExstIpCanf);

    // 第二组数据报文
    InitCanFrame(&ExstIpCanf, 0x18BDE900 + pSysPara->ucDeviceID, 8);
    ExstIpCanf.data[0] = ucStartId;
    ExstIpCanf.data[2] = sensor->ucSmoken;
    ExstIpCanf.data[3] = sensor->ucH2 >> 8;
    ExstIpCanf.data[4] = sensor->ucH2 & 0xFF;
    ExstIpCanf.data[6] = sensor->fdStatus.ucStatus;
    ExCan0Send(&ExstIpCanf);

    //--------------------------------------离线判断--------------------------------------------------//
    if(++gstFdDataBuffer[ucStartId].ucOffLineCount >= (100/(pSysPara->ucModuleNum/2)))
    {
        gstFdDataBuffer[ucStartId].ucCheckLine = 0x00;  
        gstFdDataBuffer[ucStartId].ucFault = 0;
        gstFdDataBuffer[ucStartId].ucFault |= (1 << 2); // 通讯故障
    }

    //---------------------------------------轮询优化--------------------------------------------------//
    ucStartId = GetNextId(ucStartId);
}

/**************************** 工具函数实现 ****************************/
static void SendRelayRequest(uint8_t PRDP, uint8_t PF, uint8_t PS, uint8_t SA, 
                            uint8_t data0, uint8_t data1)
{
    STR_CAN_STD_TYPE frame;
    memset(&frame, 0, sizeof(frame));
    
    frame.J1939PRDP = PRDP;
    frame.J1939PF = PF;
    frame.J1939PS = PS;
    frame.J1939SA = SA;
    frame.len = (data1 != 0) ? 2 : 1; // 根据数据长度自动判断
    frame.exd = 1;
    frame.data[0] = data0;
    if(frame.len > 1) frame.data[1] = data1;
    InCan1Send(&frame);
}

static void InitCanFrame(STR_CAN_STD_TYPE* frame, uint32_t exdid, uint8_t len)
{
    memset(frame, 0, sizeof(STR_CAN_STD_TYPE));
    frame->canid.exdid = exdid;
    frame->len = len;
    frame->exd = 1;
}

static void PackCommonData(STR_CAN_STD_TYPE* frame, const STR_FD_DATA* sensor)
{
    frame->data[1] = sensor->ucTemp1;
    frame->data[2] = sensor->ucTemp2;
    frame->data[3] = sensor->ucCo >> 8;
    frame->data[4] = sensor->ucCo & 0xFF;
    frame->data[5] = sensor->ucVoc >> 8;
    frame->data[6] = sensor->ucVoc & 0xFF;
    frame->data[7] = (sensor->ucAlarm & 0x0F) << 4 | (sensor->ucFault & 0x0F);
}

// 获取下一个有效ID

static inline uint8_t GetNextId(uint8_t start)
{
    static uint8_t Count = 0;
    
    if (++Count >= pSysPara->ucModuleNum) 
    {
        Count = 0;
        return pSysPara->ucFdSTid;
    }
    
    return start + ((start & 1) ? 1 : 3);
}
#endif
/*******************************************************************
函数名称：CanIpHandler
函数原型：void CanIpHandler(STR_CAN_STD_TYPE *canf) 
功能描述：CAN协议处理
补充信息：
输入1   ：canf: CAN FRAME
返回    ：
*********************************************************************/ 
void CanIpHandler(STR_CAN_STD_TYPE *canf)
{
//    BOOL bRet;
//	g_ucCanChannel = INCAN_MODE;
//    bRet = FdParse(canf); 				if (bRet == TRUE) return; 	// 探测器07A数据解析
//	bRet = AutoCodeHandler(canf); 		if (bRet == TRUE) return; 	// 自动编码模块
//    bRet = CanDebugHandler(canf); 		if (bRet == TRUE) return; 	// debug
//	bRet = CanBootHandler(canf); 		if (bRet == TRUE) return;	// boot
//	bRet = RecvFdData(canf); 			if (bRet == TRUE) return; 	// 透传模式下使用：给上位机反馈升级过程报文（0x1B）
//	bRet = TransparentDebug(canf);		if (bRet == TRUE) return;	// 透传模式下使用
}



