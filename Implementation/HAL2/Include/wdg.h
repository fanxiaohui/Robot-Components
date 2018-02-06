/**	@file		wdg.h
	@brief		Watchdog features
	@details	Supports the watchdog in the following modes:
				- Interrupt, where a watchdog overflow just generates an interrupt. The interrupt is disabled afterwards in hardware and needs to be reenabled if desired.
				- Reset, where a watchdog overflow resets the system, setting a flag.
				- Both, where a watchdog overflow first calls an interrupt handler and after it resets the system.
				Basic flow:
				0. Check value of @link wdg_resetOccured @endlink at the begining of the application code and act on it in case microcontroller was reset by watchdog.
				1. Set watchdog timeout with @link wdg_setTimeout @endlink.
				2. Set watchdog mode with @link wdg_enableReset @endlink, @link wdg_enableInterrupt @endlink or both. Note that both these functions start the watchdog counter.
				3. Remember to reset the watchdog counter with @link wdg_feed @endlink to prevent it from overflowing.
				- Attach a function to the watchdog interrupt handler with @link wdg_attachInterrupt @endlink prior to calling @link wdg_enableInterrupt @endlink.
				- The watchdog interrupt is disabled by hardware once it is called. Remember to call @link wdg_enableInterrupt @endlink at the end of the function attached to the interrupt handler to enable the interrupt again.
				- To stop the watchdog (for power saving for instance or during blocking function calls of unknown duration) call @link wdg_disableReset @endlink  or @link wdg_disableInterrupt @endlink depending on which mode is used.
*/

#ifndef WDG_H_
#define WDG_H_

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "math.h"
#include "types.h"

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/** Specifies the watchdog overflow period
*/
typedef enum wdg_timeout_enum_t
{
/**	Watchdog will overflow in 16ms after it is reset. */
	_16MS = 0,
/**	Watchdog will overflow in 32ms after it is reset. */
	_32MS = 1,
/**	Watchdog will overflow in 64ms after it is reset. */
	_64MS = 2,
/**	Watchdog will overflow in 125ms after it is reset. */
	_125MS = 3,
/**	Watchdog will overflow in 250ms after it is reset. */
	_250MS = 4,
/**	Watchdog will overflow in 500ms after it is reset. */
	_500MS = 5,
/**	Watchdog will overflow in 1s after it is reset. */
	_1S = 6,
/**	Watchdog will overflow in 2s after it is reset. */
	_2S = 7,
/**	Watchdog will overflow in 4s after it is reset. */
	_4S = 0x20,
/**	Watchdog will overflow in 8s after it is reset. */
	_8S = 0x21
}wdg_timeout_enum_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Sets the watchdog overflow period
	@param[in]	e_wdg_timeout: overflow period
*/
void wdg_setTimeout(wdg_timeout_enum_t e_wdg_timeout);

/** Enables the watchdog counter and sets it to reset the microcontroller if it overflows
	@remark		@link wdg_enableInterrupt @endlink can be called while Reset mode is enabled to trigger an interrupt prior to the actual reset. Note that doing so does not prevent the actual reset once the watchdog overflows.
	@warning	Remember to call @link wdg_feed @endlink to prevent a microcontroller reset.
*/
void wdg_enableReset();

/** Disables the watchdog system reset functionality
*/
void wdg_disableReset();

/** Resets the watchdog counter
*/
void wdg_feed();

/** Checks if a reset due to watchdog occurred.
	@return		TRUE if a watchdog reset occurred, FALSE otherwise
*/
bool wdg_resetOccured();

/** Enables the watchdog counter and sets it to generate an interrupt if it overflows
	@remark		@link wdg_enableReset @endlink can be called while Interrupt mode is enabled to also trigger a reset after the interrupt handler returns.
*/
void wdg_enableInterrupt();

/** Disables the watchdog interrupt functionality
*/
void wdg_disableInterrupt();

/** Sets a callback function to be called on watchdog interrupt. Can replace previous callback (no need to call @link wdg_detachInterrupt @endlink first).
	@param[in]	p_function: pointer to function to use
*/
void wdg_attachInterrupt(void (*p_function)(void));

/** Clears callback function to be called on watchdog interrupt in case the callback will no longer be used.
	@pre		Must be called while interrupt is disabled (with @link wdg_disableInterrupt @endlink).
*/
void wdg_detachInterrupt();

#endif /* WDG_H_ */
