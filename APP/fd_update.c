#include "fd_update.h"
#include "fd.h"
#include "candebug.h"
#include "para.h"
#include "Appconf.h"
#include "MidCan.h"
uint8_t gucFdUpdateFllag = TRANSPARENT_DISABLE;  //TODO:sxy

const uint8_t ucBootIds[] =
{
        0x50,
        0x96,
        0xD1,
        0xE3,
        0xE4,
        0xE5,
        0xE7,
        0xEB,
        0xF1,
        0xF5,
        0xF6,
        0xFB,
        0xFC,
        0xFD,
        0xEA,
        0xFA,
};

const uint8_t ucMaxDate = sizeof(ucBootIds) / sizeof(ucBootIds[0]);

static BOOL IsBootId(uint8_t value)
{
    for (int i = 0; i < ucMaxDate; i++)
    {
        if (value == ucBootIds[i])
        {
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * @brief Receive host data OutCan
 *
 * @param canf Can Data
 * @return BOOL
 */
extern BOOL FdUpdateMain(STR_CAN_STD_TYPE *canf)
{
    if (canf->J1939PRDP != 0x1B || canf->J1939SA != 0xF4 || gucFdUpdateFllag == TRANSPARENT_DISABLE)
    {
        return FALSE;
    }

    if (!IsBootId(canf->J1939PS))
    {
        return FALSE;
    }
    switch (canf->J1939PF)
    {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
            InCan1Send_UpGrade(canf);   //TODO:sxy  
        break;
        default:
            return FALSE;
    }
    return TRUE;
}

/**
 * @brief Receive detector update data InCan
 *
 * @param canf
 */
extern BOOL RecvFdData(STR_CAN_STD_TYPE *canf)
{
    if ((canf->J1939PRDP != 0x1B || canf->J1939PS != 0xF4) ||
        (gucFdUpdateFllag == TRANSPARENT_DISABLE))
    {
        return FALSE;
    }
    if (!IsBootId(canf->J1939SA))
    {
        return FALSE;
    }
    switch (canf->J1939PF)
    {
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
        ExCan0Send(canf);
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

/**
 * @brief enable detector update moudle OutCan
 *
 * @param canf
 * @return BOOL
 */
extern BOOL Transparent(STR_CAN_STD_TYPE *canf)
{
    STR_CAN_STD_TYPE TxCanf;
    if ((canf->J1939PRDP != 0x1A || canf->J1939PF != 0xEE || canf->J1939PS != 0xF5 ||
         canf->data[0] != 0x96 || canf->len != 5) ||
        (canf->J1939SA != pSysPara->ucDeviceID && canf->J1939SA != 0xFF))
    {
        return FALSE;
    }
    switch (canf->data[4])
    {
    case 0x00: // ²éÑ¯
        break;
    case 0x01: // ÉèÖÃ
    case 0x02:
        gucFdUpdateFllag = canf->data[4] - 1;
        break;
    default:
        return FALSE;
    }
    memcpy(&TxCanf, canf, sizeof(STR_CAN_STD_TYPE));
    TxCanf.data[0] = gucFdUpdateFllag;
    TxCanf.len = 1;
    TxCanf.J1939PS = pSysPara->ucDeviceID;
    TxCanf.J1939SA = canf->J1939PS;
    ExCan0Send(&TxCanf);

    return TRUE;
}

/**
 * @brief transpare Debug Data. InCan
 *
 * @param canf
 * @return BOOL
 */
extern BOOL TransparentDebug(STR_CAN_STD_TYPE *canf) 
{
    if (gucFdUpdateFllag == TRANSPARENT_DISABLE || canf->J1939PRDP != 0x1A)
    {
        return FALSE;
    }
    else{
        ExCan0Send(canf);
    }
    return TRUE;
}

/**
 * @brief Receive version data
 *
 * @param canf Can Data
 * @return BOOL
 */
BOOL FdVersion(STR_CAN_STD_TYPE *canf)
{
    if (canf->J1939PRDP != 0x1Au || gucFdUpdateFllag == TRANSPARENT_DISABLE)
	{
         return FALSE;
    }
    else{
        InCan1Send_UpGrade(canf);
    }
    return TRUE;
}


