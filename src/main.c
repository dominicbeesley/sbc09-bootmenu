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
#include "boot.h"
#include "rdline.h"

extern int default_vectors;

unsigned char general_buf[1024];



int nyb(char c) {
	if (c >= '0' && c <= '9')
		return c-'0';
	else if (c >= 'A' && c <= 'F')
		return c-'A'+10;
	else if (c >= 'a' && c <= 'f')
		return c-'a'+10;
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

void boot(const char *p) {
	unsigned long phys_addr;
	if (gethex_ulong(&p, &phys_addr) < 0)
		goto ERROR;

	do_boot(phys_addr);

ERROR:
	printf ("Bad command: ! <phys_addr> : !%s \n", p);

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
		buf_end += buf_addr-1;

	while (buf_addr != buf_end + 1) {
		unsigned int l;
		if (buf_addr > buf_end) 
			l = sizeof(general_buf);
		else {
			l = buf_end - buf_addr + 1;
			if (l > sizeof(general_buf))
				l = sizeof(general_buf);
		}		
		printf("W:%lx %x %x\n", phys_addr, buf_addr, l);
		membuf_out(general_buf, buf_addr, l);
		memw_in(general_buf, phys_addr, l);
		buf_addr += l;
		phys_addr += l;
	}

	
	

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
		buf_end += buf_addr-1;

	while (buf_addr != buf_end + 1) {
		unsigned int l;
		if (buf_addr > buf_end) 
			l = sizeof(general_buf);
		else {
			l = buf_end - buf_addr + 1;
			if (l > sizeof(general_buf))
				l = sizeof(general_buf);
		}		
		printf("R:%lx %x %x\n", phys_addr, buf_addr, l);
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

void info(void) {

	if (!detected_flash_type) {
		printf("**WARNING** Flash rom type not recognized default to 64K*7 : %02x/%02x\nDefaulting to", flash_det_mid, flash_det_did);
	} else {
		printf("Detected");
	}
	printf(" ROM:\t%12s\nID:\t\t%x/%x\nSize:\t\t%dx%x=%dKB\n",  current_flash_type->name, current_flash_type->manu_id, current_flash_type->dev_id, current_flash_type->sec_size, current_flash_type->n_secs, current_flash_type->sec_size * current_flash_type->n_secs );


	membuf_info();
}

int main(void) {

	puts("\n\nSBC09 - Bootloader\n====================\n");
	// check type of flash rom present
	// map bottom of flash into mmu at 8000
	mmu_16(2) = 0;

	flash_init();
	membuf_clear();
	info();

	while (1) {
		putc('\n', stdout);
		putc(':', stdout);
		if (rdline(general_buf, sizeof(general_buf))) {

			char *p = (char *)general_buf;

			skip_spaces(&p);
			char cmd = toupper(*p++);

			switch (cmd) {
			case 'C':
				fputs("Clearing Buffer...", stdout);
				membuf_clear();
				puts("cleared");
				break;
			case 'D':
				dump(p);
				break;
			case 'S':
				srec_read_to_membuf(p);
				break;
			case '?':
				info();
				break;
			case '!':
				boot(p);
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
