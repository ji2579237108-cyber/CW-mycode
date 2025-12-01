#include "saecrc.h"

 UINT8 SaeCrcCal(void *ptr, int count)
{
    int i, j;
    UINT8 ucCrc;
    UINT8 ucPoly;

    UINT8 *p = (UINT8 *)ptr;
    ucCrc = 0xff;
    ucPoly = 0x1d;

    for (i = 0; i < count; i++)
    {
        ucCrc ^= p[i];

        for (j = 0; j < 8; j++)
        {
            if (ucCrc & 0x80)
            {
                ucCrc = (ucCrc << 1) ^ ucPoly;
            }
            else
            {
                ucCrc <<= 1;
            }
        }
    }

    ucCrc ^= (UINT8)0xff;
    return ucCrc;
}


