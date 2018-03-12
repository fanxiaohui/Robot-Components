/**	@file		button.h
	@brief		Button features
	@example	button_example.c
	@details	Basic flow:
				0. <Describe the basic flow of data and function calls to use the module>
				1. <Usually initialize a @link <module>_struct_t @endlink with <@link structure members of interest @endlink>
				2. <Pass it to @link <module>_init @endlink.>
				3. <Call @link <module>_start @endlink.>
				4. <Do whatever else is needed.>
				- Attach a function to the <module> interrupt handler with @link <module>_attachInterrupt @endlink prior to calling @link <module>_enableInterrupt @endlink.
				- To stop the <module> call @link <module>_stop @endlink. (for power saving)
				- <Give whatever indications required>
*/

#ifndef BUTTON_H_
#define BUTTON_H_

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "device.h"
#include "types.h"
#include "gpio.h"


/** @example <module>_config.h
	This is an example of how to configure the <module>.
*/

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/** If button is high or low when pressed */
typedef enum button_active_enum_t
{
	BUTTON_ACTIVE_HIGH = 1,
	BUTTON_ACTIVE_LOW = 0
}button_active_enum_t;

/** button configuration structure
*/
typedef struct button_struct_t
{
/** Set the button's port and pin number */
	gpio_struct_t pin;
/** If button is high or low when pressed */
	button_active_enum_t active;
/** The button's debounce time in milliseconds */
	u8 debounceTime;
/** Function to call if button is pressed */
	void (*pressedFunction)(void);
}button_struct_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Initializes the button
	@param[in]	s_button: Button configuration structure
*/
void button_init(button_struct_t s_button);

/** Enables interrupts for the button
	@param[in]	s_button: Button configuration structure
*/
void button_start(button_struct_t s_button);

/** Disables interrupts for the button
	@param[in]	s_button: Button configuration structure
*/
void button_stop(button_struct_t s_button);

#endif /* BUTTON_H_ */
