/**	@file		counter.h
	@brief		Counter features
	@details	Supports timers 0, 1, 3 as external event counters, which can be used to count number of external events
				Basic flow:
				1. Initialize a @link counter_struct_t @endlink with either a fixed (set base frequency to a value of type @link timer_prescaler_enum_t @endlink) or a custom frequency (set to any unsigned value).
				2. Pass it to @link counter_init @endlink.
				3. Call @link counter_start @endlink.
				4. Use @link counter_getValue @endlink or @link counter_resetValue @endlink whenever needed.
				- Call @link counter_setEdge @endlink to change the edge of the external event to use as counter increment.
				- Attach a function for the timer overflow interrupt handler with @link counter_attachInterrupt @endlink prior to calling @link counter_enableInterrupt @endlink.
				- To stop the counter call @link counter_stop @endlink.
*/

#ifndef COUNTER_H_
#define COUNTER_H_

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "timer.h"
#include "types.h"

/** @example counter_config.h
	This is an example of how to configure the Counter driver
*/
#include "counter_config.h"

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/** Specifies the edge of external signal used to clock the counter.
*/
typedef enum counter_edge_enum_t
{
/** Falling edge */
	FALLING = 0b110,
/** Rising edge */
	RISING = 0b111
}counter_edge_enum_t;

/** Counter configuration structure
*/
typedef struct counter_struct_t
{
/** Base timer configuration structure */
	timer_struct_t base;
/** Which edge should trigger an increment of the counter */
	counter_edge_enum_t edge;
}counter_struct_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Initializes timer peripheral.
	@remark		Must be called before any other function for the required timer.
	@remark		Actual timer frequency may differ from the actual one due to division rounding.
	@param[in]	s_counter: counter parameters to initialize
*/
void counter_init(counter_struct_t s_counter);

/** Sets the edge to be used to trigger an increment of the counter.
	@param[in]	s_counter: timer peripheral to use
	@param[in]	e_edge: edge type to detect
*/
void counter_setEdge(counter_struct_t s_counter, counter_edge_enum_t e_edge);

/** Starts timer peripheral.
	@pre		Must be called after the timer was initialized (with @link counter_init @endlink).
	@param[in]	s_counter: timer peripheral to use
*/
void counter_start(counter_struct_t s_counter);

/** Stops timer peripheral.
	@pre		Must be called after the timer was initialized (with @link counter_init @endlink).
	@param[in]	s_counter: timer peripheral to use
*/
void counter_stop(counter_struct_t s_counter);

/** Sets compare match after a number of events
	@param[in]	s_counter: timer peripheral to use
	@param[in]	e_channel: channel to set
	@param[in]	u16_numberOfEvents: number of events for a compare match
*/
void counter_setCompareMatch(counter_struct_t s_counter, timer_channel_enum_t e_channel, u16 u16_numberOfEvents);

/** Enables a timer overflow interrupt.
	@pre		Must be called after a timer overflow interrupt was attached (with @link counter_attachInterrupt @endlink).
	@param[in]	s_counter: timer peripheral to use
	@param[in]	e_interruptType: interrupt type to enable
*/
void counter_enableInterrupt(counter_struct_t s_counter, timer_interruptType_enum_t e_interruptType);

/** Disables a timer interrupt.
	@param[in]	s_counter: timer peripheral to use
	@param[in]	e_interruptType: interrupt type to disable
*/
void counter_disableInterrupt(counter_struct_t s_counter, timer_interruptType_enum_t e_interruptType);

/** Sets a callback function to be called on timer overflow interrupt. Can replace previous callback (no need to call @link counter_detachInterrupt @endlink first).
	@param[in]	s_counter: timer peripheral to use
	@param[in]	e_interruptType: interrupt type to attach
	@param[in]	p_function: pointer to function to call on interrupt
*/
void counter_attachInterrupt(counter_struct_t s_counter, timer_interruptType_enum_t e_interruptType, void (*p_function)(void));

/** Clears callback function to be called on timer overflow interrupt in case the callback will no longer be used.
	@pre		Must be called while interrupt is disabled (with @link counter_disableInterrupt @endlink).
	@param[in]	s_counter: timer peripheral to use
	@param[in]	e_interruptType: interrupt type to detach
*/
void counter_detachInterrupt(counter_struct_t s_counter, timer_interruptType_enum_t e_interruptType);

/** Resets the timer counter.
	@param[in]	s_counter: timer peripheral to use
*/
void counter_resetValue(counter_struct_t s_counter);

/** Sets the timer counter to a certain value
	@param[in]	s_counter: timer peripheral to use
	@param[in]	u16_value: value to set
*/
void counter_setValue(counter_struct_t s_counter, u16 u16_value);

/** Returns the current timer counter value.
	@param[in]	s_counter: timer peripheral to use
	@return		timer counter value
*/
u16 counter_getValue(counter_struct_t s_counter);

#endif /* COUNTER_H_ */
