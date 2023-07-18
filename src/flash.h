#ifndef __FLASH_H__
#define __FLASH_H__

enum e_flash_sec_size {sz_4k, sz_64k};

typedef struct flash_def {
	unsigned int manu_id;
	unsigned int dev_id;
	enum e_flash_sec_size sec_size;
	unsigned int n_secs;
	const char name[10];
} t_flash_def;

extern const t_flash_def *detected_flash_type;
extern const t_flash_def *current_flash_type;

extern void flash_init();

#endif