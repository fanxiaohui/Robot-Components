/**	@file		wdg.c
	@brief		Watchdog features
	@details	See @link wdg.h @endlink for details.
*/

/************************************************************************/
/* AVR includes                                                         */
/************************************************************************/

#include <avr/wdt.h>
#include <avr/interrupt.h>

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "wdg.h"

/************************************************************************/
/* Internal variables                                                   */
/************************************************************************/

void (*p_wdgCallback)(void);

/************************************************************************/
/* Interrupt handlers                                                   */
/************************************************************************/

ISR(WDT_vect)
{
	p_wdgCallback();
}

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void wdg_setTimeout(wdg_timeout_enum_t e_wdg_timeout)
{
	/* Save current watchdog setup */
	u8 u8_temp = (WDTCSR & 0x48);
	u8_temp |= e_wdg_timeout;

	/* Special sequence must be used to configure watchdog */
	WDTCSR = (1 << WDCE) | ( 1 << WDE);
	/* Set the new register value */
	WDTCSR = u8_temp;
}

void wdg_enableReset()
{
	/* Save current watchdog setup */
	u8 u8_temp = (WDTCSR & 0x67);
	setBit(&u8_temp, WDE);

	/* Special sequence must be used to configure watchdog */
	WDTCSR = (1 << WDCE) | ( 1 << WDE);
	/* Set the new register value */
	WDTCSR = u8_temp;
}

void wdg_disableReset()
{
	/* Save current watchdog setup */
	u8 u8_temp = (WDTCSR & 0x67);
	clearBit(&u8_temp, WDE);

	/* Special sequence must be used to configure watchdog */
	WDTCSR = (1 << WDCE) | ( 1 << WDE);
	/* Set the new register value */
	WDTCSR = u8_temp;
}

void wdg_feed()
{
	wdt_reset();
}

bool wdg_resetOccured()
{
	bool b_returnValue = checkBit(MCUSR, WDRF);
	if (b_returnValue)
	clearBit(&MCUSR, WDRF);
	return b_returnValue;
}

void wdg_enableInterrupt()
{
	setBit(&WDTCSR, WDIE);
}

void wdg_disableInterrupt()
{
	clearBit(&WDTCSR, WDIE);
}

void wdg_attachInterrupt(void (*p_function)(void))
{
	p_wdgCallback = p_function;
}

void wdg_detachInterrupt()
{
	p_wdgCallback = 0;
}
