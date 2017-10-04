/**	@file		counter_config.h
	@brief		Counter configuration
	@author		Florin Popescu
	@version	1.0
	@date		08.09.2017
	@details	Specifies the following:
				- which Timer peripherals are used
				- if interrupt modes are used
	These defines are used for code size and memory usage reduction, so set the defines accordingly.
*/

#ifndef COUNTER_CONFIG_H_
#define COUNTER_CONFIG_H_

/** Used Timer peripherals.
*/
//#define USING_COUNTER0
//#define USING_COUNTER1
//#define USING_COUNTER3

/** Using interrupt mode for Timers
*/
#ifdef USING_COUNTER0
	#define TIMER0_INTERRUPT_MODE
#endif
#ifdef USING_COUNTER1
	#define TIMER1_INTERRUPT_MODE
#endif
#ifdef USING_COUNTER3
	#define TIMER3_INTERRUPT_MODE
#endif

/**	These defines inform the base timer layer that a timer is used. Do no modify!
*/
#ifdef USING_COUNTER0
	#define USING_TIMER0
#endif
#ifdef USING_COUNTER1
	#define USING_TIMER1
#endif
#ifdef USING_COUNTER3
	#define USING_TIMER3
#endif

#endif /* COUNTER_CONFIG_H_ */