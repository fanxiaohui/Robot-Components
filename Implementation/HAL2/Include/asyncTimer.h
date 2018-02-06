/**	@file		asyncTimer.h
	@brief		Asynchronous Timer features
	@details	Uses @link TIMER2 @endlink in asynchronous mode
				Basic flow:
				1. Call @link asyncTimer_init @endlink.
				2. Call @link asyncTimer_start @endlink.
				3. Call @link asyncTimer_getValue @endlink or @link asyncTimer_resetValue @endlink whenever needed.
				- Attach a function to the timer interrupt handler with @link asyncTimer_attachInterrupt @endlink prior to calling @link asyncTimer_enableInterrupt @endlink.
				- To stop the timer call @link asyncTimer_stop @endlink.
*/

#ifndef ASYNCTIMER_H_
#define ASYNCTIMER_H_

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "timer.h"
#include "types.h"

/** @example asyncTimer_config.h
	This is an example of how to configure the Asychronous Timer driver
*/
#include "asyncTimer_config.h"

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/** Specifies the clock source
*/
typedef enum asyncTimer_source_enum_t
{
/** Use digital clock signal connected to TOSC1 pin */
	CLOCK_SIGNAL,
/**	Use 32kHz oscillator connected to TOSC pins*/
	OSCILLATOR
}asyncTimer_source_enum_t;

/** Asynchronous timer configuration structure
*/
typedef struct asyncTimer_struct_t
{
/** What kind of clock source to use */
	asyncTimer_source_enum_t source;
/** What value should the input clock be divided by */
	timer_prescaler_enum_t prescaler;
}asyncTimer_struct_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Initializes timer peripheral.
	@remark		Must be called before any other function for the required timer.
*/
void asyncTimer_init(asyncTimer_struct_t s_asyncTimer);

/** Starts timer peripheral.
	@pre		Must be called after the timer was initialized (with @link asyncTimer_init @endlink).
*/
void asyncTimer_start();

/** Stops timer peripheral.
	@pre		Must be called after the timer was initialized (with @link asyncTimer_init @endlink).
*/
void asyncTimer_stop();

/** Sets compare match after a number of events
	@param[in]	e_channel: channel to set
	@param[in]	u16_numberOfEvents: number of events for a compare match
*/
void asyncTimer_setCompareMatch(timer_channel_enum_t e_channel, u16 u16_numberOfEvents);

/** Enables a timer interrupt.
	@pre		Must be called after an interrupt was attached (with @link asyncTimer_attachInterrupt @endlink).
	@param[in]	e_interruptType: interrupt type to enable
*/
void asyncTimer_enableInterrupt(timer_interruptType_enum_t e_interruptType);

/** Disables a timer interrupt.
	@param[in]	e_interruptType: interrupt type to disable
*/
void asyncTimer_disableInterrupt(timer_interruptType_enum_t e_interruptType);

/** Sets a callback function to be called on timer interrupt. Can replace previous callback (no need to call @link asyncTimer_detachInterrupt @endlink first).
	@param[in]	e_interruptType: interrupt type to attach
	@param[in]	p_function: pointer to function to call on interrupt
*/
void asyncTimer_attachInterrupt(timer_interruptType_enum_t e_interruptType, void (*p_function)(void));

/** Clears callback function to be called on timer interrupt in case the callback will no longer be used.
	@pre		Must be called while interrupt is disabled (with @link asyncTimer_disableInterrupt @endlink).
	@param[in]	e_interruptType: interrupt type to detach
*/
void asyncTimer_detachInterrupt(timer_interruptType_enum_t e_interruptType);

/** Resets the timer counter.
*/
void asyncTimer_resetValue();

/** Sets the timer counter to a certain value
	@param[in]	u16_value: value to set
*/
void asyncTimer_setValue(u16 u16_value);

/** Returns the current timer counter value.
	@return		timer counter value
*/
u16 asyncTimer_getValue();

#endif /* ASYNCTIMER_H_ */
