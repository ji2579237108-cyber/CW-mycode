/**
 * @file cbits.c
 * @author liigo
 * @brief 
 * @version 0.1
 * @date 2020-9-26
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "stdint.h"
#include "cbits.h"

union bytebits_t {
    uint8_t u8;
    struct {
        uint8_t bit0:1, bit1:1, bit2:1, bit3:1;
        uint8_t bit4:1, bit5:1, bit6:1, bit7:1;
    } bits;
};

int bit_at(uint8_t *bytes, int i) {
#if 0 
    union bytebits_t bb = { .u8 = bytes[i / 8] };
#else
    union bytebits_t bb;
    bb.u8 = bytes[i / 8];
#endif
    i = i % 8;
    switch(i) {
        case 0: return bb.bits.bit0; case 1: return bb.bits.bit1; 
        case 2: return bb.bits.bit2; case 3: return bb.bits.bit3;
        case 4: return bb.bits.bit4; case 5: return bb.bits.bit5; 
        case 6: return bb.bits.bit6; case 7: return bb.bits.bit7;
    }
    return 0;
}

void bit_set(uint8_t *bytes, int pos, uint8_t  bit) {
    union bytebits_t *pbb = (union bytebits_t*)&bytes[pos / 8];
    pos = pos % 8;
    switch(pos) {
    case 0: pbb->bits.bit0 = bit; break; 
    case 1: pbb->bits.bit1 = bit; break; 
    case 2: pbb->bits.bit2 = bit; break; 
    case 3: pbb->bits.bit3 = bit; break;
    case 4: pbb->bits.bit4 = bit; break; 
    case 5: pbb->bits.bit5 = bit; break; 
    case 6: pbb->bits.bit6 = bit; break; 
    case 7: pbb->bits.bit7 = bit; break;
    }
}

