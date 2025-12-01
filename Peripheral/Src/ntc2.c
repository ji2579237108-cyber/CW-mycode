#include "ntc.h"

static kb_t fun(float x1, float y1, float x2, float y2)
{
    kb_t kb;
    kb.k = (y2 - y1) / (x2 - x1);
    kb.b = y1 - kb.k * x1;
    return kb;
}
 

void NTC2GetValue(void* pDat)
{
    UINT8 i;
    float f = 0;
    kb_t kb;
    UINT16 NtcTabSize;
    
    NtcTabSize = get_ntc_tab_size();
    
	f = AdcDmaValue(SAMPLE_NTC2);
	f = f/1000;

    if (f > 3.29f) 
    {
        f = 3.29f;  
    }
    f = f * 10 / (3.3f - f);
    if (f <= _ntc_tab[NtcTabSize-1][1])
    {
        f = 125;
    }
    else if (f >= _ntc_tab[0][1])
    {
        f = -40;
    }
    else
    {
        for (i = 0; i < NtcTabSize; i++) {
            if (f >= _ntc_tab[i][1]) {
                //f = -40.0 + i;
                if (i == 0)
                {
                    f = -40;
                    *((float*)pDat) = f;
                    return;
                }
                kb = fun(_ntc_tab[i][1], -40+i, _ntc_tab[i-1][1], -41+i);
                f = f * kb.k + kb.b;
                break;
            }
        } 
    }
                   
    *((float*)pDat) = f; 
}



