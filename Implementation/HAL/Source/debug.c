/*
 * debug.c
 *
 * Created: 11/4/2017 8:44:08 PM
 *  Author: Andrei
 */ 

#include <stdio.h>

#include "debug.h"
#include "uart.h"

FILE uart_str = FDEV_SETUP_STREAM(printCHAR, NULL, _FDEV_SETUP_RW);
uart_struct_t s_debugUart;

void uartInit()
{
	s_debugUart.baudRate = _9600;
	s_debugUart.frameSize = _8BIT;
	s_debugUart.parityBit = NONE;
	s_debugUart.peripheral = UART0;
	s_debugUart.stopBits = _1BIT;
	s_debugUart.useRx = FALSE;
	s_debugUart.useTx = TRUE;
	uart_init(s_debugUart);
	uart_start(s_debugUart);
}

int printCHAR(char data, FILE *stream)
{
	uartInit();
	uart_transmit(s_debugUart,data);
	return 0;
}

