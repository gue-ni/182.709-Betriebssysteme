#include "mini_uart.h"

void kernel_main(void)
{
	uart_init();
	uart_send_string("Hello, I'm the raspberry pi kernel!\r\n");

	while (1) {
		uart_send(uart_recv());
	}
}
