#include <stdlib.h>
#include <stdio.h>
#include "membuf.h"
#include "crc16.h"
#include "memmap.h"
#include "hardware.h"
#include "memw.h"

unsigned int membuf_min;
unsigned int membuf_max;
unsigned int membuf_exec;

/* The memory buffer is a 64K area of RAM to which files are loaded and from
   which they can be dumped. When writing to EEPROM first SREC data is loaded
   to the buffer then moved from the buffer to EEPROM
*/

void membuf_in(unsigned char *src, unsigned int dest_addr, unsigned int len) {
	if ((unsigned long)dest_addr + len > 0x10000) {
		unsigned int rem = dest_addr + len;
		unsigned int l1 = - rem;
		memw_in(src, (long)dest_addr + ((long)((MMU_SEL_RAM) | (MMU_MEM_MAX - 4)) << 14), l1);
		memw_in(src, 0 | (long)((MMU_SEL_RAM) | (MMU_MEM_MAX - 4)) << 14, rem);
		membuf_min = 0;
		if (l1 - 1 > membuf_max)
			membuf_max = l1 - 1;
	} else {
		memw_in(src, (long)dest_addr + ((long)((MMU_SEL_RAM) | (MMU_MEM_MAX - 4)) << 14), len);

		if (dest_addr < membuf_min)
			membuf_min = dest_addr;
		if (dest_addr + len - 1 > membuf_max)
			membuf_max = dest_addr + len - 1;

	}
}

void membuf_out(unsigned char *dest, unsigned int src_addr, unsigned int len)
{
	if ((unsigned long)src_addr + len > 0x10000) {
		unsigned int rem = src_addr + len;
		unsigned int l1 = - rem;
		memw_out(dest, (long)src_addr + ((long)((MMU_SEL_RAM) | (MMU_MEM_MAX - 4)) << 14), l1);
		memw_out(dest, 0 | (long)((MMU_SEL_RAM) | (MMU_MEM_MAX - 4)) << 14, rem);

	} else {
		memw_out(dest, (long)src_addr + ((long)((MMU_SEL_RAM) | (MMU_MEM_MAX - 4)) << 14), len);
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
