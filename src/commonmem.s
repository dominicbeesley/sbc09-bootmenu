        .module commonmem

        ; exported symbols
        .globl kstack_top
	
        .area .bss

kstack_base:
	zmb 512
kstack_top:
