#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "uart.h"
#include "hardware.h"
#include "memmap.h"
#include "flash.h"
#include "membuf.h"
#include "memw.h"
#include "srec.h"
#include "boot.h"
#include "rdline.h"
#include "commonmem.h"

extern int default_vectors;

unsigned char ram_limit = 0; // this is an mmu index in the form %10xxxxxx when xxxxxx contains the highest RAM block

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
			l = GENERAL_BUF_SIZE;
		else {
			l = buf_end - buf_addr + 1;
			if (l > GENERAL_BUF_SIZE)
				l = GENERAL_BUF_SIZE;
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
			l = GENERAL_BUF_SIZE;
		else {
			l = buf_end - buf_addr + 1;
			if (l > GENERAL_BUF_SIZE)
				l = GENERAL_BUF_SIZE;
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

int promptSure(const char *prompt) {
	printf("\n%s?\n", prompt);
	while (1) {
		int c = getchar();
		if (c == 'y' || c == 'Y') {
			puts("YES");
			return 1;
		}
		else if (c == 'n' || c == 'N') {
			puts("NO");
			return 0;
		}
	}
}

void erase(const char *p) {
	unsigned long phys_addr;
	unsigned long phys_end;
	unsigned char plus = 0;

	skip_spaces(&p);
	if (*p == '!')
	{
		if (!promptSure("ERASE entire chip"))
			return;
		else {
			flash_erase_chip();	
			return;
		}
	}

	if (gethex_ulong(&p, &phys_addr) < 0)
		goto ERROR;

	skip_spaces(&p);

	if (*p == '+') 
	{
		plus = 1;
		p++;
	}

	if (gethex_ulong(&p, &phys_end) < 0)
		goto ERROR;



	if (plus)
		phys_end += phys_addr-1;


	// clear buffer
	memset(general_buf, 0xFF, GENERAL_BUF_SIZE);

	unsigned long flash_mask = (current_flash_type->sec_size * 1024) - 1;
	unsigned long buf_mask = GENERAL_BUF_SIZE - 1;

	while (phys_addr != phys_end + 1) {
		unsigned long e = phys_end;
	
		printf(">>>>%lx\n", phys_addr);

		if ((phys_addr & 0x00300000) == 0) {
			//we're in ROM area - split into Sectors
			if ((phys_addr & ~flash_mask) != (e & ~flash_mask)) {
				e = (phys_addr & ~flash_mask) | flash_mask;
			}
			flash_erase_sector(phys_addr);
		} else {
			//we're in RAM area - split into general_buf sized chunks
			if ((phys_addr & ~buf_mask) != (e & ~buf_mask)) {
				e = (phys_addr & ~buf_mask) | buf_mask;
			}	
			memw_in(general_buf, phys_addr, e - phys_addr + 1);
		}

		

		phys_addr = e + 1;
	}

	return;

ERROR:
	printf ("Bad command: E <phys_addr>[+<len>|<phys_end>] : !%s \n", p);

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

	printf("RAM:\t\t%dKB\n", 16*(1+(ram_limit & 0x3F)));

	if (!detected_flash_type) {
		printf("**WARNING** Flash rom type not recognized default to 64K*7 : %02x/%02x\nDefaulting to", flash_det_mid, flash_det_did);
	} else {
		printf("Detected");
	}
	printf(" ROM:\t%s\nID:\t\t%x/%x\nSize:\t\t%dx%d=%dKB\n",  current_flash_type->name, current_flash_type->manu_id, current_flash_type->dev_id, current_flash_type->sec_size, current_flash_type->n_secs, current_flash_type->sec_size * current_flash_type->n_secs );


	membuf_info();
}

extern const char SBC09MOS [];

void cat_slot(unsigned long ptr) {
	//check for an SBC09MOS entry
	memw_out(general_buf, ptr+0x3800, 64);
	if (!memcmp(general_buf+2, SBC09MOS, 8)) {
		printf("MOS  : %06lX : !%-6lX : %20s\n", ptr, ptr + 0x3800, general_buf+10);
		return;
	}
	memw_out(general_buf, ptr+6, 2);
	unsigned char rom_type = general_buf[0];
	unsigned char copy_offs = general_buf[1];
	memw_out(general_buf, ptr+copy_offs, 4);
	if (!memcmp(general_buf, "\0(C)", 4)) {
		memw_out(general_buf, ptr+9, copy_offs-8);
		printf("ROM  : %06lX : %02X : %25s\n", ptr, rom_type, general_buf);
	}
}

void cat(const char *p) {

	unsigned long ptr = 0;
	int flash_slots = (current_flash_type->sec_size * current_flash_type->n_secs) >> 4;
	for (int i = 0; i < flash_slots; i++) {
		cat_slot(ptr);

		ptr += 0x4000;
	}


	ptr = 0x308000;
	int ram_slots = (ram_limit & 0x3F) + 1 - 6;
	for (int i = 0; i < ram_slots; i++) {
		cat_slot(ptr);

		ptr += 0x4000;
	}

}

void ram_detect() {
	//detect RAM limit
	mmu_16(2) = 0xBF;		// top of ram - possibly an image
	unsigned char org = R_WINDOW[0x3FFF];

	ram_limit = 0xBF;
	while (ram_limit >= 0x83) {
		unsigned char n = 0x80 | ((ram_limit & 0x3F) >> 1);
		
		mmu_16(2) = 0xBF;
		R_WINDOW[0x3FFF] = 0xAA;
		mmu_16(2) = n;
		if (R_WINDOW[0x3FFF] != 0xAA)
			break;

		mmu_16(2) = 0xBF;	
		R_WINDOW[0x3FFF] = 0x55;
		mmu_16(2) = n;
		if (R_WINDOW[0x3FFF] != 0x55)
			break;

		ram_limit = n;
	}


	R_WINDOW[0x3FFF] = org;
}

int main(void) {

	ram_detect();


	puts("\n\nSBC09 - Bootloader\n====================\n");
	// check type of flash rom present
	// map bottom of flash into mmu at 8000
	mmu_16(2) = 0;

	flash_init();
	membuf_clear();
	info();

	while (1) {
		putc(':', stdout);
		if (rdline(general_buf, GENERAL_BUF_SIZE)) {

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
			case 'E':
				erase(p);
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
			case '.':
				cat(p);
				break;			
			default:
				printf("\nUnrecognized command %c %x\n", (int)cmd, (int)cmd);
				break;			
			}
		} 
	}
	

	return 0;
}
