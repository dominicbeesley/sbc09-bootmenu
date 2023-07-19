
	.globl _do_boot


	.area .text

; extern void do_boot(unsigned int long phys_addr);


_do_boot:
	ldd	4,S
	anda	#$3F
	ora	#$C0		; this now contains address in MOS area
	tfr	D,X
	ldd	3,S
	rolb 
	rola
	rolb
	rola
	tfr	A,B
	stb	$FE13		; poke the MOS register
	deca
	sta	$FE12
	jmp	[,X]