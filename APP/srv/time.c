#include "time.h"
#include "RX8010.h"
#include <stdio.h>
#include "c_convert.h"


volatile UINT32 tTimeStamp = 0;
volatile UINT32 tSysTime = 0;
/**
 * Function     : isleapyear 
 * Description  : detect leapyear
 *
 * Parameters   : int year -- the year
 * Return       : 0 -- leapyear
 *                1 -- not leapyear
 *
 * Others       : 
 */
int isleapyear(INT32 year) 
{
    if (((year % 4 == 0) && (year % 100 != 0)) || ((year % 400) == 0))        
        return 1;    
    return 0;
}

/**
 * Function     : sec_time 
 * Description  : local time to the second
 *
 * Parameters   : int32 *sectime --
 *                struct tm_t tm --
 * Return       : int32 -- the second
 *
 * Others       : 
 */
INT32 sec_time(struct tm_t *ptm)
{
    int month_days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    INT32 leapyears;
    INT32 days, secs, hours;
    int i;
                
    leapyears = days = secs = hours = 0;

    for (i = 0; i < ptm->tm_mon; i++)
        days += month_days[i]; /*get days before this month*/

    if (isleapyear(ptm->tm_year + 1970) && (ptm->tm_mon >= 2))
        days++; /*if leapyear, need to add 1*/

    days += ptm->tm_mday - 1; /*delete today*/

    leapyears = (ptm->tm_year + 1) / 4; /*count the number of leapyear from 1970~year*/
    /* leapyears = 1 + (year - 2 - 1) / 4
     * 1 -- one leapyear from 1970 to 1972 
     * 2 -- delete 1970-1971, 1971-1972
     * 1 -- delete one year(tm.tm_year) from year
     */
     days += leapyears * 366;
     days += (ptm->tm_year - leapyears) *365;

     hours += days * 24;
     hours += ptm->tm_hour;

     secs += (hours - 8) * 3600;
     secs += ptm->tm_min * 60;
     secs += ptm->tm_sec;

     return secs;
}

 void local_time(INT32 ts, struct tm_t *ptm)
{
    UINT8  month_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    UINT8  leap_year[4] = {0, 0, 1, 0};
    INT32  four_year_count;
    INT32  i, j;
    INT32  day_cout;
    INT32  leave_for_year_days;
    INT32  leave_for_fouryear;
    INT32  leave_for_month_days;
    UINT32 temp_value;

    ptm->tm_sec = ts % 60;
    temp_value = ts / 60;
    ptm->tm_min = temp_value % 60;
    temp_value /= 60;
    temp_value += 8;
    ptm->tm_hour = temp_value % 24;
    temp_value /= 24;
    ptm->tm_wday = (temp_value + 4) % 7;
    four_year_count = temp_value / (365 * 4 + 1);
    leave_for_fouryear = temp_value % (365 * 4 + 1);
    leave_for_year_days = leave_for_fouryear;
    
    for (i=0; i<4; i++) {
        day_cout = leap_year[i] ? 366 : 365;

        if (leave_for_year_days == day_cout) {
            break;
        } else if (leave_for_year_days < day_cout)
            break;
        else 
            leave_for_year_days -= day_cout;
    }
    ptm->tm_yday = leave_for_year_days;
    
    leave_for_month_days = leave_for_year_days;

    for (j=0; j<12; j++) {
        if ((leap_year[i]) && (j == 1)){
            if (leave_for_month_days < 29)
                break;
            else if (leave_for_month_days == 29) {
                j++;
                leave_for_month_days = 0;
                break;
            } else 
                leave_for_month_days -= 29;
            
            continue;
        }

        if (leave_for_month_days < month_days[j])
            break;
        else if (leave_for_month_days == month_days[j]){
            j++;
            leave_for_month_days = 0;
            break;
        } else {
            leave_for_month_days -= month_days[j];
        }
    }
    
    if (j == 12) {
        i++;
        j = 0;
    }
    ptm->tm_year = four_year_count * 4 + i;
    ptm->tm_mday = leave_for_month_days + 1;
    ptm->tm_mon = j;
    
}



void SetTime(void)
{
    struct tm_t tm;

    UINT8 year, mon, mday, hour, min, sec;  
    
    RX8010_ReadYearWithBCD(&year);
    RX8010_ReadMonWithBCD(&mon);
    RX8010_ReadMdayWithBCD(&mday);
    RX8010_ReadHourWithBCD(&hour);
    RX8010_ReadMinWithBCD(&min);
    RX8010_ReadSecWithBCD(&sec);

    tm.tm_year = bcd_to_dec(year) + 30;
    tm.tm_mon  = bcd_to_dec(mon) - 1;
    tm.tm_mday = bcd_to_dec(mday);
    tm.tm_hour = bcd_to_dec(hour);
    tm.tm_min  = bcd_to_dec(min);
    tm.tm_sec  = bcd_to_dec(sec);
    
    tTimeStamp = sec_time(&tm);    
}

