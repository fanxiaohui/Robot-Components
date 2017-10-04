/**	@file		timer_config.h
	@brief		Timer configuration
	@author		Florin Popescu
	@version	1.0
	@date		08.09.2017
	@details	Specifies the following:
				- which Timer peripherals are used (only if timer mode)
				- if interrupt modes are used
	@remark		If using other modes for the timers, their specific config files handle definitions accordingly and no change has to be made to this file
	These defines are used for code size and memory usage reduction, so set the defines accordingly.
*/

#ifndef TIMER_CONFIG_H_
#define TIMER_CONFIG_H_

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "asyncTimer_config.h"
#include "counter_config.h"
#include "inputCapture_config.h"
#include "pwm_config.h"

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/** Used Timer peripherals.
*/
//#define USING_TIMER0
//#define USING_TIMER1
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