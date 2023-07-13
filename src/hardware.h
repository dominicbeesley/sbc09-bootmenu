
#define SBC09_UART	0xFE00
#define SBC09_MMU0	0xFE10
#define SBC09_MMU1	0xFE20

#define mmu_16(x) (((unsigned char *)SBC09_MMU0)[x])

#define MMU_SEL_ROM		0x00
#define MMU_SEL_EXT 	0x40
#define MMU_SEL_RAM 	0x80
#define MMU_SEL_RAM_RO 	0xC0
#define MMU_MEM_MAX		0x3F

#define SBC09_UART_MRA		(SBC09_UART+0x0)
#define SBC09_UART_SRA		(SBC09_UART+0x1)
#define SBC09_UART_CSRA		(SBC09_UART+0x1)
#define SBC09_UART_CRA		(SBC09_UART+0x2)
#define SBC09_UART_THRA		(SBC09_UART+0x3)
#define SBC09_UART_RHRA		(SBC09_UART+0x3)
#define SBC09_UART_ACR		(SBC09_UART+0x4)
#define SBC09_UART_ISR		(SBC09_UART+0x5)
#define SBC09_UART_IMR		(SBC09_UART+0x5)
#define SBC09_UART_CTU		(SBC09_UART+0x6)
#define SBC09_UART_CTL		(SBC09_UART+0x7)
#define SBC09_UART_MRB		(SBC09_UART+0x8)
#define SBC09_UART_SRB		(SBC09_UART+0x9)
#define SBC09_UART_CSRB		(SBC09_UART+0x9)
#define SBC09_UART_CRB		(SBC09_UART+0xa)
#define SBC09_UART_THRB		(SBC09_UART+0xb)
#define SBC09_UART_RHRB		(SBC09_UART+0xb)
#define SBC09_UART_IVR1		(SBC09_UART+0xc)
#define SBC09_UART_IPR		(SBC09_UART+0xd)
#define SBC09_UART_OPCR		(SBC09_UART+0xd)
#define SBC09_UART_OPRSET	(SBC09_UART+0xe)
#define SBC09_UART_STARTCT	(SBC09_UART+0xe)
#define SBC09_UART_OPRCLR	(SBC09_UART+0xf)
#define SBC09_UART_STOPCT	(SBC09_UART+0xf)

#define SBC09_UART_RXINT			0x01
#define SBC09_UART_TXINT			0x04

#define MR1_RxRTS					0x80
#define MR1_RxINT					0x40
#define MR1_ERR_MODE				0x20
#define MR1_PARITY_MODE_ON			0x00
#define MR1_PARITY_MODE_FORCE		0x08
#define MR1_PARITY_MODE_OFF			0x10
#define MR1_PARITY_MODE_MULTI		0x18
#define MR1_PARITY_ODD				0x04
#define MR1_PARITY_BITS_5			0x00
#define MR1_PARITY_BITS_6			0x01
#define MR1_PARITY_BITS_7			0x02
#define MR1_PARITY_BITS_8			0x03

#define MR2_CHA_MODE_NORM			0x00
#define MR2_CHA_MODE_ECHO			0x40
#define MR2_CHA_MODE_LOCAL_LOOP		0x80
#define MR2_CHA_MODE_REMOTE_LOOP	0xC0
#define MR2_TxRTS					0x20
#define MR2_TxCTS					0x10
#define MR2_STOP_BITS_1				0x07
#define MR2_STOP_BITS_2				0x0F

#define SR_BRK						0x80
#define SR_FE						0x40
#define SR_PE						0x20
#define SR_OE						0x10
#define SR_TxEMT					0x08
#define SR_TxRDY					0x04
#define SR_FFULL					0x02
#define SR_RxRDY					0x01

#define CRA_CMD_NOP					0x00
#define CRA_CMD_RES_MR				0x10
#define CRA_CMD_RES_Rx				0x20
#define CRA_CMD_RES_Tx				0x30
#define CRA_CMD_RES_ERR				0x40
#define CRA_CMD_RES_CHA_BRK			0x50
#define CRA_CMD_START_BRK			0x60
#define CRA_CMD_STOP_BRK			0x70
#define CRA_CMD_SET_RxBRG			0x80
#define CRA_CMD_RES_RxBRG			0x90
#define CRA_CMD_SET_TxBRG			0xA0
#define CRA_CMD_RES_TxBRG			0xB0
#define CRA_CMD_STDBY				0xC0
#define CRA_CMD_ACT					0xD0

#define CRA_TxDIS					0x08
#define CRA_TxEN					0x04
#define CRA_RxDIS					0x02
#define CRA_RxEN					0x01

#define ACR_BRG						0x80
#define ACR_CT_MODE_CTR_IP2			0x00
#define ACR_CT_MODE_CTR_TxCA		0x10
#define ACR_CT_MODE_CTR_TxCB		0x20
#define ACR_CT_MODE_CTR_X1_DIV16	0x30
#define ACR_CT_MODE_SQ_IP2			0x40
#define ACR_CT_MODE_SQ_IP2_DIV16	0x50
#define ACR_CT_MODE_SQ_X1			0x60
#define ACR_CT_MODE_SQ_X1_DIV16		0x70
#define ACR_IP3_INT					0x08
#define ACR_IP2_INT					0x04
#define ACR_IP1_INT					0x02
#define ACR_IP0_INT					0x01

#define ISR_IP						0x80
#define ISR_BRK_B					0x40
#define ISR_RxRDY_B					0x20
#define ISR_TxRDY_B					0x10
#define ISR_CTR						0x08
#define ISR_BRK_A					0x04
#define ISR_RxRDY_A					0x02
#define ISR_TxRDY_A					0x01

#define IMR_IP						0x80
#define IMR_BRK_B					0x40
#define IMR_RxRDY_B					0x20
#define IMR_TxRDY_B					0x10
#define IMR_CTR						0x08
#define IMR_BRK_A					0x04
#define IMR_RxRDY_A					0x02
#define IMR_TxRDY_A					0x01

#define OPCR_O7_TxRDY_B				0x80
#define OPCR_O6_TxRDY_A				0x40
#define OPCR_O5_RxRDY_B				0x20
#define OPCR_O4_RxRDY_A				0x10
#define OPCR_O3_CT_OUT				0x04
#define OPCR_O3_TxCB				0x08
#define OPCR_O3_RxCB				0x0C
#define OPCR_O2_TxCA_MUL16			0x01
#define OPCR_O2_TxCA				0x02
#define OPCR_O2_RxCA				0x03

#define IPCR_DELTA_IP3				0x80
#define IPCR_DELTA_IP2				0x40
#define IPCR_DELTA_IP1				0x20
#define IPCR_DELTA_IP0				0x10
#define IPCR_IP3					0x08
#define IPCR_IP2					0x04
#define IPCR_IP1					0x02
#define IPCR_IP0					0x01

// board specific OP pins from UART
#define OP_BIT_RTS_A				0x01
#define OP_BIT_RTS_B				0x02
#define OP_BIT_LED					0x04
#define OP_BIT_MMU_16K				0x08
#define OP_BIT_nENMMU				0x10
#define OP_BIT_SDCLK				0x20
#define OP_BIT_SDMOSI				0x40
#define OP_BIT_SDCS					0x80

// board specific IP pins to UART
#define IP_BIT_CTS_A				0x01
#define IP_BIT_CTS_B				0x02
#define IP_BIT_JP1					0x04
#define IP_BIT_JP2					0x08
#define IP_BIT_JP3					0x10



//remapped vector location
#define	VECTOR_LOC					0xF7F0
#define VECTOR_ILL0					(VECTOR_LOC + 0x0)
#define	VECTOR_SWI3					(VECTOR_LOC + 0x2)
#define	VECTOR_SWI2					(VECTOR_LOC + 0x4)
#define	VECTOR_FIRQ					(VECTOR_LOC + 0x6)
#define	VECTOR_IRQ					(VECTOR_LOC + 0x8)
#define	VECTOR_SWI					(VECTOR_LOC + 0xA)					
#define	VECTOR_NMI					(VECTOR_LOC + 0xC)					
#define	VECTOR_RES					(VECTOR_LOC + 0xE)					
