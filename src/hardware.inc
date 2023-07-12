
SBC09_UART                      EQU     $FE00
SBC09_MMU0                      EQU     $FE10
SBC09_MMU1                      EQU     $FE20

SBC09_UART_MRA                  EQU     SBC09_UART+0x0
SBC09_UART_SRA                  EQU     SBC09_UART+0x1
SBC09_UART_CSRA                 EQU     SBC09_UART+0x1
SBC09_UART_CRA                  EQU     SBC09_UART+0x2
SBC09_UART_THRA                 EQU     SBC09_UART+0x3
SBC09_UART_RHRA                 EQU     SBC09_UART+0x3
SBC09_UART_ACR                  EQU     SBC09_UART+0x4
SBC09_UART_ISR                  EQU     SBC09_UART+0x5
SBC09_UART_IMR                  EQU     SBC09_UART+0x5
SBC09_UART_CTU                  EQU     SBC09_UART+0x6
SBC09_UART_CTL                  EQU     SBC09_UART+0x7
SBC09_UART_MRB                  EQU     SBC09_UART+0x8
SBC09_UART_SRB                  EQU     SBC09_UART+0x9
SBC09_UART_CSRB                 EQU     SBC09_UART+0x9
SBC09_UART_CRB                  EQU     SBC09_UART+0xa
SBC09_UART_THRB                 EQU     SBC09_UART+0xb
SBC09_UART_RHRB                 EQU     SBC09_UART+0xb
SBC09_UART_IVR1                 EQU     SBC09_UART+0xc
SBC09_UART_IPR                  EQU     SBC09_UART+0xd ; read input port
SBC09_UART_OPCR                 EQU     SBC09_UART+0xd ; write
SBC09_UART_OPRSET               EQU     SBC09_UART+0xe ; write
SBC09_UART_STARTCT              EQU     SBC09_UART+0xe ; read command
SBC09_UART_OPRCLR               EQU     SBC09_UART+0xf ; write
SBC09_UART_STOPCT               EQU     SBC09_UART+0xf ; read command

SBC09_UART_RXINT                EQU     $01
SBC09_UART_TXINT                EQU     $04



MR1_RxRTS                       EQU     $80
MR1_RxINT                       EQU     $40
MR1_ERR_MODE                    EQU     $20
MR1_PARITY_MODE_ON              EQU     $00
MR1_PARITY_MODE_FORCE           EQU     $08
MR1_PARITY_MODE_OFF             EQU     $10
MR1_PARITY_MODE_MULTI           EQU     $18
MR1_PARITY_ODD                  EQU     $04
MR1_PARITY_BITS_5               EQU     $00
MR1_PARITY_BITS_6               EQU     $01
MR1_PARITY_BITS_7               EQU     $02
MR1_PARITY_BITS_8               EQU     $03

MR2_CHA_MODE_NORM               EQU     $00
MR2_CHA_MODE_ECHO               EQU     $40
MR2_CHA_MODE_LOCAL_LOOP         EQU     $80
MR2_CHA_MODE_REMOTE_LOOP        EQU     $C0
MR2_TxRTS                       EQU     $20
MR2_TxCTS                       EQU     $10
MR2_STOP_BITS_1                 EQU     $07
MR2_STOP_BITS_2                 EQU     $0F

SR_BRK                          EQU     $80
SR_FE                           EQU     $40
SR_PE                           EQU     $20
SR_OE                           EQU     $10
SR_TxEMT                        EQU     $08
SR_TxRDY                        EQU     $04
SR_FFULL                        EQU     $02
SR_RxRDY                        EQU     $01

CRA_CMD_NOP                     EQU     $00
CRA_CMD_RES_MR                  EQU     $10
CRA_CMD_RES_Rx                  EQU     $20
CRA_CMD_RES_Tx                  EQU     $30
CRA_CMD_RES_ERR                 EQU     $40
CRA_CMD_RES_CHA_BRK             EQU     $50
CRA_CMD_START_BRK               EQU     $60
CRA_CMD_STOP_BRK                EQU     $70
CRA_CMD_SET_RxBRG               EQU     $80
CRA_CMD_RES_RxBRG               EQU     $90
CRA_CMD_SET_TxBRG               EQU     $A0
CRA_CMD_RES_TxBRG               EQU     $B0
CRA_CMD_STDBY                   EQU     $C0
CRA_CMD_ACT                     EQU     $D0

CRA_TxDIS                       EQU     $08
CRA_TxEN                        EQU     $04
CRA_RxDIS                       EQU     $02
CRA_RxEN                        EQU     $01

ACR_BRG                         EQU     $80
ACR_CT_MODE_CTR_IP2             EQU     $00
ACR_CT_MODE_CTR_TxCA            EQU     $10
ACR_CT_MODE_CTR_TxCB            EQU     $20
ACR_CT_MODE_CTR_X1_DIV16        EQU     $30
ACR_CT_MODE_SQ_IP2              EQU     $40
ACR_CT_MODE_SQ_IP2_DIV16        EQU     $50
ACR_CT_MODE_SQ_X1               EQU     $60
ACR_CT_MODE_SQ_X1_DIV16         EQU     $70
ACR_IP3_INT                     EQU     $08
ACR_IP2_INT                     EQU     $04
ACR_IP1_INT                     EQU     $02
ACR_IP0_INT                     EQU     $01

ISR_IP                          EQU     $80
ISR_BRK_B                       EQU     $40
ISR_RxRDY_B                     EQU     $20
ISR_TxRDY_B                     EQU     $10
ISR_CTR                         EQU     $08
ISR_BRK_A                       EQU     $04
ISR_RxRDY_A                     EQU     $02
ISR_TxRDY_A                     EQU     $01

IMR_IP                          EQU     $80
IMR_BRK_B                       EQU     $40
IMR_RxRDY_B                     EQU     $20
IMR_TxRDY_B                     EQU     $10
IMR_CTR                         EQU     $08
IMR_BRK_A                       EQU     $04
IMR_RxRDY_A                     EQU     $02
IMR_TxRDY_A                     EQU     $01

OPCR_O7_TxRDY_B                 EQU     $80
OPCR_O6_TxRDY_A                 EQU     $40
OPCR_O5_RxRDY_B                 EQU     $20
OPCR_O4_RxRDY_A                 EQU     $10
OPCR_O3_CT_OUT                  EQU     $04
OPCR_O3_TxCB                    EQU     $08
OPCR_O3_RxCB                    EQU     $0C
OPCR_O2_TxCA_MUL16              EQU     $01
OPCR_O2_TxCA                    EQU     $02
OPCR_O2_RxCA                    EQU     $03

IPCR_DELTA_IP3                  EQU     $80
IPCR_DELTA_IP2                  EQU     $40
IPCR_DELTA_IP1                  EQU     $20
IPCR_DELTA_IP0                  EQU     $10
IPCR_IP3                        EQU     $08
IPCR_IP2                        EQU     $04
IPCR_IP1                        EQU     $02
IPCR_IP0                        EQU     $01

; board specific OP pins from UART
OP_BIT_RTS_A    EQU     $01
OP_BIT_RTS_B    EQU     $02
OP_BIT_LED      EQU     $04
OP_BIT_MMU_16K  EQU     $08
OP_BIT_nENMMU   EQU     $10
OP_BIT_SDCLK    EQU     $20
OP_BIT_SDMOSI   EQU     $40
OP_BIT_SDCS     EQU     $80

; board specific IP pins to UART
IP_BIT_CTS_A    EQU     $01
IP_BIT_CTS_B    EQU     $02
IP_BIT_JP1      EQU     $04
IP_BIT_JP2      EQU     $08
IP_BIT_JP3      EQU     $10
