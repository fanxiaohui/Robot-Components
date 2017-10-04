/**	@file		math.h
	@brief		Basic mah functions
	@author		Florin Popescu
	@version	1.0
	@date		12.08.2017
	@details	Contains math functions to support easier bitwise operations or more advanced error calculations.
*/

#ifndef MATH_H_
#define MATH_H_

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "types.h"

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Update a bit according to a certain value
	@param[in]	u8_register: register to use
	@param[in]	u8_bitPosition: bit to update
	@param[in]	u8_bitValue: new bit value
*/
void updateBit(volatile u8 *u8_register, u8 u8_bitPosition, u8 u8_bitValue);

/** Set a bit in a register
	@param[in]	u8_register: register to use
	@param[in]	u8_bitPosition: bit to set
*/
void setBit(volatile u8 *u8_register, u8 u8_bitPosition);

/** Clear a bit in a register
	@param[in]	u8_register: register to use
	@param[in]	u8_bitPosition: bit to clear
*/
void clearBit(volatile u8 *u8_register, u8 u8_bitPosition);

/** Toggle a bit in a register
	@param[in]	u8_register: register to use
	@param[in]	u8_bitPosition: bit to toggle
*/
void toggleBit(volatile u8 *u8_register, u8 u8_bitPosition);

/** Check value of a bit in a register
	@param[in]	u8_register: register to use
	@param[in]	u8_bitPosition: bit to toggle
	@return		SET or CLEAR
*/
bitValue checkBit(u8 u8_register, u8 u8_bitPosition);

/** Calculates error between actual value of something and its expected value. Useful for division rounding errors.
	@param[in]	u32_actualValue: actual value calculated
	@param[in]	u32_expectedValue: value to be expected
	@return		error as 1/1000 of expected value
*/
u16 calculateError(u32 u32_actualValue, u32 u32_expectedValue);

/**	Calculates a rule of three used in translating between an xbit value to an ybit one or from/to percentages
	@details	Consider an equation of the form a/b = c/x for understanding parameter positions
	@param[in]	u16_extreme: the extreme to the value required (a)
	@param[in]	u16_mean1: one of the means to the value required (b or c)
	@param[in]	u16_mean2: one of the means to the value required (b or c)
	@return		the value calculated (x)
*/
u16 ruleOfThree(u16 u16_extreme, u16 u16_mean1, u16 u16_mean2);

#endif /* MATH_H_ */
