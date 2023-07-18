#include <stdlib.h>
#include "flash.h"
#include "memmap.h"
#include "hardware.h"

#define R_555 R_WINDOW[0x555]
#define R_2AA R_WINDOW[0x2AA]
#define R_CMD_AUTO 0x90
#define R_CMD_AUTO_MID 0x00
#define R_CMD_AUTO_DID 0x01
#define R_CMD_EXIT 0xF0

const t_flash_def *detected_flash_type;
const t_flash_def *current_flash_type;


const t_flash_def romtypes[] = {
	{0x37, 0x86, sz_64k, 8, "AM29040"}	
};

const t_flash_def * flash_getdef(unsigned int mid, unsigned int did) {
	int i = sizeof(romtypes) / sizeof(t_flash_def);
	const t_flash_def *p = romtypes;
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

void flash_init() {

	mmu_16(MMU_IX_WINDOW) = MMU_SEL_ROM + 0;

	unsigned char mid;
	unsigned char did;

	// unlock device codes
	R_555 = 0xAA;
	R_2AA = 0x55;
	R_555 = R_CMD_AUTO;
	mid = R_WINDOW[R_CMD_AUTO_MID];
	did = R_WINDOW[R_CMD_AUTO_DID];

	// exit device codes
	R_555 = 0xAA;
	R_2AA = 0x55;
	R_555 = R_CMD_EXIT;

	detected_flash_type = flash_getdef(mid, did);
	current_flash_type = (detected_flash_type == NULL) ? &romtypes[0] : detected_flash_type;
}