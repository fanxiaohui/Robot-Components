/**	@file		uart.h
	@brief		UART features
	@details	Supports both USART peripherals in asynchronous mode, with or without interrupts. Interrupt mode is implemented using separate ring buffers for receive and transmission for both peripherals.
				Basic flow:
				1. Initialize a @link uart_struct_t @endlink.
				2. Pass it to @link uart_init @endlink. Remember to check its return value for errors.
				3. Call @link uart_start @endlink.
				4. Use @link uart_transmit @endlink and @link uart_receive @endlink to communicate.
				- If using @link uart_receive @endlink in interrupt mode, first call @link uart_receiveAvailable @endlink to check if there was anything actually received.
				- To stop the UART (for power saving for instance) call @link uart_stop @endlink.
*/

#ifndef UART_H_
#define UART_H_

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "types.h"

/** @example uart_config.h
	This is an example of how to configure the UART driver
*/
#include "uart_config.h"

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/** Specifies the peripheral to use. The only difference between the two are their pins. Refer to microcontroller documentation for actual capabilities, input/output pins etc.
*/
typedef enum uart_peripheral_enum_t
{
/**	UART0 peripheral */
	UART0,
/**	UART1 peripheral */
	UART1
}uart_peripheral_enum_t;

/** Specifies standard baud rates.
*/
typedef enum uart_baudRate_enum_t
{
/**	A baud rate of 9600 */
	_9600 = 9600,
/**	A baud rate of 19200 */
	_19200 = 19200,
/**	A baud rate of 38400 */
	_38400 = 38400,
/**	A baud rate of 57600 */
	_57600 = 57600,
/**	A baud rate of 74880 */
	_74880 = 74880,
/**	A baud rate of 115200 */
	_115200 = 115200
}uart_baudRate_enum_t;

/** Specifies number of data bits in a frame.
*/
typedef enum uart_frameSize_enum_t
{
/**	A frame contains 5 data bits. */
	_5BIT,
/**	A frame contains 6 data bits. */
	_6BIT,
/**	A frame contains 7 data bits. */
	_7BIT,
/**	A frame contains 8 data bits. */
	_8BIT,
/**	A frame contains 9 data bits. */
	_9BIT
}uart_frameSize_enum_t;

/** Specifies number of stop bits in a frame.
*/
typedef enum uart_stopBits_enum_t
{
/**	A frame contains 1 stop bit. */
	_1BIT = 0,
/**	A frame contains 2 stop bits. */
	_2BIT = 1
}uart_stopBits_enum_t;

/** Specifies number and type of parity bits in a frame.
*/
typedef enum uart_parity_enum_t
{
/**	Parity bit is 0 if there is an even number of bits in the frame. */
	EVEN = 0,
/**	Parity bit is 1 if there is an even number of bits in the frame. */
	ODD = 1,
/** No parity check is done */
	NONE
}uart_parity_enum_t;

/** UART configuration structure
*/
typedef struct uart_struct_t
{
/** Which timer peripheral is configured */
	uart_peripheral_enum_t peripheral;

/**	Baud rate to be used. Can be a custom value or one of the usual ones defined in @link uart_baudRate_enum_t @endlink */
	u32 baudRate;

/** Specifies whether receive functionality is used */
	bool useRx;

/** Specifies whether transmit functionality is used */
	bool useTx;

/**	Frame size to use */
	uart_frameSize_enum_t frameSize;

/**	Frame size to use */
	uart_stopBits_enum_t stopBits;

/**	Whether to use parity check */
	 uart_parity_enum_t parityBit;
}uart_struct_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Initializes UART peripheral.
	@remark		Must be called before any other function for the required UART.
	@param[in]	s_uart: UART parameters to initialize
*/
void uart_init(uart_struct_t s_uart);

/** Starts UART peripheral.
	@pre		Must be called after the UART was initialized (with @link uart_init @endlink).
	@param[in]	s_uart: UART peripheral to use
*/
void uart_start(uart_struct_t s_uart);

/** Stops UART peripheral.
	@pre		Must be called after the UART was initialized (with @link uart_init @endlink).
	@param[in]	s_uart: UART peripheral to use
*/
void uart_stop(uart_struct_t s_uart);

/** Transmits a frame on UART. In interrupt mode it saves the frame to the internal ring buffer and may not send it immediately. In normal mode if there is a frame already in the transmit buffer it first waits for it to finish sending.
	@pre		Must be called after the UART was initialized (with @link uart_init @endlink) with transmission enabled.
	@param[in]	s_uart: UART peripheral to use
	@param[in]	u16_data: frame to send
*/
void uart_transmit(uart_struct_t s_uart, u16 u16_data);

/**	Checks if there is a received frame in the ring buffer that hasn't been processed yet.
	@pre		Must be called after the UART was initialized (with @link uart_init @endlink) with receive enabled.
	@param[in]	s_uart: UART peripheral to use
	@return		TRUE if there is an unprocessed frame
@*/
bool uart_receiveAvailable(uart_struct_t s_uart);

/** Receives a frame on UART. In interrupt mode it returns the first frame from the internal ring buffer and the interrupt takes care of the actual reception. In normal mode it waits until a frame is received
	@pre		Must be called after the UART was initialized (with @link uart_init @endlink) with receive enabled.
	@pre		If interrupt mode is used, must be called after checking that there is a frame received but not yet processed in the ring buffer (with @link uart_receiveAvailable @endlink) else it will just return 0
	@param[in]	s_uart: UART peripheral to use
	@return		frame received
*/
u16 uart_receive(uart_struct_t s_uart);

#endif /* UART_H_ */
