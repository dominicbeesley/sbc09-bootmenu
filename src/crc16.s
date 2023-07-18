
	.globl _crc16

	.area .text

	# unsigned int crc16(unsigned int crc, void *ptr, unsigned int n)
POLYH    	EQU $10
POLYL   	EQU $21
POLY16		EQU $1021

_crc16:
	pshs	Y,U
	tfr	X,D
	ldu	6,S	; pointer
	ldx	8,S	; byte count

@bytl:	eora	,U+
	ldy	#8	; inner loop counter
@ll1:	aslb
	rola
	bcc	@c1
	eora	#POLYH
	eorb	#POLYL
@c1:	leay	-1,Y
	bne	@ll1
	leax	-1,X
	bne	@bytl
	tfr	D,X

	puls	Y,U,PC
