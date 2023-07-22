#include <stdlib.h>
#include "flash.h"
#include "memmap.h"
#include "hardware.h"

#define R_555 R_WINDOW[0x1555]
#define R_2AA R_WINDOW[0x2AAA]
#define R_CMD_AUTO 0x90
#define R_CMD_AUTO_MID 0x00
#define R_CMD_AUTO_DID 0x01
#define R_CMD_EXIT 0xF0
#define R_CMD_PRGBYTE 0xA0
#define R_CMD_ERASE 0x80
#define R_CMD_ERASE_CHIP 0x10
#define R_CMD_ERASE_SECTOR 0x30

unsigned char flash_det_mid;
unsigned char flash_det_did;

const t_flash_def *detected_flash_type;
const t_flash_def *current_flash_type;


const t_flash_def romtypes[] = {
	{0x01, 0x20, 16, 8, "Am29F010"}, // 128k
	{0x01, 0xA4, 64, 8, "Am29F040"}, // 512k
	{0x01, 0x4F, 64, 8, "Am29LV040B"}, // 512k
	{0x37, 0x86, 64, 8, "A29040B"}, // 512k
	{0xAD, 0x40, 64, 8, "BM29F040"}, // 512k
	{0xAD, 0xA4, 64, 8, "HY29F040A"}, // 512k
	{0xC2, 0xA4, 64, 8, "MX29F040"}, // 512k
	{0xC2, 0x4F, 64, 8, "MX29LV040"}, // 512k
	{0x9D, 0x1C, 4, 32, "Pm39LV010"}, // 128k
	{0x9D, 0x3D, 4, 64, "Pm39LV020"}, // 256k
	{0x9D, 0x3E, 4, 128, "Pm39LV040"}, // 512k
	{0x9D, 0x1B, 4, 16, "Pm39LV512"}, // 64k
	{0xBF, 0xB5, 4, 32, "SST39SF010A"}, // 128k
	{0xBF, 0xB6, 4, 64, "SST39SF020A"}, // 256k
	{0xBF, 0xB7, 4, 128, "SST39SF040"}, // 512k
	{0xBF, 0xB4, 4, 16, "SST39SF512"}, // 64k
	{0xBF, 0xD5, 4, 32, "SST39VF010"}, // 128k
	{0xBF, 0xD6, 4, 64, "SST39VF020"}, // 256k
	{0xBF, 0xD7, 4, 128, "SST39VF040"}, // 512k
	{0xBF, 0xD4, 4, 16, "SST39VF512"}, // 64k
	{0x20, 0xE2, 64, 8, "M29F040B"}, // 512k
	{0x20, 0x23, 16, 8, "M29W010B"}, // 128k
	{0x20, 0xE3, 64, 8, "M29W040B"}, // 512k
	{0xDA, 0xB5, 64, 4, "W39L020"}, // 256k
	{0xDA, 0xB6, 64, 8, "W39L040"} // 512k
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

void flash_cmd(unsigned char cmd) {
	mmu_16(MMU_IX_WINDOW) = MMU_SEL_ROM + 1;
	R_555 = 0xAA;
	mmu_16(MMU_IX_WINDOW) = MMU_SEL_ROM + 0;
	R_2AA = 0x55;
	mmu_16(MMU_IX_WINDOW) = MMU_SEL_ROM + 1;
	R_555 = cmd;
}

void flash_init() {


	// unlock device codes
	flash_cmd(R_CMD_AUTO);
	mmu_16(MMU_IX_WINDOW) = MMU_SEL_ROM + 0;
	flash_det_mid = R_WINDOW[R_CMD_AUTO_MID];
	flash_det_did = R_WINDOW[R_CMD_AUTO_DID];

	// exit device codes
	flash_cmd(R_CMD_EXIT);

	detected_flash_type = flash_getdef(flash_det_mid, flash_det_did);
	current_flash_type = (detected_flash_type == NULL) ? &romtypes[0] : detected_flash_type;
}

//assumes: - all in rom and in same 16k bank (already setup in memw.c)
extern void flash_write(unsigned long phys_addr, const void *buf, unsigned int len) {
	unsigned char *p = (unsigned char *)buf;
	unsigned volatile char *off = (unsigned char *)(R_WINDOW + (phys_addr & 0x3FFF));
	unsigned char ix = (phys_addr >> 14);

	while (len) {
		__asm("orcc #$50");

		//start programming sequence
		flash_cmd(R_CMD_PRGBYTE);
		mmu_16(MMU_IX_WINDOW) = ix;		
		unsigned char c = *p++;
		*off = c;
		while (*off != *off) ;

		__asm("andcc #$AF");

		off++;
		len--;
	}
}


void flash_erase_chip(void) {
	
	__asm("orcc #$50");

	flash_cmd(R_CMD_ERASE);
	flash_cmd(R_CMD_ERASE_CHIP);

	while (*R_WINDOW != *R_WINDOW) ;

	__asm("andcc #$AF");

	
}

void flash_erase_sector(unsigned long phys_addr) {

	__asm("orcc #$50");

	flash_cmd(R_CMD_ERASE);

	mmu_16(MMU_IX_WINDOW) = MMU_SEL_ROM + 1;
	R_555 = 0xAA;
	mmu_16(MMU_IX_WINDOW) = MMU_SEL_ROM + 0;
	R_2AA = 0x55;
	mmu_16(MMU_IX_WINDOW) = (phys_addr >> 14);
	*((volatile unsigned char *)(R_WINDOW + (phys_addr & 0x3FFF & ~((1024 * current_flash_type->sec_size)-1)))) = R_CMD_ERASE_SECTOR;

	while (*R_WINDOW != *R_WINDOW) ;

	__asm("andcc #$AF");

}

