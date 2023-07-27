#ifndef __UART_H__
#define __UART_H__

extern void uart_init(void);
extern int uart_readc(void);
extern int uart_writec(char c);
extern int _uart_readc_nowait(void);
extern void uart_wait_100ms(void);
#endif
