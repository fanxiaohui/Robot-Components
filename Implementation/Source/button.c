/**	@file		button.c
	@brief		Button features
	@example	button_example.c
	@details	See button.h for details
*/

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "button.h"
#include "types.h"

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void button_init(button_struct_t s_button)
{
	s_button.pin.direction = INPUT;

	gpio_init(s_button.pin);
	gpio_attachInterrupt(s_button.pin, INTERRUPT_TOGGLE, s_button.pressedFunction);
}

void button_start(button_struct_t s_button)
{
	gpio_enableInterrupt(s_button.pin, INTERRUPT_TOGGLE);
}

void button_stop(button_struct_t s_button)
{
	gpio_disableInterrupt(s_button.pin, INTERRUPT_TOGGLE);
}