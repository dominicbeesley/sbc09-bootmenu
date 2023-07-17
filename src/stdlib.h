#ifndef __STDLIB_H__
#define __STDLIB_H__

typedef int size_t;

extern void * memset ( void * ptr, int value, size_t num );
extern char *  itoa ( int value, char * str, int base );
extern void *memcpy(void *dest, const void * src, size_t n);

#define NULL 0

#endif