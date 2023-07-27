;;;
;;; The Kernel C run-time / start routine
;;;

;;;  imported symbols
	.globl 	_main
	.globl 	init_hardware
	.globl	_malloc
	.globl	_memcpy
	.globl	_abort
	.globl   IRQ_HANDLER
	.globl	_default_vectors


	;; exported symbols
	.globl	start
	.globl 	_kstack_top
	.globl   _general_buf
	.globl	_uart_in_buf
	.globl	_uart_out_buf
	.globl   _SBC09MOS


	.area 	.start
	; if we enter here then we've been loaded to RAM for run, copy ourselved to topmost block
	; from wherever we are now (assume E00 - 3FFF)
start:  	jmp	main

	.area	.vectors
_default_vectors:
	fdb	_default_ill0
	fdb	_default_swi3
	fdb	_default_swi2
	fdb	_default_firq
	fdb	IRQ_HANDLER
	fdb	_default_swi
	fdb	_default_nmi
	fdb	_default_res

	.area 	.text

_SBC09MOS:
	fcc	"SBC09MOS"

main:	orcc 	#0x50		; interrupts definitely off
	lds 	#$200		; small stack at 0..200 for now
	ldb	#$FF

	jsr	uart_init

	;; zero out kernel's bss section
	ldx 	#__sectionbase_.bss__
	ldy 	#__sectionlen_.bss__
bss_wipe:
	clr 	,x+
	leay 	-1,y
	bne 	bss_wipe

	;; we will continue to run from our present location
	
	; page "ourselves" into C000 so that the vectors show there
	; we assume that we are running in logical low ram to E00 and
	; that that is mapped to bottom of phys ram
	lda 	#%10000000
	sta	$FE13

	lds	#_kstack_top

	;; interrupts on
	andcc	#0xAF

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
	lds	#_kstack_top

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

	.area	stack,bss
_kstack_base:	rmb 0x200	
_kstack_top:	

	.area	buffers,bss
_general_buf:	rmb 0x400
_uart_in_buf:	rmb 0x100
_uart_out_buf:	rmb 0x100
