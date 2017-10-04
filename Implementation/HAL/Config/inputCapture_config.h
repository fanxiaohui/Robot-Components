/**	@file		inputCapture_config.h
	@brief		Input Capture configuration
	@author		Florin Popescu
	@version	0.1
	@date		31.08.2017
	@details	Specifies the following:
				- which Timer peripherals are used
				- if interrupt modes are used
	These defines are used for code size and memory usage reduction, so set the defines accordingly.
*/

#ifndef INPUTCAPTURE_CONFIG_H_
#define INPUTCAPTURE_CONFIG_H_

/** Used Timer peripherals.
*/
//#define USING_INPUTCAPTURE1
//#define USING_INPUTCAPTURE3

/** Using interrupt mode for Timers
*/
#ifdef USING_INPUTCAPTURE1
	#define TIMER1_INTERRUPT_MODE
#endif
#ifdef USING_INPUTCAPTURE3
	#define TIMER3_INTERRUPT_MODE
#endif

/**	These defines inform the base timer layer that a timer is used. Do no modify!
*/
#ifdef USING_INPUTCAPTURE1
	#define USING_TIMER1
#endif
#ifdef USING_INPUTCAPTURE3
	#define USING_TIMER3
#endif

#endif /* INPUTCAPTURE_CONFIG_H_ */