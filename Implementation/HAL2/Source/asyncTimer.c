/**	@file		asyncTimer.c
	@brief		Asynchronous Timer features
	@details	See @link asyncTimer.h @endlink for details.
*/

/************************************************************************/
/* AVR includes                                                         */
/************************************************************************/

#include <avr/io.h>

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "asyncTimer.h"
#include "math.h"

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void asyncTimer_init(asyncTimer_struct_t s_asyncTimer)
{
	timer_struct_t s_baseTimer;
	s_baseTimer.peripheral = TIMER2;
	s_baseTimer.frequency = s_asyncTimer.prescaler;
	timer_init(s_baseTimer);
	switch(s_asyncTimer.source)
	{
		case CLOCK_SIGNAL:
			setBit(&ASSR, EXCLK);
			break;
		case OSCILLATOR:
			clearBit(&ASSR, EXCLK);
			break;
	}
}

void asyncTimer_start()
{
	setBit(&ASSR, AS2);
}

void asyncTimer_stop()
{
	/* Wait if one of the timer registers is currently being updated */
	while ((ASSR & 0b00011111) != 0);
	clearBit(&ASSR, AS2);
}

void asyncTimer_setCompareMatch(timer_channel_enum_t e_channel, u16 u16_numberOfEvents)
{
	if (e_channel == CHANNEL_A)
		OCR2A = (u8) u16_numberOfEvents;
	if (e_channel == CHANNEL_B)
		OCR2B = (u8) u16_numberOfEvents;
}

void asyncTimer_enableInterrupt(timer_interruptType_enum_t e_interruptType)
{
#ifdef TIMER2_INTERRUPT_MODE
	timer_struct_t s_baseTimer;
	s_baseTimer.peripheral = TIMER2;
	timer_enableInterrupt(s_baseTimer, e_interruptType);
#endif
}

void asyncTimer_disableInterrupt(timer_interruptType_enum_t e_interruptType)
{
#ifdef TIMER2_INTERRUPT_MODE
	timer_struct_t s_baseTimer;
	s_baseTimer.peripheral = TIMER2;
	timer_disableInterrupt(s_baseTimer, e_interruptType);
#endif
}

void asyncTimer_attachInterrupt(timer_interruptType_enum_t e_interruptType, void (*p_function)(void))
{
#ifdef TIMER2_INTERRUPT_MODE
	timer_struct_t s_baseTimer;
	s_baseTimer.peripheral = TIMER2;
	timer_attachInterrupt(s_baseTimer, e_interruptType, p_function);
#endif
}

void asyncTimer_detachInterrupt(timer_interruptType_enum_t e_interruptType)
{
#ifdef TIMER2_INTERRUPT_MODE
	timer_struct_t s_baseTimer;
	s_baseTimer.peripheral = TIMER2;
	timer_detachInterrupt(s_baseTimer, e_interruptType);
#endif
}

void asyncTimer_resetValue()
{
	timer_struct_t s_baseTimer;
	s_baseTimer.peripheral = TIMER2;
	timer_resetValue(s_baseTimer);
}

void asyncTimer_setValue(u16 u16_value)
{
	timer_struct_t s_baseTimer;
	s_baseTimer.peripheral = TIMER2;
	timer_setValue(s_baseTimer, u16_value);
}

u16 asyncTimer_getValue()
{
	timer_struct_t s_baseTimer;
	s_baseTimer.peripheral = TIMER2;
	return asyncTimer_getValue(s_baseTimer);
}
