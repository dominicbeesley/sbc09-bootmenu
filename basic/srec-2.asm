; This program is intended to be a faster SREC loader that is bootstrapped by the BASIC srec loader
; This program then loads a larger program into memory
; This program relocates itself to 800..DFF and uses 600..6FF as workspace

		ORG	$E00


STRBUF		EQU	$600
OSWORD		EQU	$FFF1
OSB_ACKESC	EQU	$7E
OSBYTE		EQU	$FFF4
OSASCI		EQU	$FFE3

CC_C		EQU	$01
SEC		MACRO
		ORCC	#CC_C
		ENDM
CLC		MACRO
		ANDCC	#~CC_C
		ENDM
RUNLOC		EQU	$800

reloc		leax	enter,PCR
		leau	,X
		stu	,--S
		leax	PROG_END,PCR
		tfr	X,D
		subd	,S++		
		ldy	#RUNLOC
1		lda	,U+
		sta	,Y+
		leax	-1,X
		bne	1B
		jmp	RUNLOC

enter
		LDA	#':'
		JSR	OSASCI
	; Read a line of input to &600
		LDY	#STRBUF
		LEAX	os_blk,PCR
		STY	0,X
		LDB	#$FF
		STB	2,X
		LDD	#$207E
		STD	3,X
		LDA	#0
		JSR	OSWORD
		LBCS	escape
		CMPY	#6
		BLO	notsrec
		LDX	#STRBUF
spc
		LDA	,X+
		CMPA	#' '
		BLS	spc
		CMPA	#'S'
		BNE	notsrec
		LBSR	gethexnyb
		BCS	nothex
		CMPA	#10
		BHS	notsrec
		STA	srec_type,PCR
		CLR	ck_acc,PCR
		BSR	gethex
		BCS	nothex
		CMPA	#3
		BLO	notsrec
		TFR	A,B
		STA	srec_len,PCR \record length
		LEAY	,X
		STY	bin_start,PCR
rdlp
		BSR	gethex
		BCS	nothex
		STA	,Y+
		DECB
		BNE	rdlp
		LDA	ck_acc,PCR	
		INCA
		BNE	badck
LDAsrec_typ	LDX	bin_start,PCR
		LDA	srec_type,PCR
		CMPA	#1
		BEQ	writemem
		CMPA	#9
		BEQ	exec
		LDA	#'?'
		JSR	OSASCI
		LBRA	enter

		
badck
		LEAX	str_badck,PCR
		BRA	err
notsrec
		LEAX	str_notsrec,PCR
		BRA	err
nothex
		LEAX	str_nothex,PCR
err
		BSR	printX
		LBRA	enter

printX
		LDA	,X+
		BEQ	printXex
		JSR	OSASCI
		BRA	printX
printXex
		RTS

writemem		LDA	#'w'
		JSR	OSASCI
		LDY	,X		; address
		LDB	srec_len,PCR
		SUBB	#3
		LEAX	2,X
wmlp
		LDA	,X+
		STA	,Y+
		DECB
		BNE	wmlp
		LBRA	enter
exec		LDA	#'e'
		JSR	OSASCI
		JMP	[,X]

gethex		BSR	gethexnyb
		BCS	1F
		ROLA
		ROLA
		ROLA
		ROLA
		STA	,-S
		BSR	gethexnyb
		ORA	,S+
		BCS	1F
		STA	,-S
		ADDA	ck_acc,PCR
		STA	ck_acc,PCR
		LDA	,S+
		CLC
1		RTS

gethexnyb	LDA	,X+
		SUBA	#'0'
		BCS	1F
		CMPA	#10
		BLO	2F
		SUBA	#7
		CMPA	#10
		BCS	1F
		CMPA	#16
		BLO	2F
		SEC
		RTS
2		CLC
1		RTS

str_badck	FCN	"CK"
str_notsrec	FCN	"SREC"		
str_escape	FCN	"Esc"
str_nothex	FCN	"HEX"

escape
		LEAX	str_escape,PCR
		BSR	printX
		LDA	#OSB_ACKESC
		JSR	OSBYTE
		LBRA	enter
ck_acc		FCB	0
srec_type	FCB	0
srec_len		FCB	0
os_blk		RZB	5
bin_start	FDB	0

PROG_END

		END	reloc