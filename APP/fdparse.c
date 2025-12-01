#include "cf.h"
#include "fdparse.h"
#include "Appconf.h"
#include "fd.h"
#include "para.h"
#include "fd_update.h"
#include "MidCan.h"

BOOL FdParse(STR_CAN_STD_TYPE *canf) 
{
    if (gucFdUpdateFllag != TRANSPARENT_DISABLE) return FALSE;
    if (canf->J1939PS != 0xF6 && canf->J1939PS != 0xE0 
        && canf->J1939PS != 0xA1 && canf->J1939PS != 0xF4) 
    {
        return FALSE;
    }
    switch (canf->J1939PF)
    { 
        case 0xB0:
        case 0xBA:  
            FdB0hHandler(canf);
            return TRUE;
            break;
        case 0x93:
            Fd93hHandler(canf);
            return TRUE;
            break;
    }
    
    return FALSE;
}

