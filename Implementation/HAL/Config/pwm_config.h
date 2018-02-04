/**	@file		pwm_config.h
	@brief		PWM configuration
	@details	Specifies the following:
				- which Timer peripherals are used
				- if interrupt modes are used
				These defines are used for code size and memory usage reduction, so set the defines accordingly.
*/

#ifndef PWM_CONFIG_H_
#define PWM_CONFIG_H_

/** Used Timer peripherals.
*/
//#define USING_PWM0
#define USING_PWM1
//#define USING_PWM2
//#define USING_PWM3

/** Using interrupt mode for Timers
*/
#ifdef USING_PWM0
	#define TIMER0_INTERRUPT_MODE
#endif
#ifdef USING_PWM1
	#define TIMER1_INTERRUPT_MODE
#endif
#ifdef USING_PWM2
	#define TIMER2_INTERRUPT_MODE
#endif
#ifdef USING_PWM3
	#define TIMER3_INTERRUPT_MODE
#endif

/**	These defines inform the base timer layer that a timer is used. Do no modify!
*/
#ifdef USING_PWM0
	#define USING_TIMER0
#endif
#ifdef USING_PWM1
	#define USING_TIMER1
#endif
#ifdef USING_PWM2
	#define USING_TIMER2
#endif
#ifdef USING_PWM3
	#define USING_TIMER3
#endif

#endif /* PWM_CONFIG_H_ */
