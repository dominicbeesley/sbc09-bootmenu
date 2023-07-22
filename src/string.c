#include <string.h>

int strlen(const char *p) {
	int ret = 0;
	while (*p++ != 0) ret++;
	return ret;
}

void * memset ( void * ptr, int value, size_t num )
{
	unsigned char *p = (unsigned char *)ptr;
	while (num--) {
		*p++=value;
	}
	return ptr;
}

void *memcpy(void *dest, const void * src, size_t num) {
	
	void *odest = dest;

	while (num--) {
		*(char *)dest++	 = *(char *)src++;
	}

	return odest;
}

int memcmp(const void *ptr1, const void *ptr2, size_t num) {
		
	while (num--) {
		int ret;
		ret = (*(unsigned char *)ptr1++) - (*(unsigned char *)ptr2++);
		if (ret != 0)
			return ret;
	}

	return 0;
}

