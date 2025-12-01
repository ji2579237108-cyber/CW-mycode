#include "globesta_ctrl.h"
#include "hybuffer.h"

static MODE_CTRL_STR tModeCounter[MASTER_INDEX_MAX] = { 0 }; //模式计数器

volatile static EU_MASTER_CMD gCurMode = INVALID_CMD;//当前模式

static EU_MASTER_CMD emMasterCmd(void);
static void avdFeedBcakCheck(void);

static EU_MASTER_CMD emMasterCmd(void)
{
	if(0x00 == MASETER_CMD_STA)
	{
		return CLOSE_CMD;
	}
	else if(0x01 == MASETER_CMD_STA)
	{
		return OPEN_CMD;
	}
	else
	{
		return INVALID_CMD;
	}
}

static void avdFeedBcakCheck(void)
{
	switch(gCurMode)
	{
		case CLOSE_CMD:
			if(0x01 == FeedBackSta.Byte)
			{
				GLOBE_STA = GLOBE_CLOSE_ED;
				GLOBE_OPEN_STA = GLOBE_OPEN_INVAILD;
				GLOBE_CLOSE_STA = GLOBE_CLOSE_SUCCESS;
				tModeCounter[CLOSE_CMD]._ucModeFlg = 0;
				tModeCounter[CLOSE_CMD]._ucModeCounter = 0;
			}
			else
			{
				if((tModeCounter[CLOSE_CMD]._ucModeCounter > 0) && \
					(0x00 == FeedBackSta.Byte))
				{
					GLOBE_STA = GLONE_CLOSE_ING;
				}
				else if(tModeCounter[CLOSE_CMD]._ucModeCounter > 5)
				{
					GLOBE_CLOSE_STA = GLOBE_CLOSE_FAILT;
					tModeCounter[CLOSE_CMD]._ucModeFlg = 0;
					tModeCounter[CLOSE_CMD]._ucModeCounter = 0;
				}
				else 
				{
					;/* No Action */
				}
			}
			break;
		case OPEN_CMD:
			if(0x02 == FeedBackSta.Byte)
			{
				GLOBE_STA = GLOBE_OPEN_ED;
				GLOBE_OPEN_STA = GLOBE_OPEN_SUCCESS;
				GLOBE_CLOSE_STA = GLOBE_CLOSE_INVAILD;
				tModeCounter[OPEN_CMD]._ucModeFlg = 0;
				tModeCounter[OPEN_CMD]._ucModeCounter = 0;
			}
			else
			{
				if((tModeCounter[OPEN_CMD]._ucModeCounter > 0) && \
					(0x00 == FeedBackSta.Byte))
				{
					GLOBE_STA = GLOBE_OPEN_ING;
				}
				else if(tModeCounter[OPEN_CMD]._ucModeCounter > 5)
				{
					GLOBE_OPEN_STA = GLOBE_OPEN_FAILT;
					tModeCounter[OPEN_CMD]._ucModeFlg = 0;
					tModeCounter[OPEN_CMD]._ucModeCounter = 0;
				}
				else
				{
					;/* No Action */
				}
			}
			break;
		default:
			if(0x01 == FeedBackSta.Byte)
			{
				GLOBE_STA = GLOBE_CONNECT_OK;
				GLOBE_OPEN_STA = GLOBE_OPEN_INVAILD;
				GLOBE_CLOSE_STA = GLOBE_OPEN_INVAILD;
			}
			else if(0x00 == FeedBackSta.Byte)
			{
				GLOBE_STA = GLOBE_CONNECT_NG;
				GLOBE_OPEN_STA = GLOBE_OPEN_INVAILD;
				GLOBE_CLOSE_STA = GLOBE_OPEN_INVAILD;
			}
			else if(0x03 == FeedBackSta.Byte)
			{
				GLOBE_STA = FEEDBACK_CONNECT_DIAG;
				GLOBE_OPEN_STA = GLOBE_OPEN_INVAILD;
				GLOBE_CLOSE_STA = GLOBE_OPEN_INVAILD;
			}
			else
			{
				;/* No Action */
			}
			break;
	}
}

void avdMasterProcess(void)
{
	UINT8 i = 0;
	
	static EU_MASTER_CMD enterMode = INVALID_CMD;
	
	enterMode = emMasterCmd();
	if(enterMode != gCurMode)
	{
		tModeCounter[enterMode]._ucModeFlg = 1;
		gCurMode = enterMode;
	}
	for(i = 0; i < MASTER_INDEX_MAX; i++)
	{
		if(tModeCounter[i]._ucModeFlg == 1)
		{
			tModeCounter[i]._ucModeCounter = tModeCounter[i]._ucModeCounter >= 10 ? 10 : ++tModeCounter[i]._ucModeCounter;
		}
	}
	avdFeedBcakCheck();
}



