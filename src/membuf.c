#include <stdlib.h>
#include <stdio.h>
#include "membuf.h"
#include "crc16.h"
#include "memmap.h"
#include "hardware.h"

unsigned int membuf_min;
unsigned int membuf_max;
unsigned int membuf_exec;

/* The memory buffer is a 64K area of RAM to which files are loaded and from
   which they can be dumped. When writing to EEPROM first SREC data is loaded
   to the buffer then moved from the buffer to EEPROM
*/

void membuf_in(unsigned char *src, unsigned int dest_addr, unsigned int len) {
	unsigned int ptr = dest_addr;
	unsigned int end = dest_addr + len - 1;

	while (ptr < end) {

		// 16K aligned blocks
		unsigned int end2 = ((ptr & 0xC000) == (end & 0xC000)) ? end : (ptr & 0xC000) | 0x3FFF;
		unsigned int n = end2 - ptr + 1;

		// do this here to account for cases where there's a wrap-around
		if (ptr < membuf_min)
			membuf_min = ptr;
		if (end2 >= membuf_max)
			membuf_max = end2;

		// page the block into WINDOW
		unsigned char ix = (MMU_SEL_RAM) | (MMU_MEM_MAX - 4 + (ptr >> 14));
		mmu_16(MMU_IX_WINDOW) = ix;

		memcpy((void *)(R_WINDOW + (ptr & 0x3FFF)), src, n);
		
		if (!(end2 + 1))
			break;
		ptr = end2+1;
	}
}



#define CLEAR_SIZE 64
void membuf_clear() {

	unsigned char buf[CLEAR_SIZE];
	memset(buf, 0xFF, CLEAR_SIZE);
	unsigned int addr = 0;
	do {
		membuf_in(buf, addr, CLEAR_SIZE);
		addr = addr + CLEAR_SIZE;
	} while (addr != 0);

	membuf_min = 0xFFFF;
	membuf_max = 0;
	membuf_exec = -1;

	puts("Buffer cleared\n");
}


unsigned int membuf_crc16(unsigned int addr, unsigned int end) {
	unsigned int crc = 0;
	while (addr < end) {
		// 16K aligned blocks
		unsigned int end2 = ((addr & 0xC000) == (end & 0xC000)) ? end : (addr & 0xC000) | 0x3FFF;
		unsigned int n = end2 - addr + 1;

		// page the block into WINDOW
		unsigned char ix = (MMU_SEL_RAM) | (MMU_MEM_MAX - 4 + (addr >> 14));
		mmu_16(MMU_IX_WINDOW) = ix;

		crc = crc16(crc, (void *)(R_WINDOW + (addr & 0x3FFF)), n);
		
		if (!(end2 + 1))
			break;
		addr = end2+1;
	}
	return crc;
}


void membuf_info() {
	if (membuf_max < membuf_min)
	{
		puts("Buffer empty\n");
		return;
	} else {
		printf("Buffer data %x - %x", membuf_min, membuf_max);
		if (membuf_exec != 0xFFFF) {
			printf(" [exec %x]", membuf_exec);
		}
		printf("\nCRC %x\n", membuf_crc16(membuf_min, membuf_max));
	}
}
