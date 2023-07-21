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



	.area 	.vectors

_default_vectors:
	fdb	_default_ill0
	fdb	_default_swi3
	fdb	_default_swi2
	fdb	_default_firq
	fdb	IRQ_HANDLER
	fdb	_default_swi
	fdb	_default_nmi
	fdb	_default_res	
	fdb	_default_boot	; special entry vector to call self
	fcc	"SBC09MOS"

	.area 	.text

main:	orcc	#$50		; ensure interrupts off
	lds 	#$200		; assuming memory in low block is "MOS" ram, use the MOS stack area
	; B contains the MMU index of the current MOS rom that we are running from
	stb	$100		; cache the MOS slot #
	jsr	uart_init	; B will be used here to set up the mapping

	; map top of ram in to 8000 and copy ourselves to it 

	ldb	#%10111111	; top of RAM
	stb	$FE12		; 8000 mapping
	ldb	$100		; our own MMU index
	stb 	$FE11		; map ROM image in at 4000

	ldx 	#$4000		
	ldd	#$4000
	pshs 	D,X
	ldx	#$8000
	jsr	_memcpy		; copy ourselves to top of RAM
	leas	4,S

	ldb	#%10111111	; top of RAM read / write!
	stb	$FE13		; C000 mapping we will continue to run from there!?!?
	ldb	#%11111111	; top of RAM read only for safety
	stb	$FE12		; 8000 mapping - this will get swapped about to access other memory

	ldb	#%10000000	; bottom of RAM read only for testing
	stb	$FE10		; 0000 mapping - unused
	stb	$FE11		; 4000 mapping - unused 

	;; zero out kernel's bss section
	ldx 	#__sectionbase_.bss__
	ldy 	#__sectionlen_.bss__
bss_wipe:
	clr 	,x+
	leay 	-1,y
	bne 	bss_wipe

;	lds	#kstack_top
	lds	#$E00		;TESTING TODO put back

	;; interrupts on
	andcc	#0xAF

	;; and we're off
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
	; if we are here the hardware reset vector has been hit assume the MMU is OFF
	; assume that we will be loaded from bank #1
	ldb	#1
_default_boot:
	; if we are here we have been booted from another instance of this boot menu
	; assume that block 0,1 are RAM that we can use 100..200 (i.e. MOS)
	; assume 8000-BFFF is B-1 and C000-FFFF is block in B
	jmp	main
	


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
