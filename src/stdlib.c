#include <stdlib.h>

void * memset ( void * ptr, int value, size_t num )
{
	unsigned char *p = (unsigned char *)ptr;
	while (num--) {
		*p++=value;
	}
	return (void *)p;
}

void *memcpy(void *dest, const void * src, size_t num) {
	
	void *odest = dest;

	while (num--) {
		*(char *)dest++	 = *(char *)src++;
	}

	return odest;
}

char digit(unsigned int v, unsigned int base) {
	unsigned char d = v % base;
	if (d <= 9)
		return '0' + d;
	else
		return 'A' + d - 10;
}

static void pushch(char * str, char c) {
	char *p = str;
	while (c) {
		char o = *p;
		*p++ = c;
		c = o;
	};
	*p = 0;
}

char *itoa ( int value, char * str, int base ) {
	//simplistic
	str[0] = 0;
	int minus = 0;
	unsigned int uvalue = value;
	if (value < 0 && base == 10) {
		uvalue = -value;
		minus = 1;
	}
	do {
		char c = digit(uvalue, base);
		pushch(str, c);
		uvalue = uvalue / base;

	} while (uvalue != 0);
	if (minus)
		pushch(str, '-');
	return str;
}

