#ifndef _C_MEM_H_
#define _C_MEM_H_



void c_memcpy(void *dst, const void *src, unsigned int len);
void c_bzero(void *dst, unsigned int len);
void c_memset(void *dst, int c, unsigned int len);
int c_memcmp(const void *dst, const void *src, int count);
#endif
