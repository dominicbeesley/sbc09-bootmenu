#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "uart.h"
#include "hardware.h"
#include "memmap.h"
#include "flash.h"
#include "membuf.h"
#include "srec.h"

extern int default_vectors;




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

	flash_init();
	if (!detected_flash_type) {
		fputs("**WARNING** Flash rom type not recognized default to 64K*7", stdout);
	} 
	printf("Flash ROM:\t%x/%x:%8s\n", current_flash_type->manu_id, current_flash_type->dev_id, current_flash_type->name);
	

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
				srec_read_to_membuf(p);
				break;
			case '?':
				membuf_info();
				break;
			default:
				printf("\nUnrecognized command %c %x\n", (int)cmd, (int)cmd);
				break;			
			}
		} 
	}
	

	return 0;
}
