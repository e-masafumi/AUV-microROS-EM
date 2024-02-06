#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "func-uart.h"

const uint UART0_TX_PIN = 0; 
const uint UART0_RX_PIN = 1;
const uint UART1_TX_PIN = 4;
const uint UART1_RX_PIN = 5;

static int chars_rxed = 0;

void on_uart0_rx() {
    while (uart_is_readable(uart0)) {
        uint8_t ch = uart_getc(uart0);
        // Can we send it back?
        if (uart_is_writable(uart0)) {
            // Change it slightly first!
            ch++;
            uart_putc(uart0, ch);
        }
        chars_rxed++;
    }
}
void on_uart1_rx() {
    while (uart_is_readable(uart1)) {
        uint8_t ch = uart_getc(uart1);
        // Can we send it back?
        if (uart_is_writable(uart1)) {
            // Change it slightly first!
            ch++;
            uart_putc(uart1, ch);
        }
        chars_rxed++;
    }
}



int pico_uart::setup(uart_inst_t *uartPort, uint uartBaudrate, uint dataBit, uint stopBit){
	int uartTxPin = 1000;
	int uartRxPin = 1000;

	if(uartPort == uart0){
		uartTxPin = UART0_TX_PIN;
		uartRxPin = UART0_RX_PIN;
	}
	else if(uartPort == uart1){
		uartTxPin = UART1_TX_PIN;
		uartRxPin = UART1_RX_PIN;
	}
	else{
		printf("CHECK THE UART ID");
		return 1;
	}

	uart_init(uartPort, uartBaudrate);
	gpio_set_function(uartTxPin, GPIO_FUNC_UART);	//raspi mother ver1.0
	gpio_set_function(uartRxPin, GPIO_FUNC_UART);	//raspi mother ver1.0

	int actual = uart_set_baudrate(uartPort, uartBaudrate);
	uart_set_hw_flow(uartPort, false, false);
	uart_set_format(uartPort, dataBit, stopBit, UART_PARITY_NONE);

	uart_set_fifo_enabled(uartPort, false);

	int UART_IRQ = uart0 == uart0 ? UART0_IRQ : UART1_IRQ;
	if(uartPort == uart0){
		irq_set_exclusive_handler(UART_IRQ, on_uart0_rx);
	}
	else if(uartPort == uart1){
		irq_set_exclusive_handler(UART_IRQ, on_uart1_rx);
	}
	else{
		printf("CHECK THE UART ID");
		return 1;
	}
	irq_set_enabled(UART_IRQ, true);
	uart_set_irq_enables(uartPort, true, false);

	return actual;
}

