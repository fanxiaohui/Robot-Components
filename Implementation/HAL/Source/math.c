/**	@file		math.c
	@brief		Basic math functions
	@details	See @link math.h @endlink for details.
*/

/************************************************************************/
/* AVR includes                                                         */
/************************************************************************/

#include <stdlib.h>

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "math.h"

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void updateBit(volatile u8 *u8_register, u8 u8_bitPosition, u8 u8_bitValue)
{
	u8 u8_newValue = u8_bitValue << u8_bitPosition;
	u8_newValue |= (*u8_register & ~(1 << u8_bitPosition));
	*u8_register = u8_newValue;
}

void setBit(volatile u8 *u8_register, u8 u8_bitPosition)
{
	*u8_register |= (1 << u8_bitPosition);
}

void clearBit(volatile u8 *u8_register, u8 u8_bitPosition)
{
	*u8_register &= ~(1 << u8_bitPosition);
}

void toggleBit(volatile u8 *u8_register, u8 u8_bitPosition)
{
	*u8_register ^= (1 << u8_bitPosition);
}

bitValue checkBit(u8 u8_register, u8 u8_bitPosition)
{
	return (u8_register >> u8_bitPosition) & 1;
}

u16 calculateError(u32 u32_actualValue, u32 u32_expectedValue)
{
	s32 s32_error;
	s32_error = (u32_actualValue - u32_expectedValue) * 1000 / u32_expectedValue;

	return abs(s32_error);
}

u16 ruleOfThree(u16 u16_extreme, u16 u16_mean1, u16 u16_mean2)
{
	u32 u32_result;
	u32_result = (u32) u16_mean1 * u16_mean2 / u16_extreme;
	return (u16) u32_result;
}
