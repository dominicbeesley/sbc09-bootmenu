
	.globl _do_boot


	.area .text

; extern void do_boot(unsigned int long phys_addr);


_do_boot:
	orcc	#$50		; turn off interrupts
	ldd	4,S
	anda	#$3F
	ora	#$C0		; this now contains address in MOS area
	tfr	D,U
	ldd	3,S
	rolb 
	rola
	rolb
	rola
	tfr	A,B

	; we now need to copy the boot code to "SAFE" memory,
	; page the bottom block of memory in at 8000
	lda	#$80
	sta	$FE12
	; copy the rest of our code to a "safe" part of low memory ($100..) bottom of MOS stack
	ldx	#dob_rest
	ldy	#$8100
@lp:	lda	,X+
	sta	,Y+
	cmpx	#dob_end
	bne	@lp
	lda	#$80
	sta	$FE10
	inca
	sta	$FE11		; bottom two blocks are now bottom of RAM
	jmp	$100		; continue running from RAM we just set up

dob_rest:	
	decb
	sta	$FE12		; default 8000..BFFF to slot below MOS
	incb
	stb	$FE13		; poke the MOS register
	jmp	[,U]
dob_end: