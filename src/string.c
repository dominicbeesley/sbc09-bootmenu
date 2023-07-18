#include <string.h>

int strlen(const char *p) {
	int ret = 0;
	while (*p++ != 0) ret++;
	return ret;
}