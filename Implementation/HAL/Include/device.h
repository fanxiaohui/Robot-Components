/**	@file		device.h
	@brief		Basic device features
	@author		Florin Popescu
	@version	1.1
	@date		11.09.2017
	@details	Includes features that are required for the microcontroller to function properly or microcontroller specific.
				Basic Flow:
				- Call @link device_disableJTAG @endlink to be able to use the pins normally defined for JTAG functionality for something else.
				- Call @link device_setClockPrescaler @endlink to change the division factor between the input clock and the actual system clock. This would typically be used when not much computation power is required but neither is a sleep mode.
				- Call @link device_calibrateInternalOscillator @endlink to calibrate the internal oscillator when precise timing is desired. It is a good idea to save the calibration value to a non-volatile memory.
				- Call @link device_getResetSource @endlink as early as possible after a reset to save current reset source and enable further checking. Since all reset flags are only cleared by HW on a @link POWER_ON_RESET @endlink after they are set once, having multiple flags set at once is possible if not cleared after every reset.
*/

#ifndef DEVICE_H_
#define DEVICE_H_

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "types.h"

/** @example device_config.h
	This is an example of how to configure the Device
*/
#include "device_config.h"

/**	Specifies system clock division factors
*/
typedef enum device_clockPrescaler_enum_t
{
/** 1:1 division factor from base clock */
	DEVICE_CLK_DIV_1 = 0,
/** 1:2 division factor from base clock */
	DEVICE_CLK_DIV_2 = 1,
/** 1:4 division factor from base clock */
	DEVICE_CLK_DIV_4 = 2,
/** 1:8 division factor from base clock */
	DEVICE_CLK_DIV_8 = 3,
/** 1:16 division factor from base clock */
	DEVICE_CLK_DIV_16 = 4,
/** 1:32 division factor from base clock */
	DEVICE_CLK_DIV_32 = 5,
/** 1:64 division factor from base clock */
	DEVICE_CLK_DIV_64 = 6,
/** 1:128 division factor from base clock */
	DEVICE_CLK_DIV_128 = 7,
/** 1:256 division factor from base clock */
	DEVICE_CLK_DIV_256 = 8
}device_clockPrescaler_enum_t;

/**	Specifies reset sources
*/
typedef enum device_resetSource_enum_t
{
	JTAG_RESET = 0x10,
	WATCHDOG_RESET = 0x08,
	BROWN_OUT_RESET = 0x04,
	EXTERNAL_RESET = 0x02,
	POWER_ON_RESET = 0x01
}device_resetSource_enum_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Disables JTAG functionality
	@remark		Without disabling JTAG, its corresponding pins will not be available for anything else.
*/
void device_disableJTAG();

/**	Sets system clock prescaler.
	@param[in]	e_prescaler: the division factor from the base clock to use as system clock
*/
void device_setClockPrescaler(device_clockPrescaler_enum_t e_prescaler);

/**	Changes the internal RC oscillator's frequency to a more precise value.
	@param[in]	b_highFrequencyRange: whether the high frequency range is used for calibration. Refer to OSCCAL register in the datasheet for details.
	@param[in]	u8_calibrationValue: actual calibration value
	@remark		Set system clock output on microcontroller pin from fuse bits and call this function to calibrate the clock frequency according to that.
*/
void device_calibrateInternalOscillator(bool b_highFrequencyRange, u8 u8_calibrationValue);

/**	Checks current reset reason.
	@return		reset reason
	@remark		This function also clears the corresponding reset reason flags, as all of them are only cleared by HW on a @link POWER_ON_RESET @endlink after they are set once.
*/
device_resetSource_enum_t device_getResetSource();

#endif /* DEVICE_H_ */
