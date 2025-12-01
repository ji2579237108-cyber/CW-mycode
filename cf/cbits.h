#ifndef _CBITS_H_
#define _CBITS_H_

/**
 * @file cbits.h
 * @author liigo
 * @brief 
 * @version 0.1
 * @date 2020-9-23
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "swp_incl.h"

#ifdef __cplusplus
extern "C" {
#endif

int bit_at(uint8_t *bytes, int i);
void bit_set(uint8_t *bytes, int pos, uint8_t  bit);


#ifdef __cplusplus
}
#endif

#endif // _CBITS_H_
