/**	@file		asyncTimer_config.h
	@brief		Asynchronous Timer configuration
	@details	Specifies the following:
				- if interrupt modes are used
				These defines are used for code size and memory usage reduction, so set the defines accordingly.
*/

#ifndef ASYNCTIMER_CONFIG_H_
#define ASYNCTIMER_CONFIG_H_

/** Using interrupt mode for Timer
*/
#define TIMER2_INTERRUPT_MODE

/**	These defines inform the base timer layer that a timer is used. Do no modify!
*/
#define USING_TIMER2

#endif /* ASYNCTIMER_CONFIG_H_ */
