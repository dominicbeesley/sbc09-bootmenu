#ifndef __STRING_H__
#define __STRING_H__

typedef int size_t;

int strlen(const char *);
extern void *memcpy(void *dest, const void * src, size_t n);
extern int   memcmp(const void *ptr1, const void *ptr2, size_t n);

#endif