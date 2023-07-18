#ifndef __MEMW_H__
#define __MEMW_H__	

//read data from src to the buffer
extern void memw_in(unsigned char *src, unsigned long dest_addr, unsigned int len);
//read data from the buffer to dest
extern void memw_out(unsigned char *dest, unsigned long src_addr, unsigned int len);

#endif