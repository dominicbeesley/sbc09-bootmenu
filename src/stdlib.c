#include <stdlib.h>

void * memset ( void * ptr, int value, size_t num )
{
	unsigned char *p = (unsigned char *)ptr;
	while (num--) {
		*p++=value;
	}
	return (void *)p;
}

char digit(int v, int base) {
	v = v % base;
	if (v <= 9)
		return '0' + v;
	else
		return 'A' + v - 10;
}

char *itoa ( int value, char * str, int base ) {
	//simplistic
	str[0] = 0;
	do {
		char c = digit(value, base);
		char *p = str;
		while (c) {
			char o = *p;
			*p++ = c;
			c = o;
		};
		*p = 0;

		value = value / base;

	} while (value > 0);
	return str;
}