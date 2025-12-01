#include "jdcanhandle.h"
#include "jddetector.h"
#include "ifr.h"
#include "para.h"
#include "sp_trend.h"
#include "MidCan.h"
BOOL JdCanParse (STR_CAN_STD_TYPE *canf)
{
    if (canf->J1939PRDP == 0x00U) {
        STR_JD_DETECTOR  *ptr = JdDetetorPtr(canf->J1939SA);
        if (ptr != NULL) {
            switch (canf->J1939PF) {
            case 0x51:
            
                Jd51CanParse(canf, ptr);
                ptr->tempAlarmKind = AlgoTemp(ptr->ssTemp, &ptr->stTempTicks);
                JdTempAppend(((float)ptr->ssTemp),ptr->ssTempBuf);
                ptr->ucTrend = sp_trend(ptr->ssTempBuf,10,60,0.62);                
                break;
            case 0x52:
                
                ptr->coAlarmKind = AlgoCo(ptr->usCo, &ptr->stCoTicks);
                Jd52CanParse(canf, ptr);
                break;
            case 0x54:
                Jd54CanParse(canf, ptr);
                break;
            case 0x69:
                Jd69CanParse(canf);
            default:
                break;
            }
        }
        return TRUE;
    } else {
        return FALSE;
    }
}
void Jd69CanParse(STR_CAN_STD_TYPE *canf) {
    STR_CAN_STD_TYPE stCanEx;
    stCanEx.exd = 1;
    stCanEx.len = 8;
    stCanEx.CANEXDID = 0x18DBE900 + pSysPara->ucDeviceID;
    stCanEx.data[0] = canf->J1939SA;
    stCanEx.data[1] = 0x01;
    stCanEx.data[2] = 0x5A;
    stCanEx.data[3] = 0x5A;
    stCanEx.data[4] = 0x5A;
    stCanEx.data[5] = 0x5A;
    stCanEx.data[6] = 0x5A;
    stCanEx.data[7] = 0x5A;
    ExCan0Send(&stCanEx); 
}
void ValeCtr(UINT8 addr,STR_JD_DETECTOR *dptr)
{
    STR_CAN_STD_TYPE stCanIn;
    if (dptr->tempAlarmKind == ALARM_LOW || dptr->tempAlarmKind == ALARM_HIGH) {                     
                stCanIn.CANEXDID = 0x00680000 + (addr << 8) + 0xF4;
                CanInMakeFrame(&stCanIn);
                InCan1Send(&stCanIn);   
            }
}


void Jd51CanParse (STR_CAN_STD_TYPE *canf, STR_JD_DETECTOR *jdptr)
{
    jdptr->ssTemp  = canf->data[0] * 256 + canf->data[1];
    jdptr->ssTemp1 = canf->data[2] * 256 + canf->data[3];
    jdptr->ssTemp2 = canf->data[4] * 256 + canf->data[5];
    jdptr->ssTemp3 = canf->data[6] * 256 + canf->data[7];
    
    jdptr->ucOfflineCtr = 0;

}

void Jd52CanParse (STR_CAN_STD_TYPE *canf, STR_JD_DETECTOR *jdptr)
{
    jdptr->usCo= canf->data[0] * 256 + canf->data[1];
    jdptr->ucOfflineCtr = 0;
}

void Jd54CanParse (STR_CAN_STD_TYPE *canf, STR_JD_DETECTOR *jdptr)
{
    jdptr->ucTempError= canf->data[0];
    jdptr->ucCoError= canf->data[1];
    jdptr->ucOfflineCtr = 0;
}

void JdTempBufInit(void)
{
    UINT8 i,j;
    STR_JD_DETECTOR  *ptr;
    for (i = 0; i < 8; i++)
    {
        ptr = JdDetetorPtr(i);
        for (j = 0; j < 10; j++) 
        {
           ptr->ssTempBuf[j] = 0; 
        }        
    }
}

void JdTempAppend(float temp, float *p)
{
    UINT8 i;
    for (i = 0; i < 9; i++) 
    {
        *(p + i) = *(p + i + 1);        
    }
    *(p + 9) = temp;        
}

