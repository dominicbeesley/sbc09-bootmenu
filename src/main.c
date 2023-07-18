#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "uart.h"
#include "hardware.h"


extern int default_vectors;

#define R_WINDOW ((volatile unsigned char*)0x8000)
#define MMU_IX_WINDOW 2
#define MMU_IX_MOS 3
#define R_555 R_WINDOW[0x555]
#define R_2AA R_WINDOW[0x2AA]
#define R_CMD_AUTO 0x90
#define R_CMD_AUTO_MID 0x00
#define R_CMD_AUTO_DID 0x01
#define R_CMD_EXIT 0xF0

enum sec_size {sz_4k, sz_64k};

typedef struct romtype {
	unsigned int manu_id;
	unsigned int dev_id;
	enum sec_size sec_size;
	unsigned int n_secs;
	const char name[10];
} t_romtype;

const t_romtype romtypes[] = {
	{0x37, 0x86, sz_64k, 8, "AM29040"}	
};

const t_romtype * findromtype(unsigned int mid, unsigned int did) {
	int i = sizeof(romtypes) / sizeof(t_romtype);
	const t_romtype *p = romtypes;
	while (i > 0) {
		if (p->manu_id == mid && p->dev_id == did)
			return p;
		else
		{
			p++;
			i--;
		}
	}
	return NULL;
}

unsigned int membuf_min;
unsigned int membuf_max;
unsigned int exec_addr;


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

void membuf_in(unsigned char *src, unsigned int dest_addr, unsigned int len) {
	unsigned int ptr = dest_addr;
	unsigned int end = dest_addr + len - 1;
	while (ptr < end) {
		// 16K aligned blocks
		unsigned int end2 = ((ptr & 0xC000) == (end & 0xC000)) ? end : (ptr & 0xC000) | 0x3FFF;
		unsigned int n = end2 - ptr + 1;

		unsigned char ix = (MMU_SEL_RAM) | (MMU_MEM_MAX - 4 + (ptr >> 14));
		mmu_16(MMU_IX_WINDOW) = ix;
//		printf("%x - %x, %x:%d written \n", ptr, end2, ix, n);

		// page the block into WINDOW
		memcpy((void *)(R_WINDOW + (ptr & 0x3FFF)), src, n);
		
		if (!(end2 + 1))
			break;
		ptr = end2+1;
	}
}


unsigned char srec_ck = 0;
int membuf_read_srec(const char *p) {
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
	unsigned char *b = buf;
	while (len) {
		p+=2;
		int x = srec_hex(p);
		if (x < 0 )
			goto ERROR_BAD;
		*b++ = x;
		ck += x;
		len --;
	}

	if (ck != 0xFF)
		goto ERROR_BAD;

	unsigned int addr = (buf[0] << 8) | buf[1];

	if (srec_type == '9')
	{
		printf("EXEC address : %X\n", addr);
	} else if (srec_type == '1') {
		int l = buf[0] - 3;
		if (addr < membuf_min)
			membuf_min = addr;
		if (addr + l - 1 >= membuf_max)
			membuf_max = addr + l - 1;

		membuf_in(buf + 2, addr, buf[0] - 3);
	}

	return;
ERROR_BAD:
	puts("Bad SREC");
	return -1;

}

#define CLEAR_SIZE 64
void membuf_clear() {
	membuf_min = 0xFFFF;
	membuf_max = 0;
	exec_addr = -1;

	unsigned char buf[CLEAR_SIZE];
	memset(buf, 0xFF, CLEAR_SIZE);
	unsigned int addr = 0;
	do {
		membuf_in(buf, addr, CLEAR_SIZE);
		addr = addr + CLEAR_SIZE;
	} while (addr != 0);


	puts("Buffer cleared\n");
}


char command_buf[100];

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
	asm("\tandcc	#0xAF");


	*((int*)0x8000) = 0xBEEF;
	printf("TEST: %x\n", *((int*)0x8000));
	printf("TEST: %x\n", *((int*)0xC000));
	printf("TEST: %x\n", *((int*)0xB7FA));
	printf("TEST: %x\n", *((int*)0xF7FA));

	puts("SBC09 :\t\tBootloader\n");
	// check type of flash rom present
	// map bottom of flash into mmu at 8000
	mmu_16(2) = 0;
	// unlock device codes
	R_555 = 0xAA;
	R_2AA = 0x55;
	R_555 = R_CMD_AUTO;
	unsigned char mid = R_WINDOW[R_CMD_AUTO_MID];
	unsigned char did = R_WINDOW[R_CMD_AUTO_DID];

	// exit device codes
	R_555 = 0xAA;
	R_2AA = 0x55;
	R_555 = R_CMD_EXIT;
		
	const t_romtype *rt = findromtype(mid, did);
	if (!rt) {
		puts("**WARNING** Flash rom type not recognized default to 64K*7");
	} else {
		printf("Flash ROM:\t%x/%x:%8s\n", did, mid, rt->name);
	}

	membuf_clear();

	while (1) {
		putc(':', stdout);
		if (fgets(command_buf, sizeof(command_buf), stdin)) {

			char *p = command_buf;
			while (*p == ' ') p++;

			char cmd = toupper(*p++);

			switch (cmd) {
			case 'C':
				membuf_clear();
				break;
			case 'S':
				membuf_read_srec(p);
				break;
			default:
				printf("\nUnrecognized command %c %x\n", (int)cmd, (int)cmd);
				break;
			}
		} 
	}
	

	return 0;
}
