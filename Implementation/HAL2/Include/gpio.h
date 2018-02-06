/**	@file		gpio.h
	@brief		GPIO features
	@details	Supports basic I/O functionality for all microcontroller pins.
				Basic flow:
				1. Initialize a @link gpio_struct_t @endlink
				2. Pass it to @link gpio_init @endlink
				3. Use the gpio_out and gpio_in functions
				- Pin direction and pull resistor can be changed with the specific functions without having to call @link gpio_init @endlink again.
				- Attach a function to the GPIO interrupt handler with @link gpio_attachInterrupt @endlink prior to calling @link gpio_enableInterrupt @endlink.
*/

#ifndef GPIO_H_
#define GPIO_H_

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "types.h"

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/** Specifies pin direction.
*/
typedef enum gpio_direction_enum_t
{
/**	Input pin */
	INPUT = 0,
/**	Output pin */
	OUTPUT = 1
}gpio_direction_enum_t;

/* Specifies if internal pull resistor is used. */
typedef enum gpio_pull_enum_t
{
/**	No pull resistor */
	NO_PULL = 0,
/**	Use pull up resistor */
	USE_PULLUP = 1
}gpio_pull_enum_t;

/** Specifies the pin port. */
typedef enum gpio_port_enum_t
{
/**	PORT A */
	PA,
/**	PORT B */
	PB,
/**	PORT C */
	PC,
/**	PORT D */
	PD
}gpio_port_enum_t;

/** Specifies the interrupt type to use.
*/
typedef enum gpio_interruptType_enum_t
{
/** On pin toggle */
	INTERRUPT_TOGGLE = 1,
/** On pin rising edge
	@remark	Only works on pins that have INTx functionality. */
	INTERRUPT_RISING_EDGE = 3,
/** On pin falling edge
	@remark	Only works on pins that have INTx functionality. */
	INTERRUPT_FALLING_EDGE = 2,
/** On pin low level
	@remark	Only works on pins that have INTx functionality.
	@remark	Interrupt will keep triggering as long as low level is held. */
	INTERRUPT_LOW_LEVEL = 0
}gpio_interruptType_enum_t;

/** Pin configuration structure
*/
typedef struct gpio_struct_t
{
/**	Which port the pin is connected to */
	gpio_port_enum_t port;
/**	Which pin is configured */
	u8 number;
/**	Direction of the pin */
	gpio_direction_enum_t direction;
/**	Specifies if internal pull resistor is used */
	gpio_pull_enum_t pullUp;
}gpio_struct_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Initializes GPIO pin.
	@remark		Must be called before any other function for the required pin.
	@param[in]	s_gpio: pin parameters to initialize
*/
void gpio_init(gpio_struct_t s_gpio);

/** Sets GPIO pin direction as input. Has no effect if the direction was already input.
	@param[in]	s_gpio: pin to use
*/
void gpio_setDirectionInput(gpio_struct_t *s_gpio);

/** Sets GPIO pin direction as output. Has no effect if the direction was already output.
	@param[in]	s_gpio: pin to use
*/
void gpio_setDirectionOutput(gpio_struct_t *s_gpio);

/** Sets GPIO pin high
	@pre		Must be called only when the pin is configured as output (with @link gpio_init @endlink or @link gpio_setDirectionOutput @endlink).
	@param[in]	s_gpio: pin to use
*/
void gpio_out_set(gpio_struct_t s_gpio);

/** Sets GPIO pin low
	@pre		Must be called only when the pin is configured as output (with @link gpio_init @endlink or @link gpio_setDirectionOutput @endlink).
	@param[in]	s_gpio: pin to use
*/
void gpio_out_reset(gpio_struct_t s_gpio);

/** Toggles GPIO pin
	@pre		Must be called only when the pin is configured as output (with @link gpio_init @endlink or @link gpio_setDirectionOutput @endlink).
	@param[in]	s_gpio: pin to use
*/
void gpio_out_toggle(gpio_struct_t s_gpio);

/** Enables internal Pull-Up resistor of GPIO pin
	@pre		Must be called only when the pin is configured as input (with @link gpio_init @endlink or @link gpio_setDirectionInput @endlink).
	@param[in]	s_gpio: pin to use
*/
void gpio_in_enablePullup(gpio_struct_t *s_gpio);

/** Disables internal Pull-Up resistor of GPIO pin
	@pre		Must be called only when the pin is configured as input (with @link gpio_init @endlink or @link gpio_setDirectionInput @endlink).
	@param[in]	s_gpio: pin to use
*/
void gpio_in_disablePullup(gpio_struct_t *s_gpio);

/** Returns value of GPIO pin
	@pre		Must be called only when the pin is configured as input (with @link gpio_init @endlink or @link gpio_setDirectionInput @endlink).
	@param[in]	s_gpio: pin to use
	@return		logical value of pin
*/
u8 gpio_in_read(gpio_struct_t s_gpio);

/** Enables a pin change interrupt.
	@pre		Must be called after an interrupt was attached (with @link gpio_attachInterrupt @endlink).
	@param[in]	s_gpio: GPIO pin to use
	@param[in]	e_interruptType: interrupt type to enable
*/
void gpio_enableInterrupt(gpio_struct_t s_gpio, gpio_interruptType_enum_t e_interruptType);

/** Disables a pin change interrupt.
	@param[in]	s_gpio: GPIO pin to use
	@param[in]	e_interruptType: interrupt type to disable
*/
void gpio_disableInterrupt(gpio_struct_t s_gpio, gpio_interruptType_enum_t e_interruptType);

/** Sets a callback function to be called on pin change interrupt. Can replace previous callback (no need to call @link gpio_detachInterrupt @endlink first).
	@param[in]	s_gpio: GPIO pin to use
	@param[in]	e_interruptType: interrupt type to attach
	@param[in]	p_function: pointer to function to call on interrupt
*/
void gpio_attachInterrupt(gpio_struct_t s_gpio, gpio_interruptType_enum_t e_interruptType, void (*p_function)(void));

/** Clears callback function to be called on pin change interrupt in case the callback will no longer be used.
	@pre		Must be called while interrupt is disabled (with @link gpio_disableInterrupt @endlink).
	@param[in]	s_gpio: GPIO pin to use
	@param[in]	e_interruptType: interrupt type to detach
*/
void gpio_detachInterrupt(gpio_struct_t s_gpio, gpio_interruptType_enum_t e_interruptType);

#endif /* GPIO_H_ */
