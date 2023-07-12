#include <stdlib.h>
#include <stdio.h>
#include "uart.h"

int main(void) {

	memset((void *)0x4000,100,100);

	printf("Bob %d %d %d %d", (int)23, (int)44, (int)9, (int)10);

	uart_writec(13);
	uart_writec(10);
	uart_writec('!');

	printf("\nDom\n\n\n");


	uart_writec('$');
	uart_writec('!');

	return -1;
}
