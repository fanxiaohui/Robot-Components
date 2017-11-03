/**	@file		uart_config.h
	@brief		UART configuration
	@author		Florin Popescu
	@version	1.0
	@date		12.08.2017
	@details	Specifies the following:
	- which UART peripherals are used
	- if interrupt modes are used
	- sizes of ring buffers (only used in interrupt modes)
	These defines are used for code size and memory usage reduction, so set the defines accordingly.
*/

#ifndef UART_CONFIG_H_
#define UART_CONFIG_H_

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/** Used UART peripherals
*/
#define USING_UART0
//#define USING_UART1

/** Using interrupt mode for UARTs
*/
#ifdef USING_UART0
	//#define UART0_INTERRUPT_MODE
#endif
#ifdef USING_UART1
	#define UART1_INTERRUPT_MODE
#endif

/** Buffer sizes in case interrupt mode is used
*/
#ifdef USING_UART0
	#define UART_RX0_BUFFER_MAX_SIZE		10
	#define UART_TX0_BUFFER_MAX_SIZE		10
#endif

#ifdef USING_UART1
	#define UART_RX1_BUFFER_MAX_SIZE		10
	#define UART_TX1_BUFFER_MAX_SIZE		10
#endif

#endif /* UART_CONFIG_H_ */
