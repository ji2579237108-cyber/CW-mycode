#include "CWDrv.h"
#include "cf.h"
#include "para.h"
#include "fd.h"
#include "time.h"
#include "Appconf.h"
#include "candebug.h"
#include "appdef.h"
#include "MidCan.h"
#include "ifr.h"
#include "valvectl.h"
#include "stdtype.h"
#include "auto_addressing_token_master.h"
#include "MidDio.h"
#include "fd_update.h"
#include "algo.h"


uint8_t g_ucCanChannel = 0;

typedef enum
{
    PCBA_QRCODE = 0,
    PD_QRCODE = 1
} ENU_QRCODE;

typedef union
{
    UINT8 ucPcbaQrCode[23 + 1];
    UINT8 ucPdQrCode[20 + 1];
} UN_QRCODE;

static UN_QRCODE unQrCode;
void QrCodeRead(ENU_QRCODE eQrCode)
{
    if (eQrCode == PCBA_QRCODE)
    {
        memcpy(unQrCode.ucPcbaQrCode, pSysPara->ucPCBACode, sizeof(pSysPara->ucPCBACode));
        unQrCode.ucPcbaQrCode[23] = CrcAccNegation(unQrCode.ucPcbaQrCode, sizeof(pSysPara->ucPCBACode));
    }
    else if (eQrCode == PD_QRCODE)
    {
        memcpy(unQrCode.ucPdQrCode, pSysPara->ucPDCode, sizeof(pSysPara->ucPDCode));
        unQrCode.ucPdQrCode[20] = CrcAccNegation(unQrCode.ucPdQrCode, sizeof(pSysPara->ucPDCode));
    }
}

UINT8 QrCodeSave(ENU_QRCODE eQrCode)
{
    UINT8 ucCrc;

    if (eQrCode == PCBA_QRCODE)
    {
        ucCrc = CrcAccNegation(unQrCode.ucPcbaQrCode, 23);

        if (ucCrc == unQrCode.ucPcbaQrCode[23])
        {
            memcpy(pSysPara->ucPCBACode, unQrCode.ucPcbaQrCode, 23);
            ParaWrite();
            ParaRead();
            if (0 != memcmp(pSysPara->ucPCBACode, unQrCode.ucPcbaQrCode, 23))
            {
                return 0; // 写入失败
            }
            else
            {
                return 1; // 写入成功
            }
        }
        else
        {
            return 0;
        }
    }
    else if (eQrCode == PD_QRCODE)
    {
        ucCrc = CrcAccNegation(unQrCode.ucPdQrCode, 20);
        if (ucCrc == unQrCode.ucPdQrCode[20])
        {
            memcpy(pSysPara->ucPDCode, unQrCode.ucPdQrCode, 20);
            ParaWrite();
            ParaRead();
            if (0 != memcmp(pSysPara->ucPDCode, unQrCode.ucPdQrCode, 20))
            {
                return 0; // 写入失败
            }
            else
            {
                return 1; // 写入成功
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        // No Action
        ;
    }

    return 0;
}

typedef struct
{
    UINT8 ucTestFlag;
    UINT8 ucTestPersionId;
    UINT32 uwTestTime;
} STR_TEST_INFO;

// static UINT8 ucBuffer[16];
/* private typedef ----------------------------------------------*/
typedef void (*P_DO_DEBUG_FUNC)(STR_CAN_STD_TYPE *ptr);
typedef struct DO_DEBUG
{
    UINT8 ucDebugCmd;             //
    P_DO_DEBUG_FUNC pDoDebugFunc; //
} STR_DO_DEBUG;                   // CAN DEBUG

/* private definition -------------------------------------------*/
#define DEBUG_MOD 0x0fU        // DEBUG
#define ENTRYDEBUG_STEP1 0x04U //
#define ENTRYDEBUG_STEP4 0x07U //
/* static functions ---------------------------------------------*/
static BOOL _CanDebugDoAllowed(UINT8 cmd);
static void _CanDebugSend(STR_CAN_STD_TYPE *const ptr);
static void _DidCfgHandle(UINT8 cmd, STR_CAN_STD_TYPE *pCanf);

static PARA_RETURN_STR DidHand_Delay(UINT8 cmd, UINT8 Datelen, STR_CAN_STD_TYPE *ptr);
static PARA_RETURN_STR DidHand_DeviceId(UINT8 cmd, UINT8 Datelen, STR_CAN_STD_TYPE *ptr);
static PARA_RETURN_STR DidHand_CanBand(UINT8 cmd, UINT8 Datelen, STR_CAN_STD_TYPE *ptr);

/* private variable ---------------------------------------------*/
static UINT8 ucMode = 0;
static STR_CAN_STD_TYPE stRspCanf;
static STR_CAN_STD_TYPE *const pstRspCanf = &stRspCanf;

static const STR_DEBUG_PARA_CFG stCanDebugParaCfg[] = 
{
	{ PARA_CFG_DID_ID,			 1,		DidHand_DeviceId	},
	{ PARA_CFG_DID_CAN_BAUD	,	 1,		DidHand_CanBand	    },	
	{ PARA_CFG_DID_BAT_NUM,		 1, 	DidHand_Delay	    },
};
#define PARA_CFG_NUM ARRAY_SIZE(stCanDebugParaCfg) //长度

void _doQrCode(ENU_QRCODE eQrCode, STR_CAN_STD_TYPE *ptr)
{
    UINT8 ucWr, ucRet = 1;
    UINT8 ucLeftSize;
    UINT8 ucDataLen;
    UINT8 ucTotalPackage;
    UINT8 ucCurPackge;
    UINT8 ucQrCodeLen;
    UINT8 *p;

    if (eQrCode == PCBA_QRCODE)
    {
        ucLeftSize = ucQrCodeLen = sizeof(unQrCode.ucPcbaQrCode);
        p = unQrCode.ucPcbaQrCode;
    }
    else if (eQrCode == PD_QRCODE)
    {
        ucLeftSize = ucQrCodeLen = sizeof(unQrCode.ucPdQrCode);
        p = unQrCode.ucPdQrCode;
    }

    ucWr = ptr->data[0] >> 4;
    if (ucWr == 0)
    {
        QrCodeRead(eQrCode);
        ucTotalPackage = ucLeftSize % 6 == 0 ? ucLeftSize / 6 : ucLeftSize / 6 + 1;
        ucCurPackge = 1;
        while (ucLeftSize > 0)
        {
            ucDataLen = ucLeftSize > 6 ? 6 : ucLeftSize;
            pstRspCanf->data[0] = (ucWr << 4) + ucRet;
            pstRspCanf->data[1] = (ucCurPackge << 4) + ucTotalPackage;
            c_memcpy(&pstRspCanf->data[2], p, ucDataLen);
            pstRspCanf->len = 2 + ucDataLen;
            _CanDebugSend(pstRspCanf);

            ucCurPackge++;
            p += ucDataLen;
            ucLeftSize -= ucDataLen;
        }
    }
    else if (ucWr == 1)
    {
        ucCurPackge = ptr->data[1] >> 4;
        ucTotalPackage = ptr->data[1] & 0x0f;
        if ((ucCurPackge - 1) * 6 > ucQrCodeLen)
        {
            pstRspCanf->data[0] = (ucWr << 4) + 0;
            pstRspCanf->data[1] = ptr->data[1];
            pstRspCanf->len = 2;
            _CanDebugSend(pstRspCanf);
        }
        else
        {
            p = p + (ucCurPackge - 1) * 6;
            ucDataLen = ptr->len - 2;
            if ((ucCurPackge - 1) * 6 + ucDataLen > ucQrCodeLen)
            {
                ucDataLen = ucQrCodeLen - (ucCurPackge - 1) * 6;
            }
            c_memcpy(p, ptr->data + 2, ptr->len - 2);
        }

        if (ucCurPackge == ucTotalPackage)
        {
            ucRet = QrCodeSave(eQrCode);
        }
        pstRspCanf->data[0] = (ucWr << 4) + ucRet;
        pstRspCanf->data[1] = ptr->data[1];
        pstRspCanf->len = 2;
        _CanDebugSend(pstRspCanf);
    }
}

static void _do00hSecurity1(STR_CAN_STD_TYPE *ptr)
{
}

static void _do01hSecurity2(STR_CAN_STD_TYPE *ptr)
{
}

static void _do02hsecurity3(STR_CAN_STD_TYPE *ptr)
{
}

static void _do03hsecurity4(STR_CAN_STD_TYPE *canf)
{
    pstRspCanf->data[0] = 0xFF;
    pstRspCanf->data[1] = DEBUG_ID;
    pstRspCanf->len = 2;
    _CanDebugSend(pstRspCanf);
//    ParaWrite();
//    _IRF_DISABLE_IRQ_;
//    (void)MessageConf();
//    _IRF_ENABLE_IRQ_;
}

static void _do0DhSetPara(STR_CAN_STD_TYPE *ptr)
{
    if (ptr->data[0] == 1&&(ptr->len==5))
    {
        pSysPara->ucDeviceID  = ptr->data[1];
        pSysPara->ucCanBaud = ptr->data[2];
        pSysPara->ucEPType = ptr->data[3];
        pSysPara->ucDelay = ptr->data[4];        
    }
    pstRspCanf->data[0] = 0xff;
    pstRspCanf->data[1] = pSysPara->ucDeviceID;
    pstRspCanf->data[2] = pSysPara->ucCanBaud;
    pstRspCanf->data[3] = pSysPara->ucEPType;
    pstRspCanf->data[4] = pSysPara->ucDelay;
    pstRspCanf->len = 6;

    _CanDebugSend(pstRspCanf);
}

//==========================================================

static void _do04hEntrydebugStep1(STR_CAN_STD_TYPE *canf)
{
    if (DEBUG_MSG_ID == canf->data[0] || canf->data[0] == pSysPara->ucDeviceID)
    {
        if (!ucMode)
        {
            ucMode = 0x01;
        }
    }
}

static void _do05hEntrydebugStep2(STR_CAN_STD_TYPE *canf)
{
    if (DEBUG_MSG_ID == canf->data[0] || canf->data[0] == pSysPara->ucDeviceID)
    {
        if (ucMode == 1)
        {
            ucMode |= 0x02;
        }
    }
}

static void _do06hEntrydebugStep3(STR_CAN_STD_TYPE *canf)
{
    if (DEBUG_MSG_ID == canf->data[0] || canf->data[0] == pSysPara->ucDeviceID)
    {
        if (ucMode == 0X03)
        {
            ucMode |= 0x04;
        }
    }
}

static void _do07hEntrydebugStep4(STR_CAN_STD_TYPE *canf)
{

    if (DEBUG_MSG_ID == canf->data[0] || canf->data[0] == pSysPara->ucDeviceID )
    {
        if (ucMode == 0x07)
        {
            ucMode |= 0x08;
        }
        if (ucMode == 0x0f)
        {
            pstRspCanf->data[0] = 0xff;
            pstRspCanf->data[1] = pSysPara->ucDeviceID;
            pstRspCanf->len = 2;
            _CanDebugSend(pstRspCanf);
        }
    }
}

static void _do08hQuitDebug(STR_CAN_STD_TYPE *canf)
{
    if (canf->J1939PF != 0x08)// 阻止编译警告
    { 
        return;
    }
	ucMode = 0;
    ParaWrite();
//    _IRF_DISABLE_IRQ_;
//    (void)MessageConf();
//    _IRF_ENABLE_IRQ_;
    
//    DrvFwdgtInit(10, FWDGT_PSC_DIV64);
}

static void _do15hAskSysInf(STR_CAN_STD_TYPE *canf)
{
    if (canf->J1939PF != 0x15u)// 阻止编译警告
    { 
        return;
    }
    
    pstRspCanf->data[0] = ucMode == 0xf ? 1 : 0;
    pstRspCanf->data[1] = 0x01; 
    pstRspCanf->data[2] = 0x01;
    pstRspCanf->data[3] = tCwAppVersion[0];
    pstRspCanf->data[4] = tCwAppVersion[1];
    pstRspCanf->data[5] = tCwAppVersion[2];
    pstRspCanf->data[6] = pSysPara->ucDeviceID;
    pstRspCanf->data[7] = tCwAppVersion[3];
    pstRspCanf->len = 8;
    _CanDebugSend(pstRspCanf);
}

static void _do0EhExPara(STR_CAN_STD_TYPE *canf)
{
//    UINT8 ucError = 0;
//    if (canf->data[0] == 1)
//    { // 配置
//        if (0 == ProtocolCheck(canf->data[3]))
//        { // 外部协议检测通过
//            ucError = 0xffu;
//            pSysPara->ucExModelId = canf->data[1];
//            pSysPara->usCanExBaud = canf->data[2];
//            pSysPara->ucCanEpType = canf->data[3];
//        }
//        else
//        {
//            ucError = 0x00u;
//        }
//    }
//    else
//    { // 查询
//        ucError = 0xffu;
//    }
//    pstRspCanf->data[0] = ucError;
//    pstRspCanf->data[1] = pSysPara->ucExModelId;
//    pstRspCanf->data[2] = pSysPara->usCanExBaud;
//    pstRspCanf->data[3] = pSysPara->ucCanEpType;
//    pstRspCanf->len = 4;
//    _CanDebugSend(pstRspCanf);
}

static void _do20hClearError(STR_CAN_STD_TYPE *ptr)
{
	if (1 == ptr->data[0]) {
        DioWriteChannel(HSD_IN, FALSE);
		pSysPara->ucValveFault = 0;//清除故障
		FaultResult.bits.Fault_HSD = 0;
        ParaWrite();
		delay_ms(100);
    }
}


static void _do21hSetTime(STR_CAN_STD_TYPE *ptr) 
{
    struct tm_t tm;
    tm.tm_year = ptr->data[0] + 30;
    tm.tm_mon = ptr->data[1] - 1;
    tm.tm_mday = ptr->data[2];
    tm.tm_hour = ptr->data[3];
    tm.tm_min = ptr->data[4];
    tm.tm_sec = ptr->data[5];
    tTimeStamp = sec_time(&tm);
}

static void _do22hAskTime (STR_CAN_STD_TYPE *ptr) 
{
    struct tm_t tm;
  
    local_time(tTimeStamp, &tm);
    pstRspCanf->data[0] = (UINT8)(tm.tm_year-30);
    pstRspCanf->data[1] = (UINT8)(tm.tm_mon+1);
    pstRspCanf->data[2] = (UINT8)tm.tm_mday;
    pstRspCanf->data[3] = (UINT8)tm.tm_hour;
    pstRspCanf->data[4] = (UINT8)tm.tm_min;
    pstRspCanf->data[5] = (UINT8)tm.tm_sec;
    pstRspCanf->len = 6; 
    _CanDebugSend(pstRspCanf);
}

static void _do23hBootInfo(STR_CAN_STD_TYPE *canf)
{
    if (canf->J1939PF == 0x23u)
    {
        c_memcpy(pstRspCanf->data, pSysPara->ucBootVer, sizeof(pSysPara->ucBootVer));
        pstRspCanf->len = sizeof(pSysPara->ucBootVer);
        _CanDebugSend(pstRspCanf);
    }
}

static void _doCAhPcbaQrCode(STR_CAN_STD_TYPE *canf)
{
    _doQrCode(PCBA_QRCODE, canf);
}

static void _doCBhPdQrCode(STR_CAN_STD_TYPE *canf)
{
    _doQrCode(PD_QRCODE, canf);
}


static PARA_RETURN_STR DidHand_Delay(UINT8 cmd, UINT8 Datelen, STR_CAN_STD_TYPE *ptr)
{
	PARA_RETURN_STR paraRetStr = { 0 };
	if(0x01 == cmd)//写操作
	{
		if((Datelen + 2) == ptr->len)//判断写入数据长度是否合法
		{
			pSysPara->ucDelay = ptr->data[2] ;
			ParaWrite();
			paraRetStr.Result = PARA_CONFIG_OK;
		}
		else//写数据长度错误
		{
			paraRetStr.Result = PARA_CONFIG_LEN_ERROR;
		}
		
	}
	else//读操作
	{
		if(ptr->len != 2)
		{
			paraRetStr.Result = PARA_CONFIG_LEN_ERROR;
		}
		else
		{
			paraRetStr.ResDat = pSysPara->ucDelay;
			paraRetStr.ResLen = stCanDebugParaCfg[DID_BAT_NUM].DataLen;
			paraRetStr.Result = PARA_CONFIG_OK;
		}
	}
	return paraRetStr;
}
	

static PARA_RETURN_STR DidHand_DeviceId(UINT8 cmd, UINT8 Datelen, STR_CAN_STD_TYPE *ptr)
{
	PARA_RETURN_STR paraRetStr = { 0 };
	if(0x01 == cmd)//写操作
	{
		if((Datelen + 2) == ptr->len)//判断写入数据长度是否合法
		{
			pSysPara->ucDeviceID = ptr->data[2];
			ParaWrite();
			paraRetStr.Result = PARA_CONFIG_OK;
		}
		else//写数据长度错误
		{
			paraRetStr.Result = PARA_CONFIG_LEN_ERROR;
		}
		
	}
	else//读操作
	{
		if(ptr->len != 2)
		{
			paraRetStr.Result = PARA_CONFIG_LEN_ERROR;
		}
		else
		{
			paraRetStr.ResDat = pSysPara->ucDeviceID;
			paraRetStr.ResLen = stCanDebugParaCfg[DID_ID].DataLen;
			paraRetStr.Result = PARA_CONFIG_OK;
		}
	}
	return paraRetStr;
}

static PARA_RETURN_STR DidHand_CanBand(UINT8 cmd, UINT8 Datelen, STR_CAN_STD_TYPE *ptr)
{
	PARA_RETURN_STR paraRetStr = { 0 };
	if(0x01 == cmd)//写操作
	{
		if((Datelen + 2) == ptr->len)//判断写入数据长度是否合法
		{
			pSysPara->ucCanBaud = ptr->data[2];
			ParaWrite();
			paraRetStr.Result = PARA_CONFIG_OK;
		}
		else//写数据长度错误
		{
			paraRetStr.Result = PARA_CONFIG_LEN_ERROR;
		}
		
	}
	else//读操作
	{
		if(ptr->len != 2)
		{
			paraRetStr.Result = PARA_CONFIG_LEN_ERROR;
		}
		else
		{
			paraRetStr.ResDat = pSysPara->ucCanBaud;
			paraRetStr.ResLen = stCanDebugParaCfg[DID_CAN_BAUD].DataLen;
			paraRetStr.Result = PARA_CONFIG_OK;
		}
	}
	return paraRetStr;
}


/********************************************************************
函数名称：_doD4AskHandle
函数原型：void _doD4AskHandle(STR_CAN_STD_TYPE *pCanf)
功能描述：调用 strDID_List_D4中函数 
输入    ：
返回值  ： 
补充信息：调用时需配置 strDID_List_D4
*********************************************************************/
void _doD4AskHandle(STR_CAN_STD_TYPE *pCanf)
{
	_DidCfgHandle(0x00, pCanf);
}

/********************************************************************
函数名称：_doD5SetHandle
函数原型：void _doD5SetHandle(STR_CAN_STD_TYPE *pCanf)
功能描述：调用 strDID_List_D5中的函数 
输入    ：
返回值  ： 
补充信息：调用时需要配置strDID_List_D5 
*********************************************************************/
void _doD5SetHandle(STR_CAN_STD_TYPE *pCanf)
{
    _DidCfgHandle(0x01, pCanf);
}

static const STR_DO_DEBUG stDoDebugList[] =
{
        {0x00, _do00hSecurity1				},
        {0x01, _do01hSecurity2				},
        {0x02, _do02hsecurity3				},
        {0x03, _do03hsecurity4				}, 
        
        {0x04, _do04hEntrydebugStep1		},
        {0x05, _do05hEntrydebugStep2		},
        {0x06, _do06hEntrydebugStep3		},
        {0x07, _do07hEntrydebugStep4		},       
        {0x08, _do08hQuitDebug				},
        
        {0x0E, _do0EhExPara					},
        {0x0D, _do0DhSetPara				},
        
        {0x15, _do15hAskSysInf				},
		{ 0x20, _do20hClearError			}, //清除系统故障
		{ 0x21, _do21hSetTime				}, //系统校时
		{ 0x22, _do22hAskTime				}, //获取终端时间
        {0x23, _do23hBootInfo				},

        {0xCA, _doCAhPcbaQrCode				},
        {0xCB, _doCBhPdQrCode				},
        
        
        {0XD4, _doD4AskHandle				},	// 
        {0XD5, _doD5SetHandle				},	// 
        };

BOOL CanDebugHandler(STR_CAN_STD_TYPE *ptr)
{
    UINT8 i;

    if (ptr->J1939PRDP != 0x1A || ptr->J1939PS != 0xF5)
    {
        return FALSE;
    }
    for (i = 0; i < ARRAY_SIZE(stDoDebugList); i++)
    {
        if (stDoDebugList[i].ucDebugCmd == ptr->J1939PF)
        {
            if (_CanDebugDoAllowed(ptr->J1939PF) &&
                stDoDebugList[i].pDoDebugFunc != NULL)
            {
                pstRspCanf->CANEXDID = ptr->CANEXDID;
                pstRspCanf->J1939PS = ptr->J1939SA;
                if (ptr->J1939SA != 0xff)
				{
					pstRspCanf->J1939SA = DEBUG_ID;
				}	
				else
				{
					pstRspCanf->J1939SA = pSysPara->ucDeviceID;
				}
                pstRspCanf->exd = ptr->exd;
                stDoDebugList[i].pDoDebugFunc(ptr);
                return TRUE;
            }
        }
    }
    return FALSE;
}

BOOL CanDebugMode(void)
{
    return (ucMode == 0x0f ? TRUE : FALSE);
}

static BOOL _CanDebugDoAllowed(UINT8 cmd)
{

    if (ucMode != DEBUG_MOD &&
        (cmd < ENTRYDEBUG_STEP1 || cmd > ENTRYDEBUG_STEP4))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }

}

static void _CanDebugSend(STR_CAN_STD_TYPE *const ptr)
{
    if (g_ucCanChannel == EXCAN_MODE)
    {
        ExCan0Send(ptr);
    }
    else
    {
        InCan1Send(ptr);
    }
}


// 定义一个静态函数 _DidCfgHandle，用于处理配置DID的命令
// 参数 cmd: 命令类型
// 参数 pCanf: 指向CAN标准帧结构体的指针
static void _DidCfgHandle(UINT8 cmd, STR_CAN_STD_TYPE *pCanf)
{
	UINT8 i = 0;
	UINT16 cfgDid = 0;
	UINT8 CfgDidCheck = 0;
	PARA_RETURN_STR paraRetStr = { 0 };
	
	cfgDid = ((UINT16)pCanf->data[0] << 8 | (UINT16)pCanf->data[1]);
	for(i = 0; i < PARA_CFG_NUM; i++)
	{
		if(cfgDid == stCanDebugParaCfg[i].ParaCfgDid)
		{
			paraRetStr = stCanDebugParaCfg[i].pCfgHandle(cmd, stCanDebugParaCfg[i].DataLen, pCanf);
			CfgDidCheck = 1;
			break;
		}
		else
		{
			CfgDidCheck = 0;
		}
	}
	if(0 == CfgDidCheck)//遍历不支持的DID
	{
		pstRspCanf->data[2] = PARA_CONFIG_DID_ERROR;
	}
	else
	{
		pstRspCanf->data[2] = paraRetStr.Result;
		for(i = paraRetStr.ResLen; i > 0; i--)
		{
			pstRspCanf->data[2 + i] = (paraRetStr.ResDat >> (8 * (paraRetStr.ResLen - i))) & 0xFF;
		}
	}
	pstRspCanf->data[0] = pCanf->data[0];
    pstRspCanf->data[1] = pCanf->data[1];
    pstRspCanf->len = 3 + paraRetStr.ResLen;
    _CanDebugSend(pstRspCanf); 
}


