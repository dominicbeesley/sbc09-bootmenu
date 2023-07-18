#include <stdio.h>
#include "srec.h"
#include "membuf.h"


int srec_nyb(char c) {
	if (c >= '0' && c <= '9')
		return c-'0';
	else if (c >= 'A' && c <= 'F')
		return c-'A'+10;
	else
		return -1;
}

int srec_hex(const char *p) {
	return (srec_nyb(p[0]) << 4) | srec_nyb(p[1]);
}

int srec_read_to_membuf(const char *p) {
	unsigned char buf[64];
	unsigned char srec_type = *p++;
	unsigned char ck;
	int len;
	buf[0] = 0;
	if (!srec_type)
		goto ERROR_BAD;

	len = srec_hex(p);
	if (len < 3 || len > 64)
		goto ERROR_BAD;

	ck = len;
	int l = len;
	unsigned char *b = buf;
	while (l) {
		p+=2;
		int x = srec_hex(p);
		if (x < 0 )
			goto ERROR_BAD;
		*b++ = x;
		ck += x;
		l --;
	}

	if (ck != 0xFF)
		goto ERROR_BAD;

	unsigned int addr = (buf[0] << 8) | buf[1];

	if (srec_type == '9')
	{
		printf("EXEC address : %X\n", addr);
	} else if (srec_type == '1') {
		l = len - 3;
		membuf_in(buf + 2, addr, l);
	}

	return 0;
ERROR_BAD:
	puts("Bad SREC");
	return -1;

}
