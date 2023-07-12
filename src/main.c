#include <stdlib.h>
#include <stdio.h>

int main(void) {

	memset((void *)0x4000,100,100);

	printf("Bob %d %s", (int)23, "hhpp[");

	return -1;
}
