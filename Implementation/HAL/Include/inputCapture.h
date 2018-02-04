/**	@file		inputCapture.h
	@brief		Input Capture features
	@details	Supports timers 1 and 3 as input capture, which can be used to count duration of external events
				Basic flow:
				1. Initialize a @link inputCapture_struct_t @endlink with either a fixed (set base frequency to a value of type @link timer_prescaler_enum_t @endlink) or a custom frequency (set to any unsigned value).
				2. Pass it to @link inputCapture_init @endlink.
				3. Call @link inputCapture_start @endlink.
				4. Use @link inputCapture_getValue @endlink or @link inputCapture_resetValue @endlink whenever needed.
				- Call @link inputCapture_setEdge @endlink to change the edge of the external event to use as end of measurement.
				- Attach a function for the input capture interrupt handlers with @link inputCapture_attachInterrupt @endlink prior to calling @link inputCapture_enableInterrupt @endlink.
				- To stop the input capture call @link inputCapture_stop @endlink.
				- Useful to attach an interrupt to @link OVERFLOW @endlink if the duration until the external event happens (eg. signal period) is higher than the timer overflow period to increase the input capture clock division factor. Useful also to reset the input capture counter (with @link inputCapture_resetValue @endlink) if measuring periodic signals.
				- Useful to attach an interrupt to @link EXTERNAL @endlink to change the edge of the external event (with @link inputCapture_setEdge @endlink) if measuring both edges of a signal or a non-periodic signal.
*/

#ifndef INPUTCAPTURE_H_
#define INPUTCAPTURE_H_

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "timer.h"
#include "types.h"

/** @example inputCapture_config.h
	This is an example of how to configure the Input Capture driver
*/
#include "inputCapture_config.h"

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/** Specifies the edge of external signal used to save counter value.
*/
typedef enum inputCapture_edge_enum_t
{
	/** Falling edge */
	FALLING = 0,
	/** Rising edge */
	RISING = 1
}inputCapture_edge_enum_t;

/** Specifies the source of the input capture signal
*/
typedef enum inputCapture_source_enum_t
{
	/** Input capture pin */
	ICP = 0,
	/** Analog comparator */
	AC = 1
}inputCapture_source_enum_t;

/** Input capture configuration structure
*/
typedef struct inputCapture_struct_t
{
/** Base timer configuration structure */
	timer_struct_t base;
/** Which edge should trigger a save of the counter value */
	inputCapture_edge_enum_t edge;
/** Which source should be used */
	inputCapture_source_enum_t source;
/**	Whether or not noise canceling is used
	@remark This introduces a delay of 4 I/O clock cycles (independent of timer prescaler). */
	bool useNoiceCanceling;
}inputCapture_struct_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Initializes timer peripheral.
	@remark		Must be called before any other function for the required timer.
	@remark		Actual timer frequency may differ from the actual one due to division rounding.
	@param[in]	s_inputCapture: input capture parameters to initialize
*/
void inputCapture_init(inputCapture_struct_t s_inputCapture);

/** Sets the edge to be used to trigger a save of the counter value.
	@param[in]	s_inputCapture: timer peripheral to use
	@param[in]	e_edge: edge type to detect
*/
void inputCapture_setEdge(inputCapture_struct_t s_inputCapture, inputCapture_edge_enum_t e_edge);

/** Starts timer peripheral.
	@pre		Must be called after the timer was initialized (with @link inputCapture_init @endlink).
	@param[in]	s_inputCapture: timer peripheral to use
*/
void inputCapture_start(inputCapture_struct_t s_inputCapture);

/** Stops timer peripheral.
	@pre		Must be called after the timer was initialized (with @link inputCapture_init @endlink).
	@param[in]	s_inputCapture: timer peripheral to use
*/
void inputCapture_stop(inputCapture_struct_t s_inputCapture);

/** Enables a timer overflow interrupt.
	@pre		Must be called after a timer interrupt was attached (with @link inputCapture_attachInterrupt @endlink).
	@param[in]	s_inputCapture: timer peripheral to use
	@param[in]	e_interruptType: interrupt type to enable
*/
void inputCapture_enableInterrupt(inputCapture_struct_t s_inputCapture, timer_interruptType_enum_t e_interruptType);

/** Disables a timer interrupt.
	@param[in]	s_inputCapture: timer peripheral to use
	@param[in]	e_interruptType: interrupt type to disable
*/
void inputCapture_disableInterrupt(inputCapture_struct_t s_inputCapture, timer_interruptType_enum_t e_interruptType);

/** Sets a callback function to be called on timer overflow interrupt. Can replace previous callback (no need to call @link inputCapture_detachInterrupt @endlink first).
	@param[in]	s_inputCapture: timer peripheral to use
	@param[in]	e_interruptType: interrupt type to attach
	@param[in]	p_function: pointer to function to call on interrupt
*/
void inputCapture_attachInterrupt(inputCapture_struct_t s_inputCapture, timer_interruptType_enum_t e_interruptType, void (*p_function)(void));

/** Clears callback function to be called on timer overflow interrupt in case the callback will no longer be used.
	@pre		Must be called while interrupt is disabled (with @link inputCapture_disableInterrupt @endlink).
	@param[in]	s_inputCapture: timer peripheral to use
	@param[in]	e_interruptType: interrupt type to detach
*/
void inputCapture_detachInterrupt(inputCapture_struct_t s_inputCapture, timer_interruptType_enum_t e_interruptType);

/** Resets the timer counter.
	@param[in]	s_inputCapture: timer peripheral to use
*/
void inputCapture_resetValue(inputCapture_struct_t s_inputCapture);

/** Sets the timer counter to a certain value
	@param[in]	s_inputCapture: timer peripheral to use
	@param[in]	u16_value: value to set
*/
void inputCapture_setValue(inputCapture_struct_t s_inputCapture, u16 u16_value);

/** Returns the current timer counter value.
	@param[in]	s_inputCapture: timer peripheral to use
	@return		timer counter value
*/
u16 inputCapture_getValue(inputCapture_struct_t s_inputCapture);

#endif /* INPUTCAPTURE_H_ */
