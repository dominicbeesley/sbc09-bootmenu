#ifndef __MEMBUF_H__
#define __MEMBUF_H__	

//read data from src to the buffer
extern void membuf_in(unsigned char *src, unsigned int dest_addr, unsigned int len);
//clear the entire buffer
extern void membuf_clear();
//form a crc16 of the given area of the buffer
extern unsigned int membuf_crc16(unsigned int addr, unsigned int end);
//print information about the buffer contents
extern void membuf_info();

#endif