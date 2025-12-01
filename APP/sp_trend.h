/**
 * @file sp_trend.h
 * @author calm
 * @brief 趋势判断
 * @version 0.1
 * @date 2022-08-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _SP_TREND_H_
#define _SP_TREND_H_



/**
 * @brief 趋势判断函数
 * 
 * @param d 数据
 * @param n 数据长度
 * @param rw 稳态值
 * @param s 门限值
 * @return int 趋势值
 */
int sp_trend(float *d, int n, float rw, float s);

#endif //_SP_TREND_H_
