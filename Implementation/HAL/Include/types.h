/**	@file		types.h
	@brief		Generic data types
	@author		Florin Popescu
	@version	1.0
	@date		08.08.2017
	@details	Contains the following definitions:
				- NO_ERROR returned by a function which may fail
				- bitValue set or clear
				- bool type
				- signed and unsigned 8 to 32 bit wide data types
*/

#ifndef TYPES_H_
#define TYPES_H_

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/**	Speficies that a function that may fail (usually initializations) succeeded with no errors.
*/
#define NO_ERROR	0

/**	Logical values of bits
*/
typedef enum bitValue
{
	SET = 1,
	CLEAR = 0
}bitValue;

/**	Boolean data type
*/
typedef enum bool
{
	TRUE = 1,
	FALSE = 0
}bool;

/**	Unsigned integer 8 bits wide
*/
typedef unsigned char	u8;

/**	Unsigned integer 16 bits wide
*/
typedef unsigned short	u16;

/**	Unsigned integer 32 bits wide
*/
typedef unsigned long	u32;

/**	signed integer 8 bits wide
*/
typedef signed char		s8;

/**	signed integer 16 bits wide
*/
typedef signed short	s16;

/**	signed integer 32 bits wide
*/
typedef signed long		s32;

/**	floating point value where the first 16 bits represent the integer part and the last 16 bits the decimal part
*/
typedef u32 f1616;

#endif /* TYPES_H_ */
