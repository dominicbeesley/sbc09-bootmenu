#include <stdio.h>
#include <stdlib.h>
#include "rdline.h"
#include "uart.h"

//similar to OSWORD 0, automatically limits chars to >' ' <0x7F
extern char *rdline(char * buf, int maxlen) {

	char *p = buf;
	int l = 0;
	int c;

	while (1) {
		c = uart_readc();
		if (c == -1)
			break;
		else if (c == 8 || c == 127)
		{
			if (l == 0)
				uart_writec('\a');
			else {
				p--;
				l--;
				uart_writec(8);
				uart_writec(' ');
				uart_writec(8);
			}
		} else if (c == '\r' || c == '\n') {
			break;
		} else if (c == 3) {
			//ctrl-c exit
			return NULL;
		} else if (c >= ' ' && c < 0x7F) {
			if (l > maxlen - 1) 
			{
				uart_writec(7);
			} else {
				*p++ = c;
				l++;
				uart_writec(c);
			}
		}
	}
	buf[l] = 0;
	return buf;	

}