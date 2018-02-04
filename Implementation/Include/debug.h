/**	@file		debug.h
	@brief		Debug logging functions
	@details
*/

#ifndef DEBUG_H_
#define DEBUG_H_

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "types.h"

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Initializes the debug interface.
	@remark Must be called before any other debug function is used.
*/
void debug_init();

/** Starts the debug interface.
	@pre Must be called after the debug is initialized (with @link debug_init @endlink).
*/
void debug_start();

/** Stops the debug interface.
	@pre Must be called after the debug is initialized (with @link debug_init @endlink).
*/
void debug_stop();

/**	Writes a character on the debug interface.
	@pre Must be called after the debug is initialized (with @link debug_init @endlink).
	@param[in]	u8_char: character to write
*/
void debug_writeChar(u8 u8_char);

/**	Writes a string on the debug interface.
	@pre Must be called after the debug is initialized (with @link debug_init @endlink).
	@param[in]	pc8_string: string to write
*/
void debug_writeString(char* pc8_string);

/**	Writes a number on the debug interface.
	@pre Must be called after the debug is initialized (with @link debug_init @endlink).
	@param[in]	u16_data: number to write
*/
void debug_writeDecimal(u16 u16_data);

/**	Writes a byte in hex format on the debug interface.
	@pre Must be called after the debug is initialized (with @link debug_init @endlink).
	@param[in]	u8_data: byte to write
*/
void debug_writeHex(u8 u8_data);

/**	Writes a 2 byte word in hex format on the debug interface.
	@pre Must be called after the debug is initialized (with @link debug_init @endlink).
	@param[in]	u16_data: word to write
*/
void debug_writeHexWord(u16 u16_data);

/**	Writes a 4 byte double word in hex format on the debug interface.
	@pre Must be called after the debug is initialized (with @link debug_init @endlink).
	@param[in]	u32_data: word to write
*/
void debug_writeHexDWord(u32 u32_data);

#endif /* DEBUG_H_ */