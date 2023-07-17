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
	.globl   IRQ_HANDLER
	.globl	_default_vectors


	;; exported symbols
	.globl	start

	;; startup code at offset 0
	.area 	.start
start:  jmp	main

	.area 	.text

_default_vectors:
	fdb	_default_ill0
	fdb	_default_swi3
	fdb	_default_swi2
	fdb	_default_firq
	fdb	IRQ_HANDLER
	fdb	_default_swi
	fdb	_default_nmi
	fdb	_default_res


main:	orcc 	#0x50		; interrupts definitely off
	lds 	#kstack_top

	;; zero out kernel's bss section
	ldx 	#__sectionbase_.bss__
	ldy 	#__sectionlen_.bss__
bss_wipe:
	clr 	,x+
	leay 	-1,y
	bne 	bss_wipe

	jsr 	_main

;TEST
	swi

exit:
	orcc 	#0x10
stop:	bra 	stop

_malloc:
	ldx	#str_malloc
	bra	ab1
_abort:
	ldx	#str_abort
	bra	ab1


ab1:	; assume the stack's gone
	lds	#kstack_top

	orcc	#0x50
	pshs	X
	ldx	#str_crash
	jsr	_printstr
	puls	X
	jsr	_printstr	
@h:	bra	@h

_default_ill0:
	ldx	#str_ill0
	bra	ab1
_default_swi3:
	ldx	#str_swi3
	bra	ab1
_default_swi2:
	ldx	#str_swi2
	bra	ab1
_default_firq:
	ldx	#str_firq
	bra	ab1
_default_swi:
	ldx	#str_swi
	bra	ab1
_default_nmi:
	ldx	#str_nmi
	bra	ab1
_default_res:
	ldx	#str_res
	bra	ab1



_printstr:
@lp:	ldb	,X+
	beq	@sk
	jsr	uart_writec_direct
	bra	@lp
@sk:	rts

	
	.area	.data
str_malloc:	fcn "malloc"
str_abort:	fcn "abort"
str_ill0:	fcn "illegal"
str_swi3:	fcn "swi3"
str_swi2:	fcn "swi2"
str_firq:	fcn "firq"
str_swi:		fcn "swi"
str_nmi:		fcn "nmi"
str_res:		fcn "reset"
str_crash:	fcn "CRASH: "
