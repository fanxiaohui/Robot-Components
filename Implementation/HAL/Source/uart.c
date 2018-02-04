/**	@file		uart.c
	@brief		UART features
	@details	See @link uart.h @endlink for details.
*/

/************************************************************************/
/* AVR includes                                                         */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "device.h"
#include "math.h"
#include "uart.h"

/************************************************************************/
/* Internal variables                                                   */
/************************************************************************/

#ifdef USING_UART0
	volatile u16 au16_tx0Buffer[UART_TX0_BUFFER_MAX_SIZE];
	volatile u8 u8_tx0LastTransmittedIndex;
	volatile u8 u8_tx0LastRequestedIndex;
	volatile u16 au16_rx0Buffer[UART_RX0_BUFFER_MAX_SIZE];
	volatile u8 u8_rx0LastReceivedIndex;
	u8 u8_rx0LastProcessedIndex;
	volatile bool b_rx0Available;
#endif

#ifdef USING_UART1
	volatile u16 au16_tx1Buffer[UART_TX1_BUFFER_MAX_SIZE];
	volatile u8 u8_tx1LastTransmittedIndex;
	volatile u8 u8_tx1LastRequestedIndex;
	volatile u16 au16_rx1Buffer[UART_RX1_BUFFER_MAX_SIZE];
	volatile u8 u8_rx1LastReceivedIndex;
	u8 u8_rx1LastProcessedIndex;
	volatile bool b_rx1Available;
#endif

/************************************************************************/
/* Internal functions                                                   */
/************************************************************************/

u16 calculateUBRR(u32 u32_baudRate, bool *b_usingDoubleSpeed)
{
	u32 u32_ubrr;
	u8 u8_divisor = 16;
	u32 u32_actualBaudRate;

	u32_ubrr = F_CPU / u8_divisor / u32_baudRate - 1;
	u32_actualBaudRate = F_CPU / u8_divisor / (u32_ubrr + 1);

	if (calculateError(u32_actualBaudRate, u32_baudRate) >= 20)
	{
		u8_divisor = 8;
		*b_usingDoubleSpeed = TRUE;
		u32_ubrr = F_CPU / u8_divisor / u32_baudRate - 1;
		u32_actualBaudRate = F_CPU / u8_divisor / (u32_ubrr + 1);
	}
	return u32_ubrr;
}

/************************************************************************/
/* Interrupt handlers                                                   */
/************************************************************************/

#ifdef UART0_INTERRUPT_MODE
ISR(USART0_RX_vect)
{
	if (checkBit(UCSR0B, UCSZ02) == SET)
		au16_rx0Buffer[u8_rx0LastReceivedIndex] = ((UCSR0B & (1 << RXB80)) << 8) | UDR0;
	else
		au16_rx0Buffer[u8_rx0LastReceivedIndex] = UDR0;
	if (u8_rx0LastReceivedIndex < UART_RX0_BUFFER_MAX_SIZE - 1)
		u8_rx0LastReceivedIndex++;
	else
		u8_rx0LastReceivedIndex = 0;
	b_rx0Available = TRUE;
}

ISR(USART0_UDRE_vect)
{
	if (u8_tx0LastTransmittedIndex != u8_tx0LastRequestedIndex)
	{
		if (checkBit(UCSR0B, UCSZ02) == SET)
			UCSR0B |= (u8) (((au16_tx0Buffer[u8_tx0LastTransmittedIndex] >> 8) & 1) << TXB80);
		UDR0 = (u8) au16_tx0Buffer[u8_tx0LastTransmittedIndex];
		if (u8_tx0LastTransmittedIndex < UART_TX0_BUFFER_MAX_SIZE - 1)
			u8_tx0LastTransmittedIndex++;
		else
			u8_tx0LastTransmittedIndex = 0;
	}
	else
		clearBit(&UCSR0B, UDRIE0);
}

ISR(USART0_TX_vect)
{
	u8 i;
	for (i = 0; i < UART_TX0_BUFFER_MAX_SIZE; i++)
		au16_tx0Buffer[i] = 0;
	u8_tx0LastTransmittedIndex = 0;
	u8_tx0LastRequestedIndex = 0;
}
#endif

#ifdef UART1_INTERRUPT_MODE
ISR(USART1_RX_vect)
{
	if (checkBit(UCSR1B, UCSZ12) == SET)
		au16_rx1Buffer[u8_rx1LastReceivedIndex] = ((UCSR1B & (1 << RXB81)) << 8) | UDR1;
	else
		au16_rx1Buffer[u8_rx1LastReceivedIndex] = UDR1;
	if (u8_rx1LastReceivedIndex < UART_RX1_BUFFER_MAX_SIZE - 1)
		u8_rx1LastReceivedIndex++;
	else
		u8_rx1LastReceivedIndex = 0;
	b_rx1Available = TRUE;
}

ISR(USART1_UDRE_vect)
{
	if (u8_tx1LastTransmittedIndex != u8_tx1LastRequestedIndex)
	{
		if (checkBit(UCSR1B, UCSZ12) == SET)
			UCSR1B |= (u8) (((au16_tx1Buffer[u8_tx1LastTransmittedIndex] >> 8) & 1) << TXB81);
		UDR1 = (u8) au16_tx1Buffer[u8_tx1LastTransmittedIndex];
		if (u8_tx1LastTransmittedIndex < UART_TX1_BUFFER_MAX_SIZE - 1)
			u8_tx1LastTransmittedIndex++;
		else
			u8_tx1LastTransmittedIndex = 0;
	}
	else
		clearBit(&UCSR1B, UDRIE1);
}

ISR(USART1_TX_vect)
{
	u8 i;
	for (i = 0; i < UART_TX1_BUFFER_MAX_SIZE; i++)
		au16_tx1Buffer[i] = 0;
	u8_tx1LastTransmittedIndex = 1;
	u8_tx1LastRequestedIndex = 1;
}
#endif

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void uart_init(uart_struct_t s_uart)
{
	u16 u16_ubrr;
	bool b_usingDoubleSpeed = FALSE;

	switch (s_uart.peripheral)
	{
	case UART0:
	#ifdef USING_UART0
		/* Set asynchronous mode */
		UCSR0C &= ~(0b11 << UMSEL00);
		/* Set baud rate */
		u16_ubrr = calculateUBRR(s_uart.baudRate, &b_usingDoubleSpeed);
		UBRR0 = u16_ubrr;
		updateBit(&UCSR0A, U2X0, b_usingDoubleSpeed);
		/* Set frame size */
		switch (s_uart.frameSize)
		{
			case _5BIT:
				clearBit(&UCSR0C, UCSZ00);
				clearBit(&UCSR0C, UCSZ01);
				clearBit(&UCSR0B, UCSZ02);
				break;
			case _6BIT:
				setBit(&UCSR0C, UCSZ00);
				clearBit(&UCSR0C, UCSZ01);
				clearBit(&UCSR0B, UCSZ02);
				break;
			case _7BIT:
				clearBit(&UCSR0C, UCSZ00);
				setBit(&UCSR0C, UCSZ01);
				clearBit(&UCSR0B, UCSZ02);
				break;
			case _8BIT:
				setBit(&UCSR0C, UCSZ00);
				setBit(&UCSR0C, UCSZ01);
				clearBit(&UCSR0B, UCSZ02);
				break;
			case _9BIT:
				setBit(&UCSR0C, UCSZ00);
				setBit(&UCSR0C, UCSZ01);
				setBit(&UCSR0B, UCSZ02);
				break;
		}
		/* Set number of stop bits */
		updateBit(&UCSR0C, USBS0, s_uart.stopBits);
		/* Set parity check */
		if (s_uart.parityBit == NONE)
			UCSR0C &= ~(0b11 << UPM00);
		else
		{
			setBit(&UCSR0C, UPM01);
			updateBit(&UCSR0C, UPM00, s_uart.parityBit);
		}
	#ifdef UART0_INTERRUPT_MODE
		/* Set interrupts */
		if (s_uart.useRx)
		{
			u8_rx0LastProcessedIndex = 0;
			u8_rx0LastReceivedIndex = 0;
			b_rx0Available = FALSE;
			setBit(&UCSR0B, RXCIE0);
		}
		if (s_uart.useTx)
		{
			u8_tx0LastRequestedIndex = 0;
			u8_tx0LastTransmittedIndex = 0;
			setBit(&UCSR0B, TXCIE0);
			setBit(&UCSR0B, UDRIE0);
		}
	#endif
	#endif
		break;
	case UART1:
	#ifdef USING_UART1
		/* Set asynchronous mode */
		UCSR1C &= ~(0b11 << UMSEL10);
		/* Set baud rate */
		u16_ubrr = calculateUBRR(s_uart.baudRate, &b_usingDoubleSpeed);
		UBRR1 = u16_ubrr;
		updateBit(&UCSR1A, U2X1, b_usingDoubleSpeed);
		/* Set frame size */
		switch (s_uart.frameSize)
		{
			case _5BIT:
				clearBit(&UCSR1C, UCSZ10);
				clearBit(&UCSR1C, UCSZ11);
				clearBit(&UCSR1B, UCSZ12);
				break;
			case _6BIT:
				setBit(&UCSR1C, UCSZ10);
				clearBit(&UCSR1C, UCSZ11);
				clearBit(&UCSR1B, UCSZ12);
				break;
			case _7BIT:
				clearBit(&UCSR1C, UCSZ10);
				setBit(&UCSR1C, UCSZ11);
				clearBit(&UCSR1B, UCSZ12);
				break;
			case _8BIT:
				setBit(&UCSR1C, UCSZ10);
				setBit(&UCSR1C, UCSZ11);
				clearBit(&UCSR1B, UCSZ12);
				break;
			case _9BIT:
				setBit(&UCSR1C, UCSZ10);
				setBit(&UCSR1C, UCSZ11);
				setBit(&UCSR1B, UCSZ12);
				break;
		}
		/* Set number of stop bits */
		updateBit(&UCSR1C, USBS1, s_uart.stopBits);
		/* Set parity check */
		if (s_uart.parityBit == NONE)
			UCSR1C &= ~(0b11 << UPM10);
		else
		{
			setBit(&UCSR1C, UPM11);
			updateBit(&UCSR1C, UPM10, s_uart.parityBit);
		}
	#ifdef UART1_INTERRUPT_MODE
		/* Set interrupts */
		if (s_uart.useRx)
		{
			u8_rx1LastProcessedIndex = 0;
			u8_rx1LastReceivedIndex = 0;
			b_rx1Available = FALSE;
			setBit(&UCSR1B, RXCIE1);
		}
		if (s_uart.useTx)
		{
			u8_tx1LastRequestedIndex = 0;
			u8_tx1LastTransmittedIndex = 0;
			setBit(&UCSR1B, TXCIE1);
			setBit(&UCSR1B, UDRIE1);
		}
	#endif
#endif
		break;
	}
}

void uart_start(uart_struct_t s_uart)
{
	switch (s_uart.peripheral)
	{
		case UART0:
		#ifdef USING_UART0
			updateBit(&UCSR0B, RXEN0, s_uart.useRx);
			updateBit(&UCSR0B, TXEN0, s_uart.useTx);
		#endif
			break;
		case UART1:
		#ifdef USING_UART1
			updateBit(&UCSR1B, RXEN1, s_uart.useRx);
			updateBit(&UCSR1B, TXEN1, s_uart.useTx);
		#endif
			break;
	}
}

void uart_stop(uart_struct_t s_uart)
{
	switch (s_uart.peripheral)
	{
		case UART0:
		#ifdef USING_UART0
			clearBit(&UCSR0B, RXEN0);
			clearBit(&UCSR0B, TXEN0);
			while (checkBit(UCSR0B, RXEN0) == SET);
			while (checkBit(UCSR0B, TXEN0) == SET);
		#endif
			break;
		case UART1:
		#ifdef USING_UART1
			clearBit(&UCSR1B, RXEN1);
			clearBit(&UCSR1B, TXEN1);
			while (checkBit(UCSR1B, RXEN1) == SET);
			while (checkBit(UCSR1B, TXEN1) == SET);
		#endif
			break;
	}
}

void uart_transmit(uart_struct_t s_uart, u16 u16_data)
{
	switch (s_uart.peripheral)
	{
		case UART0:
		#ifdef USING_UART0
		#ifdef UART0_INTERRUPT_MODE
			while (checkBit(UCSR0A, UDRE0) == CLEAR);
			au16_tx0Buffer[u8_tx0LastRequestedIndex] = u16_data;
			if (u8_tx0LastRequestedIndex < UART_TX0_BUFFER_MAX_SIZE - 1)
				u8_tx0LastRequestedIndex++;
			else
				u8_tx0LastRequestedIndex = 0;
			if (checkBit(UCSR0A, UDRE0) == SET)
			{
				if (u8_tx0LastTransmittedIndex < UART_TX0_BUFFER_MAX_SIZE - 1)
					u8_tx0LastTransmittedIndex++;
				else
					u8_tx0LastTransmittedIndex = 0;
				setBit(&UCSR0B, UDRIE0);
				if (s_uart.frameSize == _9BIT)
					UCSR0B |= (((u16_data >> 8) & 1) << TXB80);
				UDR0 = (u8) u16_data;
			}
		#else
			while (checkBit(UCSR0A, UDRE0) == CLEAR);
			if (s_uart.frameSize == _9BIT)
			updateBit(&UCSR0B, TXB80, (u8) ((u16_data >> 8) & 1));
			UDR0 = (u8) u16_data;
		#endif
		#endif
			break;
		case UART1:
		#ifdef USING_UART1
		#ifdef UART1_INTERRUPT_MODE
			while (checkBit(UCSR1A, UDRE1) == CLEAR);
			au16_tx1Buffer[u8_tx1LastRequestedIndex] = u16_data;
			if (u8_tx1LastRequestedIndex < UART_TX1_BUFFER_MAX_SIZE - 1)
				u8_tx1LastRequestedIndex++;
			else
				u8_tx1LastRequestedIndex = 0;
			if (checkBit(UCSR1A, UDRE1) == SET)
			{
				if (u8_tx1LastTransmittedIndex < UART_TX1_BUFFER_MAX_SIZE - 1)
					u8_tx1LastTransmittedIndex++;
				else
					u8_tx1LastTransmittedIndex = 0;
				setBit(&UCSR1B, UDRIE1);
				if (s_uart.frameSize == _9BIT)
					UCSR1B |= (((u16_data >> 8) & 1) << TXB81);
				UDR1 = (u8) u16_data;
			}
		#else
			while (checkBit(UCSR1A, UDRE1) == CLEAR);
			if (s_uart.frameSize == _9BIT)
				updateBit(&UCSR1B, TXB81, (u8) ((u16_data >> 8) & 1));
			UDR1 = (u8) u16_data;
		#endif
		#endif
			break;
	}
}

bool uart_receiveAvailable(uart_struct_t s_uart)
{
	switch (s_uart.peripheral)
	{
		case UART0:
		#ifdef UART0_INTERRUPT_MODE
			return b_rx0Available;
		#endif
			break;
		case UART1:
		#ifdef UART1_INTERRUPT_MODE
			return b_rx1Available;
		#endif
			break;
	}
	return TRUE;
}

u16 uart_receive(uart_struct_t s_uart)
{
	u16 u16_returnValue = 0;

	switch (s_uart.peripheral)
	{
		case UART0:
		#ifdef USING_UART0
		#ifdef UART0_INTERRUPT_MODE
			u16_returnValue = au16_rx0Buffer[u8_rx0LastProcessedIndex];
			if (u8_rx0LastProcessedIndex < UART_RX0_BUFFER_MAX_SIZE - 1)
				u8_rx0LastProcessedIndex++;
			else
				u8_rx0LastProcessedIndex = 0;
			if (u8_rx0LastProcessedIndex == u8_rx0LastReceivedIndex)
				b_rx0Available = FALSE;
		#else
			while (checkBit(UCSR0A, RXC0) == CLEAR);
			if (s_uart.frameSize == _9BIT)
				u16_returnValue = (UCSR0B & (1 << RXB80)) << 8;
			u16_returnValue |= UDR0;
		#endif
		#endif
			break;
		case UART1:
		#ifdef USING_UART1
		#ifdef UART1_INTERRUPT_MODE
			u16_returnValue = au16_rx1Buffer[u8_rx1LastProcessedIndex];
			if (u8_rx1LastProcessedIndex < UART_RX1_BUFFER_MAX_SIZE - 1)
				u8_rx1LastProcessedIndex++;
			else
				u8_rx1LastProcessedIndex = 0;
			if (u8_rx1LastProcessedIndex == u8_rx1LastReceivedIndex)
				b_rx1Available = FALSE;
		#else
			while (checkBit(UCSR1A, RXC1) == CLEAR);
			if (s_uart.frameSize == _9BIT)
				u16_returnValue = (UCSR1B & (1 << RXB81)) << 8;
			u16_returnValue |= UDR1;
		#endif
		#endif
			break;
	}

	return u16_returnValue;
}
