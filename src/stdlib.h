#ifndef __STDLIB_H__
#define __STDLIB_H__

typedef int size_t;

extern void * memset ( void * ptr, int value, size_t num );

char *  itoa ( int value, char * str, int base );

#endif