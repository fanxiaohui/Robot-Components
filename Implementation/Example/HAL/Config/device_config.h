/**	@file		device_config.h
	@brief		Basic device configuration
	@author		Florin Popescu
	@version	1.0
	@date		08.09.2017
	@details	Includes various defines used throughout the HAL. All defines included here must be treated accordingly to desired configuration.
*/

#ifndef DEVICE_CONFIG_H_
#define DEVICE_CONFIG_H_

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/** Clock frequency. Used for various purposes such as UART baud rate calculation.
*/
#define SYSTEM_CLOCK_FREQUENCY	8000000UL

/** Programming type. By default PC2...PC5 have internal pull-ups enabled and can only be used for JTAG functionality. To use these pins for other purposes JTAG has to be disabled and ISP programming has to be used. This is done in device_init if the following macro is defined.
	Uncomment it if you want to use JTAG functionality.
*/
//#define USING_JTAG_PROGRAMMING

#endif /* DEVICE_CONFIG_H_ */
