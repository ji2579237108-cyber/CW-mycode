#include "c_mem.h"

 void c_memcpy(void *dst, const void *src, unsigned int len)
{
    char *pdst = (char *)dst;
    const char *psrc = (char *)src;
    
    while (len--) {
        *pdst++ = *psrc++;    
    }
}

 void c_bzero(void *dst, unsigned int len)
{
    char *pdst = (char *)dst;
    while (len--) {
        *pdst++ = 0;  
    }
}

 void c_memset(void *dst, int c, unsigned int len)
{
    
    char *pdst = (char *)dst;
    while (len--) {
        *pdst++ = (char)c;
    }       
}

int c_memcmp(const void *dst, const void *src, int count) 
{
    const char *pdst = (char *)dst;
    const char *psrc = (char *)src;
    while (count--) {
        if (*pdst++ != *psrc++) {
            return 1;
        }
    }
    return 0;
}

