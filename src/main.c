#include <stdlib.h>
#include <stdio.h>
#include "uart.h"
#include "hardware.h"

extern void default_vectors;


int main(void) {

	// setup channel A of UART for 115200 and 16K MMU enabled
	uart_init();
	// map topmost RAM block as read/write at top 8000 and ready it with vectors etc, 
	// we treat this as our own private workspace
	// first map in at 8000 and setup vectors
	mmu_16(2) = MMU_SEL_RAM | MMU_MEM_MAX;
	memcpy((void *)((VECTOR_LOC & 0x3FFF) | 0x8000), &default_vectors, 16);
	// and now map it in at C000
	mmu_16(3) = MMU_SEL_RAM | MMU_MEM_MAX;

	*((int*)0x8000) = 0xBEEF;
	printf("TEST: %x\n", *((int*)0x8000));
	printf("TEST: %x\n", *((int*)0xC000));
	printf("TEST: %x\n", *((int*)0xB7FA));
	printf("TEST: %x\n", *((int*)0xF7FA));

	puts("SBC09 : Bootloader\n");

	return 0;
}
