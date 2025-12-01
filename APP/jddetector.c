#include "jddetector.h"
#include "para.h"
#include "ifr.h"
static STR_JD_DETECTOR stJdDetectors[DETECTOR_NBR] = { 0 };

void JdCommDetect(void) {
    UINT8 ucIx;
    for (ucIx = 0; ucIx < pSysPara->ucModuleNum; ucIx++) {
        STR_JD_DETECTOR *p = JdDetetorPtr(ucIx + 1);
        if (p != NULL) {
            JdDetecotrCommDetect(p);
        }
    }    
}
void JdDetecotrAsk (void) {
    STR_CAN_STD_TYPE stCanIn;
    stCanIn.exd = 1;
    stCanIn.len = 8;
    stCanIn.CANEXDID = 0x0050FFF4;
    stCanIn.data[0] = 0x00;
    stCanIn.data[1] = 0x00;
    stCanIn.data[2] = 0x00;
    stCanIn.data[3] = 0x00;
    stCanIn.data[4] = 0x00;
    stCanIn.data[5] = 0x00;
    stCanIn.data[6] = 0x00;
    stCanIn.data[7] = 0x00;
    _IFR_CAN_IN_SEND(&stCanIn);
}
void JdDetecotrCommDetect (STR_JD_DETECTOR *jdptr)
{

    CTR_ADD(jdptr->ucOfflineCtr, 0xFFU);
    if (jdptr->ucOfflineStatus != 1 && jdptr->ucOfflineCtr > JD_OFFLINE_SEC)
    {
        jdptr->ucOfflineStatus = 1;
        JdDtectorOfflineSet(jdptr);
    }
    if (jdptr->ucOfflineStatus == 1 && jdptr->ucOfflineCtr <= JD_OFFLINE_SEC)
    {
        jdptr-> ucOfflineStatus = 0;
    }
}
void JdDtectorOfflineSet (STR_JD_DETECTOR *jdptr)
{
    jdptr->coAlarmKind = ALARM_NOR;
    jdptr->tempAlarmKind = ALARM_NOR;
    jdptr->stCoTicks.ucLowAlarmticks = 0;
    jdptr->stCoTicks.ucHighAlarmticks = 0;
    jdptr->stTempTicks.ucLowAlarmticks = 0;
    jdptr->stTempTicks.ucHighAlarmticks = 0;    
}
STR_JD_DETECTOR *JdDetetorPtr (UINT8 ucId)
{
    if (ucId > 0 && ucId <= DETECTOR_NBR)
    {
		return &stJdDetectors[ucId - 1];
	} 
    else 
    {
		return NULL;
	}
}
void  JdDetectorsInit(void)
{
    UINT8 i,j;
    for(i = 0; i < DETECTOR_NBR; i++) {
        for(j = 0; i < 10; j++) {
            stJdDetectors[i].ssTempBuf[j] = 0;        
        }
        
    }
    
}