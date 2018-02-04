/**	@file		debug.h
	@brief		Debug logging functions
	@details	See @link debug.h @endlink for details
*/

/************************************************************************/
/* AVR includes                                                         */
/************************************************************************/

#include <string.h>

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "math.h"
#include "uart.h"

/************************************************************************/
/* Internal variables                                                   */
/************************************************************************/

uart_struct_t s_debugUart;

/************************************************************************/
/* Internal functions                                                   */
/************************************************************************/

void byteToASCIIHex(u8* au8_result, u8 u8_byte)
{
	au8_result[0] = (u8_byte / 0x10 + '0' > '9')? u8_byte / 0x10 - 10 + 'A' : u8_byte / 0x10 + '0';
	au8_result[1] = (u8_byte % 0x10 + '0' > '9')? u8_byte % 0x10 - 10 + 'A' : u8_byte % 0x10 + '0';
}

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void debug_init()
{
	s_debugUart.baudRate = _57600;
	s_debugUart.frameSize = _8BIT;
	s_debugUart.parityBit = NONE;
	s_debugUart.peripheral = UART0;
	s_debugUart.stopBits = _1BIT;
	s_debugUart.useRx = FALSE;
	s_debugUart.useTx = TRUE;
	uart_init(s_debugUart);
	uart_start(s_debugUart);
}

void debug_writeChar(u8 u8_char)
{
	uart_transmit(s_debugUart, u8_char);
	uart_transmit(s_debugUart, '\n');
}

void debug_writeString(char* pc8_string)
{
	for (int i = 0; i < strlen(pc8_string); i++)
		uart_transmit(s_debugUart, pc8_string[i]);
	uart_transmit(s_debugUart, '\n');
}

void debug_writeHex(u8 u8_data)
{
	u8 u8_ASCIIData[2];
	byteToASCIIHex(u8_ASCIIData, u8_data);
	for (u8 i = 0; i < 2; i++)
	uart_transmit(s_debugUart, u8_ASCIIData[i]);
	uart_transmit(s_debugUart, '\n');
}

void debug_writeHexWord(u16 u16_data)
{
	u8 u8_ASCIIData[4];
	byteToASCIIHex(u8_ASCIIData, u16_data >> 8);
	byteToASCIIHex(u8_ASCIIData + 2, u16_data);
	for (u8 i = 0; i < 4; i++)
		uart_transmit(s_debugUart, u8_ASCIIData[i]);
	uart_transmit(s_debugUart, '\n');
}

void debug_writeHexDWord(u32 u32_data)
{
	u8 u8_ASCIIData[8];
	byteToASCIIHex(u8_ASCIIData, u32_data >> 24);
	byteToASCIIHex(u8_ASCIIData + 2, u32_data >> 16);
	byteToASCIIHex(u8_ASCIIData + 4, u32_data >> 8);
	byteToASCIIHex(u8_ASCIIData + 6, u32_data);
	for (u8 i = 0; i < 8; i++)
		uart_transmit(s_debugUart, u8_ASCIIData[i]);
	uart_transmit(s_debugUart, '\n');
}

void debug_writeDecimal(u16 u16_data)
{
	uart_transmit(s_debugUart, u16_data / 10000 % 10 + '0');
	uart_transmit(s_debugUart, u16_data / 1000 % 10 + '0');
	uart_transmit(s_debugUart, u16_data / 100 % 10 + '0');
	uart_transmit(s_debugUart, u16_data / 10 % 10 + '0');
	uart_transmit(s_debugUart, u16_data % 10 + '0');
	uart_transmit(s_debugUart, '\n');
}
