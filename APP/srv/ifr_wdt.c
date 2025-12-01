#include "ifr_wdt.h"
#include "watchdog.h"

const IRF_WDT irfWdt = {  _SWP_PORTING_
    WDTInit,
    WDTEnable,
    WDTFeed,
    WDTDisable,
    WDTStop,
};
