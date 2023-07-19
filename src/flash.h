#ifndef __FLASH_H__
#define __FLASH_H__


typedef struct flash_def {
	unsigned int manu_id;
	unsigned int dev_id;
	unsigned int sec_size;
	unsigned int n_secs;
	const char name[12];
} t_flash_def;

extern const t_flash_def *detected_flash_type;
extern const t_flash_def *current_flash_type;

extern void flash_init();

//assumes: - all in rom and in same 16k bank (already setup in memw.c)
extern void flash_write(unsigned long phys_addr, const void *buf, unsigned int len);

#endif