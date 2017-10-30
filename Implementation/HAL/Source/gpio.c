/**	@file		gpio.c
	@brief		GPIO features
	@author		Florin Popescu
	@version	1.1
	@date		11.09.2017
	@details	See gpio.h for details.
*/

/************************************************************************/
/* AVR includes                                                         */
/************************************************************************/

#include <avr/interrupt.h>
#include <avr/io.h>

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "gpio_config.h"
#include "gpio.h"
#include "math.h"
#include "uart.h"

/************************************************************************/
/* Internal variables                                                   */
/************************************************************************/

#ifdef USING_PCINT0
void (*p_pcInt0Callback)(void);
u8 u8_pcInt0OldState;
#endif
#ifdef USING_PCINT1
void (*p_pcInt1Callback)(void);
u8 u8_pcInt1OldState;
#endif
#ifdef USING_PCINT2
void (*p_pcInt2Callback)(void);
u8 u8_pcInt2OldState;
#endif
#ifdef USING_PCINT3
void (*p_pcInt3Callback)(void);
u8 u8_pcInt3OldState;
#endif
#ifdef USING_EXTINT
void (*p_extIntCallbacks[3])(void);
#endif

uart_struct_t s_debugUart;

/************************************************************************/
/* Internal functions                                                   */
/************************************************************************/

void gpio_changeDirection(gpio_struct_t s_gpio)
{
	switch (s_gpio.port)
	{
		case PA:
			updateBit(&DDRA, s_gpio.number, s_gpio.direction);
			break;
		case PB:
			updateBit(&DDRB, s_gpio.number, s_gpio.direction);
			break;
		case PC:
			updateBit(&DDRC, s_gpio.number, s_gpio.direction);
			break;
		case PD:
			updateBit(&DDRD, s_gpio.number, s_gpio.direction);
			break;
	}
}

/************************************************************************/
/* Interrupt handlers                                                   */
/************************************************************************/

ISR(PCINT0_vect)
{
#ifdef USING_PCINT0
	p_pcInt0Callback();
#endif
}

ISR(PCINT1_vect)
{
#ifdef USING_PCINT1
	p_pcInt1Callback();
#endif
}

ISR(PCINT2_vect)
{
#ifdef USING_PCINT2
	p_pcInt2Callback();
#endif
}

ISR(PCINT3_vect)
{
#ifdef USING_PCINT3
	p_pcInt3Callback();
#endif
}

ISR(INT0_vect)
{
#ifdef USING_EXTINT
	p_extIntCallbacks[0]();
#endif
}

ISR(INT1_vect)
{
#ifdef USING_EXTINT
	p_extIntCallbacks[1]();
#endif
}

ISR(INT2_vect)
{
#ifdef USING_EXTINT
	p_extIntCallbacks[2]();
#endif
}

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void gpio_init(gpio_struct_t s_gpio)
{
	/* Configure pin direction */
	gpio_changeDirection(s_gpio);

	/* Configure pin pull-up */
	if (s_gpio.direction == INPUT)
	{
		switch (s_gpio.port)
		{
			case PA:
				updateBit(&PORTA, s_gpio.number, s_gpio.pullUp);
				break;
			case PB:
				updateBit(&PORTB, s_gpio.number, s_gpio.pullUp);
				break;
			case PC:
				updateBit(&PORTC, s_gpio.number, s_gpio.pullUp);
				break;
			case PD:
				updateBit(&PORTD, s_gpio.number, s_gpio.pullUp);
				break;
		}
	}
}

void gpio_setDirectionInput(gpio_struct_t *s_gpio)
{
	s_gpio->direction = INPUT;
	gpio_changeDirection(*s_gpio);
}

void gpio_setDirectionOutput(gpio_struct_t *s_gpio)
{
	s_gpio->direction = OUTPUT;
	gpio_changeDirection(*s_gpio);
}

void gpio_out_set(gpio_struct_t s_gpio)
{
	switch (s_gpio.port)
	{
		case PA:
			setBit(&PORTA, s_gpio.number);
			break;
		case PB:
			setBit(&PORTB, s_gpio.number);
			break;
		case PC:
			setBit(&PORTC, s_gpio.number);
			break;
		case PD:
			setBit(&PORTD, s_gpio.number);
			break;
	}
}

void gpio_out_reset(gpio_struct_t s_gpio)
{
	switch (s_gpio.port)
	{
		case PA:
			clearBit(&PORTA, s_gpio.number);
			break;
		case PB:
			clearBit(&PORTB, s_gpio.number);
			break;
		case PC:
			clearBit(&PORTC, s_gpio.number);
			break;
		case PD:
			clearBit(&PORTD, s_gpio.number);
			break;
	}
}

void gpio_out_toggle(gpio_struct_t s_gpio)
{
	switch (s_gpio.port)
	{
		case PA:
			toggleBit(&PORTA, s_gpio.number);
			break;
		case PB:
			toggleBit(&PORTB, s_gpio.number);
			break;
		case PC:
			toggleBit(&PORTC, s_gpio.number);
			break;
		case PD:
			toggleBit(&PORTD, s_gpio.number);
			break;
	}
}

void gpio_in_enablePullup(gpio_struct_t *s_gpio)
{
	s_gpio->pullUp = USE_PULLUP;
	switch (s_gpio->port)
	{
		case PA:
			setBit(&PORTA, s_gpio->number);
			break;
		case PB:
			setBit(&PORTB, s_gpio->number);
			break;
		case PC:
			setBit(&PORTC, s_gpio->number);
			break;
		case PD:
			setBit(&PORTD, s_gpio->number);
			break;
	}
}

void gpio_in_disablePullup(gpio_struct_t *s_gpio)
{
	s_gpio->pullUp = NO_PULL;
	switch (s_gpio->port)
	{
		case PA:
			clearBit(&PORTA, s_gpio->number);
			break;
		case PB:
			clearBit(&PORTB, s_gpio->number);
			break;
		case PC:
			clearBit(&PORTC, s_gpio->number);
			break;
		case PD:
			clearBit(&PORTD, s_gpio->number);
			break;
	}
}

u8 gpio_in_read(gpio_struct_t s_gpio)
{
	switch (s_gpio.port)
	{
		case PA:
			return checkBit(PINA, s_gpio.number);
		case PB:
			return checkBit(PINB, s_gpio.number);
		case PC:
			return checkBit(PINC, s_gpio.number);
		case PD:
			return checkBit(PIND, s_gpio.number);
		default:
			return 0;
	}
}

void gpio_enableInterrupt(gpio_struct_t s_gpio, gpio_interruptType_enum_t e_interruptType)
{
	switch (s_gpio.port)
	{
		case PA:
		#ifdef USING_PCINT0
			setBit(&PCICR, PCIE0);
			setBit(&PCMSK0, s_gpio.number);
		#endif
			break;
		case PB:
		#ifdef USING_EXTINT
			if (s_gpio.number == 2)
			{
				updateBit(&EICRA, ISC21, checkBit(e_interruptType, 1));
				updateBit(&EICRA, ISC20, checkBit(e_interruptType, 0));
				setBit(&EIMSK, INT2);
			}
			else
			{
		#endif
		#ifdef USING_PCINT1
				setBit(&PCICR, PCIE1);
				setBit(&PCMSK1, s_gpio.number);
		#endif
		#ifdef USING_EXTINT
			}
		#endif
			break;
		case PC:
		#ifdef USING_PCINT2
			setBit(&PCICR, PCIE2);
			setBit(&PCMSK2, s_gpio.number);
		#endif
			break;
		case PD:
		#ifdef USING_EXTINT
			if (s_gpio.number == 2)
			{
				updateBit(&EICRA, ISC21, checkBit(e_interruptType, 1));
				updateBit(&EICRA, ISC20, checkBit(e_interruptType, 0));
				setBit(&EIMSK, INT0);
			}
			else if (s_gpio.number == 3)
			{
				updateBit(&EICRA, ISC21, checkBit(e_interruptType, 1));
				updateBit(&EICRA, ISC20, checkBit(e_interruptType, 0));
				setBit(&EIMSK, INT1);
			}
			else
			{
		#endif
		#ifdef USING_PCINT3
				setBit(&PCICR, PCIE3);
				setBit(&PCMSK3, s_gpio.number);
		#endif
		#ifdef USING_EXTINT
			}
		#endif
			break;
	}
}

void gpio_disableInterrupt(gpio_struct_t s_gpio, gpio_interruptType_enum_t e_interruptType)
{
	switch (s_gpio.port)
	{
		case PA:
		#ifdef USING_PCINT0
			clearBit(&PCICR, PCIE0);
			clearBit(&PCMSK0, s_gpio.number);
		#endif
			break;
		case PB:
		#ifdef USING_EXTINT
			if (s_gpio.number == 2)
			{
				updateBit(&EICRA, ISC21, checkBit(~e_interruptType, 1));
				updateBit(&EICRA, ISC20, checkBit(~e_interruptType, 0));
				clearBit(&EIMSK, INT2);
			}
			else
			{
		#endif
		#ifdef USING_PCINT1
				clearBit(&PCICR, PCIE1);
				clearBit(&PCMSK1, s_gpio.number);
		#endif
		#ifdef USING_EXTINT
			}
		#endif
			break;
		case PC:
		#ifdef USING_PCINT2
			clearBit(&PCICR, PCIE2);
			clearBit(&PCMSK2, s_gpio.number);
		#endif
			break;
		case PD:
		#ifdef USING_EXTINT
			if (s_gpio.number == 2)
			{
				updateBit(&EICRA, ISC21, checkBit(~e_interruptType, 1));
				updateBit(&EICRA, ISC20, checkBit(~e_interruptType, 0));
				clearBit(&EIMSK, INT0);
			}
			else if (s_gpio.number == 3)
			{
				updateBit(&EICRA, ISC21, checkBit(~e_interruptType, 1));
				updateBit(&EICRA, ISC20, checkBit(~e_interruptType, 0));
				clearBit(&EIMSK, INT1);
			}
			else
			{
		#endif
		#ifdef USING_PCINT3
				clearBit(&PCICR, PCIE3);
				clearBit(&PCMSK3, s_gpio.number);
		#endif
		#ifdef USING_EXTINT
			}
		#endif
			break;
	}
}

void gpio_attachInterrupt(gpio_struct_t s_gpio, gpio_interruptType_enum_t e_interruptType, void (*p_function)(void))
{
	switch (s_gpio.port)
	{
		case PA:
		#ifdef USING_PCINT0
			p_pcInt0Callback = p_function;
			u8_pcInt0OldState = PORTA;
		#endif
			break;
		case PB:
		#ifdef USING_EXTINT
			if (s_gpio.number == 2)
				p_extIntCallbacks[2] = p_function;
			else
			{
		#endif
		#ifdef USING_PCINT1
			p_pcInt1Callback = p_function;
			u8_pcInt1OldState = PORTB;
		#endif
		#ifdef USING_EXTINT
			}
		#endif
			break;
		case PC:
		#ifdef USING_PCINT2
			p_pcInt2Callback = p_function;
			u8_pcInt2OldState = PORTC;
		#endif
			break;
		case PD:
		#ifdef USING_EXTINT
			if (s_gpio.number == 2)
				p_extIntCallbacks[0] = p_function;
			else if (s_gpio.number == 3)
				p_extIntCallbacks[1] = p_function;
			else
			{
		#endif
		#ifdef USING_PCINT3
				p_pcInt3Callback = p_function;
		#endif
		#ifdef USING_EXTINT
			}
		#endif
			break;
	}
}

void gpio_detachInterrupt(gpio_struct_t s_gpio, gpio_interruptType_enum_t e_interruptType)
{
	switch (s_gpio.port)
	{
		case PA:
		#ifdef USING_PCINT0
			p_pcInt0Callback = 0;
		#endif
			break;
		case PB:
		#ifdef USING_EXTINT
			if (s_gpio.number == 2)
				p_extIntCallbacks[2] = 0;
			else
			{
			#endif
			#ifdef USING_PCINT1
				p_pcInt1Callback = 0;
			#endif
			#ifdef USING_EXTINT
			}
		#endif
			break;
		case PC:
		#ifdef USING_PCINT2
			p_pcInt2Callback = 0;
		#endif
			break;
		case PD:
		#ifdef USING_EXTINT
			if (s_gpio.number == 2)
				p_extIntCallbacks[0] = 0;
			else if (s_gpio.number == 3)
				p_extIntCallbacks[1] = 0;
			else
			{
			#endif
			#ifdef USING_PCINT3
				p_pcInt3Callback = 0;
				u8_pcInt3OldState = PORTD;
			#endif
			#ifdef USING_EXTINT
			}
		#endif
			break;
	}
}