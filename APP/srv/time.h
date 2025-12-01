#ifndef _PTIME_H_
#define _PTIME_H_

#include "stdtype.h"

extern volatile UINT32 tTimeStamp;
extern volatile UINT32 tSysTime;
struct tm_t
{
    INT32 tm_sec;         /* second [0, 59]        */ 
    INT32 tm_min;         /* minute [0, 59]        */
    INT32 tm_hour;        /* hour [0, 23]          */
    INT32 tm_mday;        /* day [1, 31]           */    
    INT32 tm_mon;         /* month [0, 11]         */  
    INT32 tm_year;        /* year [1970, -]        */
    INT32 tm_wday;        /* day in week [0, 6]    */         
    INT32 tm_yday;        /* day in year [0, 365]  */  
};

void local_time(INT32 ts, struct tm_t *ptm);
INT32 sec_time(struct tm_t *ptm);
void SetTime(void);


#endif /* _PTIME_H_ */

