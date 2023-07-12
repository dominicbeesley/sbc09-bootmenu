;;;
;;; The Kernel C run-time / start routine
;;;

;;;  imported symbols
	.globl 	_main
	.globl 	init_hardware
	.globl 	kstack_top


	;; exported symbols
	.globl	start

	;; startup code at offset 0
	.area 	.start
start:  jmp	main

	.area 	.text

main:	orcc 	#0x10		; interrupts definitely off
	lds 	#kstack_top

	;; zero out kernel's bss section
	ldx 	#__sectionbase_.bss__
	ldy 	#__sectionlen_.bss__
bss_wipe:
	clr 	,x+
	leay 	-1,y
	bne 	bss_wipe

	jsr 	_init_hardware
	jsr 	_main
	orcc 	#0x10
stop:	bra 	stop