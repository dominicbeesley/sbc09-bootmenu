
	include "hardware.inc"

	.globl	_uart_init
	.globl	_uart_readc
	.globl	_uart_writec

	.area .text

FC_NONE      EQU 0
FC_XON_XOFF  EQU 1
FC_RTS_CTS   EQU 2

FLOW_CONTROL EQU FC_RTS_CTS

;; Threshold at which RTS is asserted
FC_LO_THRESH EQU $80

;; Threshold at which RTS is de-asserted
FC_HI_THRESH EQU $C0

UART_XTAL       EQU     3686400
UART_T0DIV      EQU     (((UART_XTAL/16)/100)/2)-1


_uart_init:
                lda     #MR1_PARITY_MODE_OFF|MR1_PARITY_BITS_8    ; no parity, 8 bits/char - mr1a,b
                sta     SBC09_UART_MRA
                lda     #MR2_TxCTS|MR2_STOP_BITS_1                ; cts enable tx, 1.000 stop bits - mr2a,b
                sta     SBC09_UART_MRA
                lda     #CRA_TxEN|CRA_RxEN                        ; enable tx and rx
                sta     SBC09_UART_CRA
                lda     #CRA_CMD_SET_RxBRG                        ; set channel a rx extend bit
                sta     SBC09_UART_CRA
                lda     #CRA_CMD_SET_TxBRG                        ; set channel a tx extend bit
                sta     SBC09_UART_CRA
                lda     #%10001000                                ; internal 115,200 baud
                sta     SBC09_UART_CSRA
                lda     #ACR_CT_MODE_SQ_X1_DIV16                  ; timer mode, clock = xtal/16 = 3686400 / 16 = 230400 hz
                sta     SBC09_UART_ACR
                ldd     #UART_T0DIV                               ; 16-bit write to counter to get a 100hz tick
                std     SBC09_UART_CTU
                lda     #OP_BIT_RTS_A                             ; assert rts
                sta     SBC09_UART_OPRSET
                lda     #IMR_CTR|IMR_RxRDY_A                      ; timer int, rx int enabled; tx int disabled
                sta     SBC09_UART_IMR
                lda     SBC09_UART_STARTCT                        ; start the counter-timer

      ;; On reset the MMU is disabled, with block size set to 16K

MMU_16K
                lda     #%10000000                                ; 0000-3fff -> ram block 0
                sta     SBC09_MMU0 + 0
                lda     #%10000001                                ; 4000-7fff -> ram block 1
                sta     SBC09_MMU0 + 1
                lda     #%00000000                                ; 8000-bfff -> rom0 block 0
                sta     SBC09_MMU0 + 2
                lda     #%00000001                                ; c000-ffff -> rom0 block 1
                sta     SBC09_MMU0 + 3

      ;; Enable the MMU with 16K block size
                lda     #%00010000     ; op4 = low (mmu enabled, output is inverted)
                sta     SBC09_UART_OPRSET
		
		rts

_uart_readc:
		; unbuffered read - TODO: Use Hoglet's interrupt driven buffers

		lda	#SR_RxRDY
@lp:		bita	SBC09_UART_SRA
		beq	@lp
		ldb	SBC09_UART_RHRA
		clra
		tfr 	D,X

		jsr	_uart_writec

		rts

_uart_writec:
		; unbuffered write - TODO: Use Hoglet's interrupt driven buffers
		
		lda	#SR_TxRDY
@lp:		bita	SBC09_UART_SRA
		beq	@lp
		stb	SBC09_UART_THRA
		clrb
		rts                