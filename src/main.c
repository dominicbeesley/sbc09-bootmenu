#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "uart.h"
#include "hardware.h"
#include "memmap.h"
#include "flash.h"
#include "membuf.h"
#include "memw.h"
#include "srec.h"

extern int default_vectors;

unsigned char general_buf[1024];



int nyb(char c) {
	if (c >= '0' && c <= '9')
		return c-'0';
	else if (c >= 'A' && c <= 'F')
		return c-'A'+10;
	else
		return -1;
}

void skip_spaces(const char **p) {
	while (**p == ' ') (*p)++;
}

int gethex_ulong(const char **p, unsigned long *ret) {

	skip_spaces(p);

	*ret = 0;
	int val = -1;

	int v;
	while((v = nyb(**p)) >= 0) {
		(*p)++;
		
		*ret = (*ret << 4) | v;
		val = 0;
	}

	return val;
}

int gethex_uint(const char **p, unsigned int *ret) {

	skip_spaces(p);

	*ret = 0;
	int val = -1;
	
	int v;
	while((v = nyb(**p)) >= 0) {
		(*p)++;
		
		*ret = (*ret << 4) | v;
		val = 0;
	}

	return val;
}


void write_mem(const char *p) {
	unsigned long phys_addr;
	unsigned int buf_addr;
	unsigned int buf_end;
	unsigned char plus = 0;

	if (gethex_ulong(&p, &phys_addr) < 0)
		goto ERROR;

	if (gethex_uint(&p, &buf_addr) < 0)
		goto ERROR;

	skip_spaces(&p);
	if (*p == '+') 
	{
		plus = 1;
		p++;
	}

	if (gethex_uint(&p, &buf_end) < 0)
		goto ERROR;

	if (plus)
		buf_end += buf_addr;

	
	
	

	return;

ERROR:
	printf ("Bad command: W <phys_addr> <bufstart> [+<len>|<end>] : !%s \n", p);

}

void read_mem(const char *p) {
	unsigned long phys_addr;
	unsigned int buf_addr;
	unsigned int buf_end;
	unsigned char plus = 0;

	if (gethex_ulong(&p, &phys_addr) < 0)
		goto ERROR;

	if (gethex_uint(&p, &buf_addr) < 0)
		goto ERROR;

	skip_spaces(&p);
	if (*p == '+') 
	{
		plus = 1;
		p++;
	}

	if (gethex_uint(&p, &buf_end) < 0)
		goto ERROR;

	if (plus)
		buf_end += buf_addr;

	while (buf_addr != buf_end) {
		unsigned int l;
		if (buf_addr > buf_end) 
			l = sizeof(general_buf);
		else {
			l = buf_end - buf_addr;
			if (l > sizeof(general_buf))
				l = sizeof(general_buf);
		}		
		printf("%lx %x %x", phys_addr, buf_addr, l);
		memw_out(general_buf, phys_addr, l);
		membuf_in(general_buf, buf_addr, l);
		buf_addr += l;
		phys_addr += l;
	}

	return;

ERROR:
	printf ("Bad command: R <phys_addr> <bufstart> [+<len>|<end>] : !%s \n", p);

}

unsigned int dump_addr = 0;
void dump(const char *p) {
	unsigned int start;
	if (gethex_uint(&p, &start) >= 0) {
		dump_addr = start;
	}


	unsigned int end = (dump_addr + 0x100) & 0xFF00;
	unsigned char first = 1;

	printf("%x %x\n", dump_addr, end);

	while (dump_addr != end) {

		//print address
		printf("\n%04X :", dump_addr & 0xFFF0);

		int skip = dump_addr & 0xF;

		//pad with spaces if not on para boundary
		for (int x = 0; x < skip; x++)
			fputs("   ", stdout);

		int n = 16 - skip;
		if (end > dump_addr && end - dump_addr < n)
			n = end - dump_addr;

		membuf_out(general_buf + (dump_addr & 0xF), dump_addr, n);

		for (int i = 0 ; i < n ; i++)
			printf(" %02X", general_buf[i + skip]);

		fputs(" | ", stdout);
		for (int x = 0; x < skip; x++)
			putc(' ', stdout);
		for (int i = 0 ; i < n ; i++)
		{	
			unsigned char c = general_buf[i + skip];
			putc((c < 32 || c > 127)?'.':c, stdout);
		}

		dump_addr += n;		
	}

	putc('\n', stdout);

}

int main(void) {

	// setup channel A of UART for 115200 and 16K MMU enabled
	uart_init();

	// map topmost RAM block as read/write at top 8000 and ready it with vectors etc, 
	// we treat this as our own private workspace
	// first map in at 8000 and setup vectors
	mmu_16(MMU_IX_WINDOW) = MMU_SEL_RAM | MMU_MEM_MAX;
	memcpy((void *)((VECTOR_LOC & 0x3FFF) | 0x8000), &default_vectors, 16);
	// and now map it in at C000
	mmu_16(MMU_IX_MOS) = MMU_SEL_RAM_RO | MMU_MEM_MAX;

	//enable interrupts
	__asm("\tandcc	#0xAF");


	*((int*)0x8000) = 0xBEEF;
	printf("TEST: %x\n", *((int*)0x8000));
	printf("TEST: %x\n", *((int*)0xC000));
	printf("TEST: %x\n", *((int*)0xB7FA));
	printf("TEST: %x\n", *((int*)0xF7FA));

	puts("SBC09 :\t\tBootloader\n");
	// check type of flash rom present
	// map bottom of flash into mmu at 8000
	mmu_16(2) = 0;

	flash_init();
	if (!detected_flash_type) {
		fputs("**WARNING** Flash rom type not recognized default to 64K*7", stdout);
	} 
	printf("Flash ROM:\t%x/%x:%8s\n", current_flash_type->manu_id, current_flash_type->dev_id, current_flash_type->name);
	

	membuf_clear();

	while (1) {
		putc(':', stdout);
		if (fgets(general_buf, sizeof(general_buf), stdin)) {

			char *p = (char *)general_buf;

			skip_spaces(&p);
			char cmd = toupper(*p++);

			switch (cmd) {
			case 'C':
				membuf_clear();
				break;
			case 'D':
				dump(p);
				break;
			case 'S':
				srec_read_to_membuf(p);
				break;
			case '?':
				membuf_info();
				break;
			case 'W':
				write_mem(p);
				break;
			case 'R':
				read_mem(p);
				break;
			default:
				printf("\nUnrecognized command %c %x\n", (int)cmd, (int)cmd);
				break;			
			}
		} 
	}
	

	return 0;
}
