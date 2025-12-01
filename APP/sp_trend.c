/**
 * @file sp_trend.c
 * @author calm
 * @brief 趋势判断
 * @version 0.1
 * @date 2022-08-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "sp_trend.h"

static int sgn1(float x, float s)
{
    if (x > s)          return 1;
    else if (x < -s)    return -1;
    else                return 0;
}

static int sgn2(int x)
{
    if (x > 1)          return 1;
    else if (x < -1)    return -1;
    else                return 0;
}

/**
 * @brief 趋势判断函数
 * 
 * @param d 数杮
 * @param n 数杮长度
 * @param rw 稳思值
 * @param s 门陝值
 * @return int 趋势值
 */
int sp_trend(float *d, int n, float rw, float s)
{
    int i, j, sum = 0;

    for (i = 0; i < n-1; i++) {
        for (j = 1; j < n; j++) {
            sum += sgn2(sgn1((d[n-1-i] - d[n-1-j]), s) + \
                    sgn1((d[n-1-j] - rw), s));
        }
    }
    return sum;  
}
