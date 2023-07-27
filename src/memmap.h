#ifndef __MEMMAP_H__
#define __MEMMAP_H__

//R_WINDOW the cpu address of the "window" used for accessing buffer/flash/ram
#define R_WINDOW ((volatile unsigned char *)0x8000)
#define R_REBOOT ((unsigned long *)0xFFF8)
#define R_REBOOT_I ((unsigned long *)0xFFFC)



//MMU index of the window
#define MMU_IX_WINDOW 2
//MMU index of the MOS
#define MMU_IX_MOS 3

#endif