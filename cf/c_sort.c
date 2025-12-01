#include "c_sort.h"

/*****************************************************
函数功能：冒泡排序
参数说明：arr 要排序的数据，size 数据个数
           flag=0 正序，flag=1 反序
函数返回：无
*****************************************************/
void bubble_sort(UINT16 *arr, UINT16 count, UINT8 flag)
{   
	UINT16 i, j, temp;
	
    for (i = 0; i < count-1; i++) {
        for (j = 0; j <count-i-1; j++) {
            if (flag == 0 && arr[j] > arr[j+1] ||\
			    flag == 1 && arr[j] < arr[j+1]) {
			    temp = arr[j+1];
			    arr[j+1]= arr[j];
			    arr[j] = temp;
			}
        }
    }
}
