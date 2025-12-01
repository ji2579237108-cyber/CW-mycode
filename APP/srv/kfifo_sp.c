#include "kfifo_sp.h"
#include "c_mem.h"

#ifndef min
#define min(x,y) ((x)<(y)?(x):(y))
#endif

/*****************************************
*函数声明
*****************************************/
static void kfifo_copy_in(struct __kfifo *fifo, const void *src, unsigned int len, unsigned int off);
unsigned int __kfifo_out_peek(struct __kfifo *fifo, void *buf, unsigned int len);

#if 0
static inline unsigned int roundup_pow_of_two(unsigned int v);
static unsigned int rounddown_pow_of_two(unsigned int n);
#endif

/*******************************************************************
函数名称：kfifo_unused
函数原型：static inline unsigned int kfifo_unused(struct __kfifo *fifo)
功能描述：计算fifo中未使用的元素数量
输入1   ：缓冲区结构体指针
返回值  ：fifo中未使用的元素数量
补充信息：无  
*********************************************************************/

static inline unsigned int kfifo_unused(struct __kfifo *fifo)
{
	return (fifo->mask + 1) - (fifo->in - fifo->out);
}


/*******************************************************************
函数名称：kfifo_in
函数原型：unsinged int kfifo_in(struct __kfifo *fifo,const void *buf, unsigned int len)
功能描述：向数据缓冲区写入多个数据
输入1   ：缓冲区结构体指针
输入2   ：数据指针
输入3   ：读取数据个数
返回值  ：读取成功个数
补充信息：无  
*********************************************************************/
unsigned int __kfifo_in(struct __kfifo *fifo, const void *buf, unsigned int len)
{
	unsigned int l;

	l = kfifo_unused(fifo);
	if (len > l)
		len = l;

	kfifo_copy_in(fifo, buf, len, fifo->in);
	fifo->in += len;
	return len;
}

/*******************************************************************
函数名称：kfifo_out
函数原型：unsigned int kfifo_out(struct __kfifo *fifo, void *buf, unsigned int len)
功能描述：向数据缓冲区写入多个数据
输入1   ：缓冲区结构体指针
输入2   ：数据指针
输入3   ：读取数据个数
返回值  ：读取成功个数
补充信息：无  
*********************************************************************/
unsigned int __kfifo_out(struct __kfifo *fifo, void *buf, unsigned int len)
{
	len = __kfifo_out_peek(fifo, buf, len);
	fifo->out += len;
	return len;
}





/**********************************************
 * FIFO输入 内部函数
 **********************************************/
static void kfifo_copy_in(struct __kfifo *fifo, const void *src, unsigned int len, unsigned int off)
{
	unsigned int size = fifo->mask + 1;
	unsigned int esize = fifo->esize;
	unsigned int l;

	off &= fifo->mask;
	if (esize != 1) {
		off *= esize;
		size *= esize;
		len *= esize;
	}
	l = min(len, size - off);

	memcpy((fifo->data + off), src, l);
	memcpy(fifo->data, src + l, len - l);
	/*
	 * make sure that the data in the fifo is up to date before
	 * incrementing the fifo->in index counter
	 */
}


/**************************************************
 * FIFO输出  内部函数
 ****************************************************/
static void kfifo_copy_out(struct __kfifo *fifo, void *dst, unsigned int len, unsigned int off)
{
	unsigned int size = fifo->mask + 1;
	unsigned int esize = fifo->esize;
	unsigned int l;

	off &= fifo->mask;
	if (esize != 1) {
		off *= esize;
		size *= esize;
		len *= esize;
	}
	l = min(len, size - off);

	memcpy(dst, fifo->data + off, l);
	memcpy(dst + l, fifo->data, len - l);
	/*
	 * make sure that the data is copied before
	 * incrementing the fifo->out index counter
	 */
}

unsigned int __kfifo_out_peek(struct __kfifo *fifo, void *buf, unsigned int len)
{
	unsigned int l;

	l = fifo->in - fifo->out;
	if (len > l)
		len = l;

	kfifo_copy_out(fifo, buf, len, fifo->out);
	return len;
}
