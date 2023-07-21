
	include "hardware.inc"

	.globl	uart_init
	.globl	_uart_readc
	.globl	_uart_writec
	.globl  uart_writec_direct
	.globl  IRQ_HANDLER

	.area .text


FC_RTS_CTS	EQU 1

;; Threshold at which RTS is asserted
FC_LO_THRESH EQU $80

;; Threshold at which RTS is de-asserted
FC_HI_THRESH EQU $C0

UART_XTAL       EQU     3686400
UART_T0DIV      EQU     (((UART_XTAL/16)/100)/2)-1


uart_init:
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
                lda     #UART_T0DIV/256                           ; 16-bit write to counter to get a 100hz tick
                sta     SBC09_UART_CTU
                lda     #UART_T0DIV%256                           ; 16-bit write to counter to get a 100hz tick
                sta     SBC09_UART_CTU+1
                lda     #OP_BIT_RTS_A                             ; assert rts
                sta     SBC09_UART_OPRSET
                lda     #IMR_CTR|IMR_RxRDY_A                      ; timer int, rx int enabled; tx int disabled
                sta     SBC09_UART_IMR
                lda     SBC09_UART_STARTCT                        ; start the counter-timer

      ;; On reset the MMU is disabled, with block size set to 16K

MMU_16K
		cmpb	#$FF
		bne	MMU_BOOT

	; if b == FF then we're being run from BASIC, assume we are in RAM somewhere "low"
                lda     #%10000000                                ; 0000-3fff -> ram block 0
                sta     SBC09_MMU0 + 0
                lda     #%10000001                                ; 4000-7fff -> ram block 1
                sta     SBC09_MMU0 + 1
        ; ROM/MOS slots will get sorted later
		jmp	MMU_ENA
MMU_BOOT
	; assume that this is either a cold boot (in which case the stack is in low RAM)
	; or we're being booted from another instance 
                lda     #%10000000                                ; 0000-3fff -> ram block 0
                sta     SBC09_MMU0 + 0
	; B contains the MOS slot that we're running from
		stb	SBC09_MMU0 + 3

MMU_ENA
      ;; Enable the MMU with 16K block size
                lda     #%00010000     ; op4 = low (mmu enabled, output is inverted)
                sta     SBC09_UART_OPRSET

	
		rts

_uart_readc:
@l1:		lda	ubuf_rx_head
		cmpa	ubuf_rx_tail
		beq	@l1
		ldx	#ubuf_rx+0x80
		ldb	A,X
		inc	ubuf_rx_head
	IFDEF FC_RTS_CTS
		lda	ubuf_rx_tail		; Determine whethe RTS needs to be raised
		suba	ubuf_rx_head		; Tail - Head gives the receive buffer occupancy
		cmpa	#FC_LO_THRESH
		bne	@s2
		lda	#$01
		sta	SBC09_UART_OPRSET	; assert RTS
@s2:
   	ENDIF
   		clra
   		tfr	D,X
      		rts

_uart_writec:
@l1:		lda	ubuf_tx_tail 		; Is there space in the Tx buffer for one more character?
		inca	
		cmpa	ubuf_tx_head
		beq	@l1          		; No, then loop back and wait for characters to drain
		ldx	#ubuf_tx+0x80  		; Write the character to the tail of the Tx buffer
		stb	A,X
		sta	ubuf_tx_tail 		; Save the updated tail pointer
		lda	#IMR_CTR|IMR_RxRDY_A|IMR_TxRDY_A
						; Enable Tx interrupts to make sure buffer is serviced
		sta	SBC09_UART_IMR
		ldx	#0
		rts

; a special version that, with interrupts disabled, will write direct for when there's a crash
uart_writec_direct:
		lda	#SR_TxRDY
@lp:		bita	SBC09_UART_SRA
		beq	@lp
		stb	SBC09_UART_THRA
		clrb
		rts

;; *************************************************************
;; Main IRQ Handler
;; *************************************************************

IRQ_RX
      		lda	SBC09_UART_RHRA       	; Read UART Rx Data (and clear interrupt)
      		cmpa	#$1B            	; Test for escape
      		bne	IRQ_NOESC
      		ldb	#$80            	; Set the escape flag
      		stb 	ubuf_escape_flag

IRQ_NOESC
      		ldb	ubuf_rx_tail		; B = keyboard buffer tail index
      		ldx	#ubuf_rx+0x80		; X = keyboard buffer base address
      		sta	B,X			; store the character in the buffer
      		incb				; increment the tail pointer
      		cmpb	ubuf_rx_head		; has it hit the head (buffer full?)
      		beq	IRQ_HANDLER		; yes, then drop characters
      		stb	ubuf_rx_tail		; no, then save the incremented tail pointer

	;; Simple implementation of RTS/CTS to prevent receive buffer overflow
	IFDEF FC_RTS_CTS
		subb	ubuf_rx_head		; Tail - Head gives the receive buffer occupancy
		cmpb	#FC_HI_THRESH		; Compare with upper threshold
		bne	IRQ_HANDLER
		ldb	#$01
		stb	SBC09_UART_OPRCLR	; de-assert RTS
	ENDIF

IRQ_HANDLER

		lda	SBC09_UART_SRA		; Read UART status register
		bita	#SR_RxRDY		; Test bit 0 (RxRdy)
		bne	IRQ_RX			; Ready, branch back handle the character

		bita	#SR_TxRDY		; Test bit 2 (TxRdy)
		beq	IRQ_TIMER		; Not ready, branch forward to the timer check

	;; Simple implementation of XON/XOFF to prevent receive buffer overflow
	IFDEF FC_XON_XOFF
		ldb	ubuf_rx_tail		; Determine if we need to send XON or XOFF
		subb	ubuf_rx_head		; Tail - Head gives the receive buffer occupancy
		eorb	ubuff_xoff		; In XOFF state, complement to give some hysterisis
		cmpb	#$C0			; C=0 if occupancy >=75% (when in XON) or 25% (when in XOFF)
		bcs	IRQ_TX_CHAR		; Nothing to do...
		lda	#$11			; 0x11 = XON character
		com	ubuff_xoff		; toggle the XON/XOFF state
		beq	SEND_A			; Send XON
		lda	#$13			; 0x13 = XOFF character
		bra	SEND_A			; Send XOFF
	ENDIF

IRQ_TX_CHAR
		ldb	ubuf_tx_head		; Is the Tx buffer empty?
		cmpb	ubuf_tx_tail
		beq	IRQ_TX_EMPTY		; Yes, then disable Tx interrupts and exit
		ldx	#ubuf_tx+0x80		; No, then write the next character
		incb	
		stb	ubuf_tx_head
		lda	B,X
SEND_A
      		sta	SBC09_UART_THRA

IRQ_TIMER
		lda	SBC09_UART_ISR		; Read UART Interrupt Status Register
		anda	#ISR_CTR		; Check the timer bit
		beq	IRQ_EXIT
		lda	SBC09_UART_STOPCT	; Clear the interrupt
		inc	l_time			; Update the system clock
		bne	IRQ_EXIT
		inc	l_time+1
		bne	IRQ_EXIT
		inc	l_time+2
		bne	IRQ_EXIT
		inc	l_time+3

IRQ_EXIT
      		rti

IRQ_TX_EMPTY
		lda	#IMR_CTR|IMR_RxRDY_A	; Disable TX interrupts
		sta	SBC09_UART_IMR
		bra	IRQ_TIMER		
	
	.area .bss
ubuf_rx:		rmb 256
ubuf_tx:		rmb 256
ubuf_rx_head:		rmb 1
ubuf_rx_tail:		rmb 1
ubuf_tx_head:		rmb 1
ubuf_tx_tail:		rmb 1
ubuf_escape_flag:	rmb 1
l_time:			rmb 4
ubuff_xoff		rmb 1

