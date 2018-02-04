/**	@file		counter.c
	@brief		Timer features
	@details	See @link counter.h @endlink for details.
*/

/************************************************************************/
/* AVR includes                                                         */
/************************************************************************/

#include <avr/io.h>

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "counter.h"
#include "math.h"

/************************************************************************/
/* Internal variables                                                   */
/************************************************************************/

#ifdef USING_COUNTER0
	u8 u8_counter0ClockSource;
#endif
#ifdef USING_COUNTER1
	u8 u8_counter1ClockSource;
#endif
#ifdef USING_COUNTER3
	u8 u8_counter3ClockSource;
#endif

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void counter_init(counter_struct_t s_counter)
{
	if (s_counter.base.peripheral != TIMER2)
	{
		timer_init(s_counter.base);
		counter_setEdge(s_counter, s_counter.edge);
	}
}

void counter_setEdge(counter_struct_t s_counter, counter_edge_enum_t e_edge)
{
	switch(s_counter.base.peripheral)
	{
		case TIMER0:
		#ifdef USING_COUNTER0
			setBit(&u8_counter0ClockSource, CS02);
			setBit(&u8_counter0ClockSource, CS01);
			updateBit(&u8_counter0ClockSource, CS00, e_edge);
		#endif
			break;
		case TIMER1:
		#ifdef USING_COUNTER1
			setBit(&u8_counter1ClockSource, CS12);
			setBit(&u8_counter1ClockSource, CS11);
			updateBit(&u8_counter1ClockSource, CS10, e_edge);
		#endif
			break;
		case TIMER3:
		#ifdef USING_COUNTER3
			setBit(&u8_counter3ClockSource, CS32);
			setBit(&u8_counter3ClockSource, CS31);
			updateBit(&u8_counter3ClockSource, CS30, e_edge);
		#endif
			break;
		default:
			break;
	}
}

void counter_start(counter_struct_t s_counter)
{
	switch(s_counter.base.peripheral)
	{
		case TIMER0:
		#ifdef USING_COUNTER0
			updateBit(&TCCR0B, CS02, (u8_counter0ClockSource >> 2) & 1);
			updateBit(&TCCR0B, CS01, (u8_counter0ClockSource >> 1) & 1);
			updateBit(&TCCR0B, CS00, (u8_counter0ClockSource & 1));
		#endif
			break;
		case TIMER1:
		#ifdef USING_COUNTER1
			updateBit(&TCCR1B, CS12, (u8_counter1ClockSource >> 2) & 1);
			updateBit(&TCCR1B, CS11, (u8_counter1ClockSource >> 1) & 1);
			updateBit(&TCCR1B, CS10, (u8_counter1ClockSource & 1));
		#endif
			break;
		case TIMER3:
		#ifdef USING_COUNTER3
			updateBit(&TCCR3B, CS32, (u8_counter3ClockSource >> 2) & 1);
			updateBit(&TCCR3B, CS31, (u8_counter3ClockSource >> 1) & 1);
			updateBit(&TCCR3B, CS30, (u8_counter3ClockSource & 1));
		#endif
			break;
		default:
			break;
	}
}

void counter_stop(counter_struct_t s_counter)
{
	switch(s_counter.base.peripheral)
	{
		case TIMER0:
		#ifdef USING_COUNTER0
			u8_counter0ClockSource = TCCR0B & (0b00000111);
		#endif
			break;
		case TIMER1:
		#ifdef USING_COUNTER1
			u8_counter1ClockSource = TCCR1B & (0b00000111);
		#endif
			break;
		case TIMER3:
		#ifdef USING_COUNTER3
			u8_counter3ClockSource = TCCR3B & (0b00000111);
		#endif
			break;
		default:
			break;
	}
}

void counter_setCompareMatch(counter_struct_t s_counter, timer_channel_enum_t e_channel, u16 u16_numberOfEvents)
{
	switch(s_counter.base.peripheral)
	{
		case TIMER0:
		#ifdef USING_TIMER0
			if (e_channel == CHANNEL_A && !usingCompareRegisterAsMax(s_counter.base))
				OCR0A = (u8) u16_numberOfEvents;
			if (e_channel == CHANNEL_B)
				OCR0B = (u8) u16_numberOfEvents;
		#endif
			break;
		case TIMER1:
		#ifdef USING_TIMER1
			if (e_channel == CHANNEL_A)
			{
				#ifdef USING_INPUTCAPTURE1
				if (!usingCompareRegisterAsMax(s_counter.base))
				#endif
					OCR1A = u16_numberOfEvents;
			}
			if (e_channel == CHANNEL_B)
				OCR1B = u16_numberOfEvents;
		#endif
			break;
		case TIMER3:
		#ifdef USING_TIMER3
			if (e_channel == CHANNEL_A)
			{
				#ifdef USING_INPUTCAPTURE3
				if (!usingCompareRegisterAsMax(s_counter.base))
				#endif
					OCR3A = u16_numberOfEvents;
			}
			if (e_channel == CHANNEL_B)
				OCR3B = u16_numberOfEvents;
		#endif
			break;
		default:
			break;
	}
}

void counter_enableInterrupt(counter_struct_t s_counter, timer_interruptType_enum_t e_interruptType)
{
	timer_enableInterrupt(s_counter.base, e_interruptType);
}

void counter_disableInterrupt(counter_struct_t s_counter, timer_interruptType_enum_t e_interruptType)
{
	timer_disableInterrupt(s_counter.base, e_interruptType);
}

void counter_attachInterrupt(counter_struct_t s_counter, timer_interruptType_enum_t e_interruptType, void (*p_function)(void))
{
	timer_attachInterrupt(s_counter.base, e_interruptType, p_function);
}

void counter_detachInterrupt(counter_struct_t s_counter, timer_interruptType_enum_t e_interruptType)
{
	timer_detachInterrupt(s_counter.base, e_interruptType);
}

void counter_resetValue(counter_struct_t s_counter)
{
	timer_resetValue(s_counter.base);
}

void counter_setValue(counter_struct_t s_counter, u16 u16_value)
{
	timer_setValue(s_counter.base, u16_value);
}

u16 counter_getValue(counter_struct_t s_counter)
{
	return timer_getValue(s_counter.base);
}
