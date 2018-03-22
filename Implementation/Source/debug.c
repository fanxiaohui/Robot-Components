/**	@file		debug.h
	@brief		Debug logging functions
	@details	See @link debug.h @endlink for details
*/

/************************************************************************/
/* AVR includes                                                         */
/************************************************************************/

#include <stdlib.h>
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
	s_debugUart.baudRate = _19200;
	s_debugUart.frameSize = _8BIT;
	s_debugUart.parityBit = EVEN;
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
}

void debug_writeString(char* pc8_string)
{
	for (int i = 0; i < strlen(pc8_string); i++)
		uart_transmit(s_debugUart, pc8_string[i]);
}

void debug_writeHex(u8 u8_data)
{
	u8 u8_ASCIIData[2];
	byteToASCIIHex(u8_ASCIIData, u8_data);
	for (u8 i = 0; i < 2; i++)
	uart_transmit(s_debugUart, u8_ASCIIData[i]);
}

void debug_writeHexWord(u16 u16_data)
{
	u8 u8_ASCIIData[4];
	byteToASCIIHex(u8_ASCIIData, u16_data >> 8);
	byteToASCIIHex(u8_ASCIIData + 2, u16_data);
	for (u8 i = 0; i < 4; i++)
		uart_transmit(s_debugUart, u8_ASCIIData[i]);
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
}

void debug_writeDecimal(s16 s16_data)
{
	u16 u16_data;
	
	if (s16_data < 0)
	{
		debug_writeChar('-');
		u16_data = 0x10000 - s16_data;
	}
	else
	{
		debug_writeChar('+');
		u16_data = s16_data;
	}
	debug_writeChar(u16_data / 10000 % 10 + '0');
	debug_writeChar(u16_data / 1000 % 10 + '0');
	debug_writeChar(u16_data / 100 % 10 + '0');
	debug_writeChar(u16_data / 10 % 10 + '0');
	debug_writeChar(u16_data % 10 + '0');
}

void debug_writeFixedPoint(f24 f24_data)
{
	u32 integer = f24tos32_integer(f24_data);
	u32 decimal = f24tos32_decimal(f24_data);
	
	if (f24_data < 0)
		debug_writeChar('-');
	else
		debug_writeChar('+');
	
	debug_writeChar(integer / 10000 % 10 + '0');
	debug_writeChar(integer / 1000 % 10 + '0');
	debug_writeChar(integer / 100 % 10 + '0');
	debug_writeChar(integer / 10 % 10 + '0');
	debug_writeChar(integer % 10 + '0');
	debug_writeChar('.');
	debug_writeChar(decimal / 1000 % 10 + '0');
	debug_writeChar(decimal / 100 % 10 + '0');
	debug_writeChar(decimal / 10 % 10 + '0');
	debug_writeChar(decimal % 10 + '0');
}

void debug_writeNewLine()
{
	uart_transmit(s_debugUart, '\n');
}
