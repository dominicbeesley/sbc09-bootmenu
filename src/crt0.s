;;;
;;; The Kernel C run-time / start routine
;;;

;;;  imported symbols
	.globl 	_main
	.globl 	init_hardware
	.globl 	kstack_top
	.globl	_malloc
	.globl	_memcpy
	.globl	_abort


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
exit:
	orcc 	#0x10
stop:	bra 	stop

_malloc:
	ldx	#str_malloc
	bra	ab1
_memcpy:
	ldx	#str_memcpy
	bra	ab1
_abort:
	ldx	#str_abort
ab1:	jsr	_printstr
	bra	exit




_printstr:
	ldb	,X+
@lp:	beq	@sk
	jsr	_uart_writec
	bra	@lp
@sk:	rts

	
	.area	.data
str_memcpy:	fcn "memcpy"
str_malloc:	fcn "malloc"
str_abort:	fcn "abort"