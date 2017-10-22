/**	@file		gpio_config.h
	@brief		GPIO configuration
	@author		Florin Popescu
	@version	1.0
	@date		11.09.2017
	@details	Specifies which kinds of external interrupts are enabled. Refer to datasheet for functionality details
	These defines are used for code size and memory usage reduction, so set the defines accordingly.
*/

#ifndef GPIO_CONFIG_H_
#define GPIO_CONFIG_H_

/** Used pin change interrupts
*/
//#define USING_PCINT0
//#define USING_PCINT1
#define USING_PCINT2

/** Using external edge or low level interrupts
*/
//#define USING_EXTINT

#endif /* GPIO_CONFIG_H_ */