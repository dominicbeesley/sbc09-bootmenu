#include <stdlib.h>
#include <stdio.h>
#include "memw.h"
#include "memmap.h"
#include "hardware.h"

/* These are functions for reading from the memory outside the normal 64K 
	cpu addressing range. A window at R_WINDOW is paged in from the larger
	memory map and copied piece-meal to/from the cpu's memory
*/

void memw_in(unsigned char *src, unsigned long dest_addr, unsigned int len) {
	unsigned long ptr = dest_addr;
	unsigned long end = dest_addr + len - 1;

	while (ptr < end) {

		// 16K aligned blocks
		unsigned long end2 = ((ptr & 0x3FC000) == (end & 0x3FC000)) ? end : (ptr & 0x3FC000) | 0x3FFF;
		unsigned int n = end2 - ptr + 1;

		// page the block into WINDOW
		unsigned char ix = (ptr >> 14);

		if ((ptr & 0x300000) == 0) {
			flash_write(ptr, src, len);
		} else {
			mmu_16(MMU_IX_WINDOW) = ix;		
			memcpy((void *)(R_WINDOW + (ptr & 0x3FFF)), src, n);
		}
		
		if (!(end2 + 1))
			break;
		ptr = end2+1;
	}
}

void memw_out(unsigned char *dest, unsigned long src_addr, unsigned int len) {
	unsigned long ptr = src_addr;
	unsigned long end = src_addr + len - 1;

	while (ptr < end) {

		// 16K aligned blocks
		unsigned long end2 = ((ptr & 0x3FC000) == (end & 0x3FC000)) ? end : (ptr & 0x3FC000) | 0x3FFF;
		unsigned int n = end2 - ptr + 1;

		// page the block into WINDOW
		unsigned char ix = (ptr >> 14);
		mmu_16(MMU_IX_WINDOW) = ix;

		memcpy(dest, (void *)(R_WINDOW + (ptr & 0x3FFF)), n);
		
		if (!(end2 + 1))
			break;
		ptr = end2+1;
	}
}

