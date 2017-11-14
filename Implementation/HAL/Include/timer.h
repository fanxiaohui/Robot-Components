/**	@file		timer.h
	@brief		Timer features
	@author		Florin Popescu
	@version	1.0
	@date		08.09.2017
	@details	Supports all 4 timers as regular timers with a fixed or custom frequency
				Basic flow:
				1. Initialize a @link timer_struct_t @endlink with either a fixed (set timer_struct_t.frequency to a value of type @link timer_prescaler_enum_t @endlink) or a custom frequency (set to any unsigned value).
				2. Pass it to @link timer_init @endlink.
				3. Call @link timer_start @endlink.
				4. Call @link timer_getValue @endlink or @link timer_resetValue @endlink whenever needed.
				- It may be useful to synchronously start all configured timers. To do this, call @link timer_prepareSynchronisedStart @endlink after all timers are initialised, afterwards call @link timer_start @endlink for each configured timer, followed by @link timer_synchronisedStart @endlink.
				- Attach a function to the timer interrupt handlers with @link timer_attachInterrupt @endlink prior to calling @link timer_enableInterrupt @endlink.
				- To stop the timer call @link timer_stop @endlink.
	@remark		When using @link timer_attachInterrupt @endlink, setting to @link OVERFLOW @endlink will implement the interrupt when the timer resets, not when it actually overflows. That means if OCRxA or ICRx is used as maximum value for the timer, that corresponding interrupt handler is actually the one that calls the callback passed to @link OVERFLOW @endlink interrupt. Likewise, interrupt callbacks set for @link COMPARE_MATCH_A @endlink or @link EXTERNAL @endlink will be called by the corresponding interrupt handlers only if those registers are not used as maximum value for the timer. This is done to provide an abstraction to support microcontroller families that never have compare match registers used as maximum value.
*/

#ifndef TIM_H_
#define TIM_H_

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "types.h"

/** @example timer_config.h
	This is an example of how to configure the Timer driver
*/
#include "timer_config.h"

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/** Specifies the peripheral to use. Refer to microcontroller documentation for timer capabilities, input/output pins etc.
*/
typedef enum timer_peripheral_enum_t
{
/** 8bit. Supports TIMER, COUNTER, FAST and PHASE CORRECT PWM modes. */
	TIMER0,
/** 16bit. Supports TIMER, COUNTER, INPUT CAPTURE, FAST and PHASE CORRECT PWM modes. */
	TIMER1,
/** 16bit. Supports TIMER, COUNTER, INPUT CAPTURE, FAST and PHASE CORRECT PWM modes. */
	TIMER2,
/** 8bit. Supports TIMER, COUNTER, FAST and PHASE CORRECT PWM modes. Can be used as wake up source from sleep. */
	TIMER3
}timer_peripheral_enum_t;

/** Specifies fixed frequencies for the timer clock (timer counts until overflow instead of a custom value).
*/
typedef enum timer_prescaler_enum_t
{
/** 1:1 division factor from IO clock */
	TIMER_CLK_DIV_1 = 0xfffff9,
/** 1:8 division factor from IO clock */
	TIMER_CLK_DIV_8 = 0xfffffa,
/** 1:32 division factor from IO clock */
	TIMER_CLK_DIV_32 = 0xfffffb,
/** 1:64 division factor from IO clock */
	TIMER_CLK_DIV_64 = 0xfffffc,
/** 1:128 division factor from IO clock */
	TIMER_CLK_DIV_128 = 0xfffffd,
/** 1:256 division factor from IO clock */
	TIMER_CLK_DIV_256 = 0xfffffe,
/** 1:1024 division factor from IO clock */
	TIMER_CLK_DIV_1024 = 0xffffff
}timer_prescaler_enum_t;

/** Specifies the interrupt type to use.
*/
typedef enum timer_interruptType_enum_t
{
/** When timer overflows
	@remark This may not necessarily be handled in the timer overflow interrupt. See remark at module description for more details. */
	OVERFLOW = 0,
/** When a compare match occurs on channel A
	@remark Applicable only if using a fixed frequency */
	COMPARE_MATCH_A = 1,
/** When a compare match occurs on channel B */
	COMPARE_MATCH_B = 2,
/** When an external event occurs
	@remark Applicable only in input capture mode. */
	EXTERNAL = 3
}timer_interruptType_enum_t;

/** Specifies the channel used for compare match interrupts.
*/
typedef enum timer_channel_enum_t
{
/** Channel A */
	CHANNEL_A,
/** Channel B */
	CHANNEL_B
}timer_channel_enum_t;

/** Timer configuration structure
*/
typedef struct timer_struct_t
{
/** Which timer peripheral is configured */
	timer_peripheral_enum_t peripheral;
/** The frequency at which the timer overflows. Count frequency is dependent on this. Use @link timer_prescaler_enum_t @endlink if count frequency is more of interest than overflow one.
	@warning The actual frequency set may be different or not achievable because of division rounding down or 8bit resolution of timer. */
	u32 frequency;
}timer_struct_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Initializes timer peripheral.
	@remark		Must be called before any other function for the required timer.
	@remark		Actual timer frequency may differ from the actual one due to division rounding.
	@param[in]	s_timer: timer parameters to initialize
*/
void timer_init(timer_struct_t s_timer);

/** Prepares all timers to be started at the same time.
	@pre		Must be called after all timers were initialized (with @link timer_init @endlink).
	@remark		Calling this freezes all timers so be careful when using it if one timer is already running.
	@remark		Calling @link timer_start @endlink after this has no immediate effect but is required before a synchronised start.
*/
void timer_prepareSynchronisedStart();

/** Starts all configured timers simultaneously
	@pre		Must be called after the timers were started individually (with @link timer_start @endlink).
*/
void timer_synchronisedStart();

/** Starts timer peripheral.
	@pre		Must be called after the timer was initialized (with @link timer_init @endlink).
	@param[in]	s_timer: timer peripheral to use
*/
void timer_start(timer_struct_t s_timer);

/** Stops timer peripheral.
	@pre		Must be called after the timer was initialized (with @link timer_init @endlink).
	@param[in]	s_timer: timer peripheral to use
*/
void timer_stop(timer_struct_t s_timer);

/** Sets compare match as percentage of overflow period
	@param[in]	s_timer: timer peripheral to use
	@param[in]	e_channel: channel to set
	@param[in]	u8_percent: percentage to set
*/
void timer_setCompareMatch(timer_struct_t s_timer, timer_channel_enum_t e_channel, u8 u8_percent);

/** Enables a timer interrupt.
	@pre		Must be called after an interrupt was attached (with @link timer_attachInterrupt @endlink).
	@param[in]	s_timer: timer peripheral to use
	@param[in]	e_interruptType: interrupt type to enable
*/
void timer_enableInterrupt(timer_struct_t s_timer, timer_interruptType_enum_t e_interruptType);

/** Disables a timer interrupt.
	@param[in]	s_timer: timer peripheral to use
	@param[in]	e_interruptType: interrupt type to disable
*/
void timer_disableInterrupt(timer_struct_t s_timer, timer_interruptType_enum_t e_interruptType);

/** Sets a callback function to be called on timer interrupt. Can replace previous callback (no need to call @link timer_detachInterrupt @endlink first).
	@param[in]	s_timer: timer peripheral to use
	@param[in]	e_interruptType: interrupt type to attach
	@param[in]	p_function: pointer to function to call on interrupt
*/
void timer_attachInterrupt(timer_struct_t s_timer, timer_interruptType_enum_t e_interruptType, void (*p_function)(void));

/** Clears callback function to be called on timer interrupt in case the callback will no longer be used.
	@pre		Must be called while interrupt is disabled (with @link timer_disableInterrupt @endlink).
	@param[in]	s_timer: timer peripheral to use
	@param[in]	e_interruptType: interrupt type to detach
*/
void timer_detachInterrupt(timer_struct_t s_timer, timer_interruptType_enum_t e_interruptType);

/** Resets the timer counter.
	@param[in]	s_timer: timer peripheral to use
*/
void timer_resetValue(timer_struct_t s_timer);

/** Sets the timer counter to a certain value
	@param[in]	s_timer: timer peripheral to use
	@param[in]	u16_value: value to set
*/
void timer_setValue(timer_struct_t s_timer, u16 u16_value);

/** Returns the current timer counter value.
	@param[in]	s_timer: timer peripheral to use
	@return		timer counter value
*/
u16 timer_getValue(timer_struct_t s_timer);

/**	Function internal to the timer libraries.
	@param[in]	s_timer: timer peripheral to check
	@return		whether ICR or OCRA register is used as max value for timer
*/
bool usingCompareRegisterAsMax(timer_struct_t s_timer);

/**	Function internal to the timer libraries.
	@param[in]	s_timer: timer peripheral to use
	@param[in]	u32_frequency: target frequency
	@param[in]	pu32_prescaler: pointer to prescaler value to set in register after function call
	@return		value to set in timer top value register
*/
u16 calculateTopRegister(timer_struct_t s_timer, u32 u32_frequency, u32 *pu32_prescaler);

/**	Wait for a number of milliseconds
	@param[in]	milliseconds to wait
*/
void timer_delayMs(u32 milliseconds);

/**	Wait for a number of microseconds
	@param[in]	microseconds to wait
*/
void timer_delayUs(u16 microseconds);

#endif /* TIM_H_ */
