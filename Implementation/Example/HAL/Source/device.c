/**	@file		device.c
	@brief		Basic device features
	@author		Florin Popescu
	@version	1.0
	@date		11.08.2017
	@details	See device.h for details.
*/

/************************************************************************/
/* AVR includes                                                         */
/************************************************************************/

#include <avr/io.h>

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "device.h"
#include "math.h"

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void device_disableJTAG()
{
#ifndef USING_JTAG_PROGRAMMING
	MCUCR = 0x80;
	MCUCR = 0x80;
#endif
}

void device_setClockPrescaler(device_clockPrescaler_enum_t e_prescaler)
{
	CLKPR = 0x80;
	CLKPR |= e_prescaler;
}

void device_calibrateInternalOscillator(bool b_highFrequencyRange, u8 u8_calibrationValue)
{
	updateBit(&OSCCAL, CAL7, b_highFrequencyRange);
	OSCCAL |= u8_calibrationValue & (0x7f);
}

device_resetSource_enum_t device_getResetSource()
{
	device_resetSource_enum_t returnValue = MCUSR;
	MCUSR = 0;
	return returnValue;
}