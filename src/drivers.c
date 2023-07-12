
extern void uart_init(void);
extern int uart_readc(void);
extern int uart_writec(char);

const char *message = "Hello";

void init_hardware(void) {

	uart_init();
	
}