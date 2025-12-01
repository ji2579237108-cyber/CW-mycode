#include "auto_addressing_token_master.h"
#include "stdint.h"
#include "para.h"

#if 0
/*  max timeout */ 
#define EFFICTIVE_TIME_OUT(x)          ((x * 1000)/ENCODE_PROCESS_EXCUTE_TIME) 

/*  Reply to the packet  */
#define TRANS_IN_PACKET                 0x1AD5F5F4
#define TRANS_EXP_PACKET                0x1AD5F400 + pSysPara->ucDeviceID

int runtimeflag;
    
extern ENU_CODE_MODE encodeswtich = AUTOCODE_CLOSE; //the swtich of auto-encoding

STR_CODE_DATA  MasterCodingData;             // the data during encoding

static STR_CAN_STD_TYPE canf;
    
/**
 * @brief this enum is used the function reply_coding_result()
 * 
 */
typedef enum 
{
    ENCODE_RESULT = 0,
    ILLEGAL_PARA,
    SEQUENCE_MAX
}ENU_REPLAY_TYPE;



/**
 * @brief when coding is processing ,effictive time add .
 *        if timeout occurs ,change mode to finish.
 * 
 */
static void EffictiveTimeAdd(void)
{
    if(MasterCodingData.CurrentMode == TOKEN_TRANS)
    {
        MasterCodingData.EffictiveTime ++;
        if(MasterCodingData.EffictiveTime >= EFFICTIVE_TIME_OUT(1))
        {
            MasterCodingData.CurrentMode = TOKEN_CODE_FINISH;    
        }
    }
    MasterCodingData.EffictiveTime ++;
    if(MasterCodingData.EffictiveTime >= EFFICTIVE_TIME_OUT(10))    //TODO:修改判断次数 -V1.3.7.2
    {
        MasterCodingData.CurrentMode = TOKEN_CODE_FINISH;    
    }
}

void FD_exit_coding(void)
{
    //广播探测器结束编码模式
    canf.canid.exdid = TRANS_IN_PACKET ;
    canf.data[0] = 0xCE;
    canf.data[1] = 0x20;
    canf.data[2] = 0x00;
    canf.exd = 1;
    canf.len = 3;
    InCan1Send(&canf);
    
    //探测器退出调试模式
    canf.canid.exdid = 0x1A08F5F4 ;
    canf.exd = 1;
    canf.len = 0;
    InCan1Send(&canf);
}

/**
 * @brief  use this func ,Automatic encoding of broadcast start
 * 
 */
static void token_code_start()
{
    // send can message to detectors start encode
    canf.canid.exdid = TRANS_IN_PACKET ;
    canf.data[0] = 0xCE;
    canf.data[1] = 0x20;
    canf.data[2] = 0x01;
    canf.exd = 1;
    canf.len = 3;
    InCan1Send(&canf);
    runtimeflag = 0;
    
}

static void send_fd_token_replay(void )
{
    /*
    send can message to detectors 
    in order to get a reply from the detector holding the token
    */ 
    canf.canid.exdid = TRANS_IN_PACKET ;
    canf.data[0] = 0xCE;
    canf.data[1] = 0x21;
    canf.exd = 1;
    canf.len = 2;
    InCan1Send(&canf);
    runtimeflag = 0;
}


static void send_coding_fd_id(void )
{

    canf.canid.exdid = TRANS_IN_PACKET ;
    canf.data[0] = 0xCE;
    canf.data[1] = 0x22;
    canf.data[2] = MasterCodingData.CunrrentID;
    canf.exd = 1;
    canf.len = 3;
    InCan1Send(&canf);
    runtimeflag = 0;
    
    /* 回复给上位机 或者 80A*/
    canf.canid.exdid = TRANS_EXP_PACKET;
     
    canf.data[0] = 0xFF;
    canf.data[1] = 0x01;
    canf.data[2] = 0x00;
    canf.data[3] = MasterCodingData.CunrrentID;
    canf.data[4] = 0x00;
    canf.exd = 1;
    canf.len = 5;
    ExCan0Send(&canf);
}

static void Send_token_trans(void)
{
    canf.canid.exdid = TRANS_IN_PACKET ;
    canf.data[0] = 0xCE;
    canf.data[1] = 0x23;
    canf.data[2] = 0x01;
    canf.exd = 1;
    canf.len = 3;
    InCan1Send(&canf);
    runtimeflag = 0;
    
    //判断是否编码完成，未完成则重复之前的步骤
    MasterCodingData.CodeCompletedNumber++;
    // 处理编码进度状态转换
    if (MasterCodingData.CodeCompletedNumber == MasterCodingData.CodeExpectNumber) 
    {
        MasterCodingData.CurrentMode = TOKEN_CODE_FINISH;
    } 
    else 
    {
        MasterCodingData.CurrentMode = BROADCAST_TOKEN_REPLY;
        runtimeflag = 1;
        
        // 特殊中点处理
        if (MasterCodingData.CodeCompletedNumber == MasterCodingData.CodeExpectNumber / 2)
        {
            MasterCodingData.CunrrentID = pSysPara->ucFdSTid + 1;
            return;  // 直接返回避免后续ID修改
        }
    }
        
    //配置的ID根据顺序、逆序进行自增、自减
    if(MasterCodingData.sequence == POSITIVE_SEQUENCE )
    {
        MasterCodingData.CunrrentID += 4;                    // 编码方式按照 每次增加4的方式编码
    }    
}

/**
 * @brief if encode is finish and sucess,then do somthing
 * 
 */
 static void encode_sucess_operation()
 {
     
    canf.canid.exdid = TRANS_EXP_PACKET ;
    canf.data[0] = 0xFF;
    canf.data[1] = 0x01;
    canf.data[2] = 0x00;
    canf.data[3] = 0x00;
    canf.data[4] = 0x00;
    canf.exd = 1;
    canf.len = 5;
    ExCan0Send(&canf);
 }


/**
 * @brief if encode is finish and fail,then do somthing
 * 
 */
 static void encode_fail_operation()
 {
    
    canf.canid.exdid = TRANS_EXP_PACKET ;
    canf.data[0] = 0xFF;
    canf.data[1] = 0x01;
    canf.data[2] = 0x00;
    canf.data[3] = 0x00;
    canf.data[4] = 0x01;
    canf.exd = 1;
    canf.len = 5;
    ExCan0Send(&canf);
 }


/**
 * @brief if encode is finish check whether the encoding is sucessful
 *          sucess or fail send can meaasge to PC
 */
static void exit_coding(void)
{
    if(MasterCodingData.CodeExpectNumber == MasterCodingData.CodeCompletedNumber)
    {
        encode_sucess_operation();
    }
    else
    {
        encode_fail_operation();
    }
    
        FD_exit_coding();
        DioWriteChannel(CODE_OUT,  0);       // 拉低编码线
        encodeswtich = AUTOCODE_CLOSE;
    
          //  delay_ms(5);
        DrvFwdgtInit(10,FWDGT_PSC_DIV64); //测试
}

/**
 * @b+rief the encode result is returned by CAN fram
 * 
 * @param type :ENCODE_RESULT: If a detector is coded ,reply the detector ID to PC
 *              ILLEGAL_PARA :  The parameter passed at the start of encoding is illegal
 */
//static void reply_coding_result(ENU_REPLAY_TYPE type)
//{
//    //给上位机发送完成的编码的探测器
//    canf.canid.exdid = TRANS_EXP_PACKET;
//     
//    canf.data[0] = 0xFF;
//    canf.data[1] = 0x01;
//    canf.data[2] = 0x00;
//    if(MasterCodingData.sequence == POSITIVE_SEQUENCE )
//    {
//        canf.data[3] = MasterCodingData.CunrrentID;
//    }
//    else if(MasterCodingData.sequence == NEGTIVE_SEQUENCE)
//    {
//        canf.data[3] = MasterCodingData.CunrrentID;
//    }
//    canf.data[4] = 0x00;
//    canf.exd = 1;
//    canf.len = 5;
//    ExCan0Send(&canf);    
//}



/**
 * @brief  start auto coding process
 * 
 * @param start_add  the first dector's id
 * @param fd_num    the number of dectors that need to encode
 * @param order     positive sequence = 0  negative order = 1
 */
#include "candebug.h"
BOOL AutoCodeToken_Start(STR_CAN_STD_TYPE *ptr)
{
    if (ptr->J1939PRDP != 0x1A || ptr->J1939PF != 0xD5 ||
        ptr->data[0] != 0xFF || ptr->data[1] != 0x01 ||
        !(ptr->J1939PS == 0xF5 || ptr->J1939PS == pSysPara->ucDeviceID) ||
        ptr->len != 5)
    {
        return FALSE;
    }
    if(CanDebugMode()){
        _doD5SetHandle(ptr);
        return TRUE;
    }

    FD_exit_coding();//探测器退出编码模式，防止异常中断后未退出
    delay_ms(1200);
    FDDebugMode(0xFF);  //探测器进入调试模式
    
    encodeswtich = AUTOCODE_OPEN;
    
    pSysPara->ucModuleNum                   = ptr->data[3];                     //探测器个数
    pSysPara->ucFdSTid                      = ptr->data[2];                     //探测器起始ID
    pSysPara->ucFdEndid                     = ptr->data[2] + ptr->data[3];      //探测器结束ID
    ParaWrite();
    
    MasterCodingData.CurrentMode            = BROADCAST_START_CODE;
    MasterCodingData.CodeCompletedNumber    = 0;
    MasterCodingData.sequence               = ptr->data[4];
    MasterCodingData.EffictiveTime          = 0;
    MasterCodingData.CodeExpectNumber       = ptr->data[3];
    
    DioWriteChannel(CODE_OUT,  1);       // 拉高编码线

    if(MasterCodingData.sequence == POSITIVE_SEQUENCE)
    {
          MasterCodingData.CunrrentID = ptr->data[2];
    }
    else if(MasterCodingData.sequence == NEGTIVE_SEQUENCE)
    {
        MasterCodingData.CunrrentID = ((ptr->data[2] + ptr->data[3])-1);
    }else
    {
        MasterCodingData.CurrentMode = TOKEN_CODE_FINISH;           //TODO:sxy 发送的是错误的数
    }
    
//**********************************************************************************************************
    //进入编码模式的反馈报文
    canf.canid.exdid = TRANS_EXP_PACKET;
    canf.data[0] = 0xFF;
    canf.data[1] = 0x01;
    canf.data[2] = 0x00;
    canf.exd = 1;
    canf.len = 3;
    ExCan0Send(&canf);
    return TRUE;
}

/**
 * @brief The main process of token coding
 * 
 */
void AutoCodeToken_MainProcess (void)
{
    if(encodeswtich == AUTOCODE_CLOSE){
        return;
    }
    switch (MasterCodingData.CurrentMode)
    {
    case BROADCAST_START_CODE:  //0
        token_code_start();
        break;
    case BROADCAST_TOKEN_REPLY:  //1
        send_fd_token_replay();
        break;
    case SET_TOKEN_ID:   //2
        send_coding_fd_id();
        break;
    case TOKEN_TRANS:   //3
        Send_token_trans();
        break;
    case TOKEN_CODE_FINISH:		//4
        exit_coding();
        break;

    default:
        break;
    }
    EffictiveTimeAdd();
}

void AutoCodeToken_DataRecive(STR_CAN_STD_TYPE *ptr)
{
    if (runtimeflag == 0)
    {
        MasterCodingData.EffictiveTime = 0;
        if (MasterCodingData.CurrentMode < TOKEN_CODE_FINISH &&
            MasterCodingData.CurrentMode != TOKEN_TRANS)
        {
            MasterCodingData.CurrentMode++;
        }
    }
    runtimeflag = 1;
}

typedef void (*P_DO_DEBUG_FUNC)(STR_CAN_STD_TYPE *ptr);
typedef struct  
{
    UINT8  DID_H;
    UINT8  DID_M;
	UINT8  DID_L;
    P_DO_DEBUG_FUNC  DID_FUNC; 
} STR_DID ; 
const STR_DID strDID_List_D5[] =
{
	{0XCE, 0X20,0x00, AutoCodeToken_DataRecive},
	{0XCE, 0X21,0x00, AutoCodeToken_DataRecive},
    {0XCE, 0X22,0x00, AutoCodeToken_DataRecive},
    {0XCE, 0X23,0x00, AutoCodeToken_DataRecive},

};  
extern BOOL AutoCodeHandler(STR_CAN_STD_TYPE *ptr)
{

    if (ptr->J1939PRDP != 0x1A || ptr->J1939PF != 0xD5) {
        return FALSE;
    }
    UINT8 i;
    for (i = 0; i < ARRAY_SIZE(strDID_List_D5); i++)    
    {
        if (strDID_List_D5[i].DID_H == ptr->data[0] &&
            strDID_List_D5[i].DID_M == ptr->data[1] &&
            strDID_List_D5[i].DID_L == ptr->data[2])
        {
            strDID_List_D5[i].DID_FUNC(ptr);
            return TRUE;
        }
    }
    return TRUE;
}

void FDDebugMode(uint8_t Id)
{
    canf.canid.exdid = 0x1A04F5F4 ;
    canf.data[0] = Id;
    canf.exd = 1;
    canf.len = 1;
    InCan1Send(&canf);
	delay_ms(10);
    canf.canid.exdid = 0x1A05F5F4 ;
    canf.data[0] = Id;
    canf.exd = 1;
    canf.len = 1;
    InCan1Send(&canf);
	delay_ms(10);
    canf.canid.exdid = 0x1A06F5F4 ;
    canf.data[0] = Id;
    canf.exd = 1;
    canf.len = 1;
    InCan1Send(&canf);
	delay_ms(10);
    canf.canid.exdid = 0x1A07F5F4 ;
    canf.data[0] = Id;
    canf.exd = 1;
    canf.len = 1;
    InCan1Send(&canf);
    delay_ms(10);
}

void FDExDebugMode(void)
{
    canf.canid.exdid = 0x1A08F5F4 ;
    canf.data[0] = 0xFF;
    canf.exd = 1;
    canf.len = 1;
    InCan1Send(&canf);
}

void FDClearError(void)
{
    canf.canid.exdid = 0x1A20F5F4 ;
    canf.data[0] = 0x01;
    canf.exd = 1;
    canf.len = 1;
    InCan1Send(&canf);
}

/**
 * @bref 检测80A在线回复
*/
BOOL CheckOnline(STR_CAN_STD_TYPE *ptr)
{
    if (ptr->J1939PRDP != 0x18 || ptr->J1939PF != 0xCA || ptr->data[0] != 0x11 || ptr->len != 1)
    {
        return FALSE;
    }
    if ((pSysPara->ucDeviceID == ptr->J1939PS || ptr->J1939PS == 0xff))
    {
        canf.canid.exdid = (0x18 << 24) + (0xCA << 16) + (0xEF << 8) + (pSysPara->ucDeviceID); // SA
        canf.data[0] = 0x11;
        canf.exd = 1;
        canf.len = 1;
        ExCan0Send(&canf);
		return TRUE;
    }
    else{
		return FALSE;
	}
}
#endif
