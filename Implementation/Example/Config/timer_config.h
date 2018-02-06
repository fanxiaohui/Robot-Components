/**	@file		timer_config.h
	@brief		Timer configuration
	@details	Specifies the following:
				- which Timer peripherals are used (define only if used in timer mode)
				- if interrupt modes are used
				These defines are used for code size and memory usage reduction, so set the defines accordingly.
	@remark		If using other modes for the timers, their specific config files handle definitions accordingly and no change has to be made to this file
*/

#ifndef TIMER_CONFIG_H_
#define TIMER_CONFIG_H_

#include "asyncTimer_config.h"
#include "counter_config.h"
#include "inputCapture_config.h"
#include "pwm_config.h"

/** Used Timer peripherals.
*/
//#define USING_TIMER0
#define USING_TIMER1
//#define USING_TIMER2
//#define USING_TIMER3

/** Using interrupt mode for Timers
*/
#ifdef USING_TIMER0
	#define TIMER0_INTERRUPT_MODE
#endif
#ifdef USING_TIMER1
	#define TIMER1_INTERRUPT_MODE
#endif
#ifdef USING_TIMER2
	#define TIMER2_INTERRUPT_MODE
#endif
#ifdef USING_TIMER3
	#define TIMER3_INTERRUPT_MODE
#endif

#endif /* TIMER_CONFIG_H_ */
