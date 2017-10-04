/**	@file		timer.c
	@brief		Timer features
	@author		Florin Popescu
	@version	1.0
	@date		08.09.2017
	@details	See timer.h for details.
*/

/************************************************************************/
/* AVR includes                                                         */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "device.h"
#include "math.h"
#include "timer.h"

/************************************************************************/
/* Internal variables                                                   */
/************************************************************************/

#ifdef USING_TIMER0
u8 u8_timer0ClockSource;
void (*p_timer0Callbacks[3])(void);
#endif
#ifdef USING_TIMER1
u8 u8_timer1ClockSource;
void (*p_timer1Callbacks[4])(void);
#endif
#ifdef USING_TIMER2
u8 u8_timer2ClockSource;
void (*p_timer2Callbacks[3])(void);
#endif
#ifdef USING_TIMER3
u8 u8_timer3ClockSource;
void (*p_timer3Callbacks[4])(void);
#endif

/************************************************************************/
/* Internal functions                                                   */
/************************************************************************/

u16 calculateTopRegister(timer_struct_t s_timer, u32 u32_frequency, u32 *pu32_prescaler)
{
	u32 u32_returnValue = 0;
	u16 u16_divisor;

	switch(s_timer.peripheral)
	{
		case TIMER0:
		#ifdef USING_TIMER0
			u16_divisor = 1;
			*pu32_prescaler = TIMER_CLK_DIV_1;
			u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
			if (u32_returnValue > 0xff)
			{
				u16_divisor = 8;
				*pu32_prescaler = TIMER_CLK_DIV_8;
				u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
				if (u32_returnValue > 0xff)
				{
					u16_divisor = 64;
					*pu32_prescaler = TIMER_CLK_DIV_64;
					u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
					if (u32_returnValue > 0xff)
					{
						u16_divisor = 256;
						*pu32_prescaler = TIMER_CLK_DIV_256;
						u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
						if (u32_returnValue > 0xff)
						{
							u16_divisor = 1024;
							*pu32_prescaler = TIMER_CLK_DIV_1024;
							u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
						}
					}
				}
			}
		#endif
			break;
		case TIMER1:
		#ifdef USING_TIMER1
			u16_divisor = 1;
			*pu32_prescaler = TIMER_CLK_DIV_1;
			u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
			if (u32_returnValue > 0xffff)
			{
				u16_divisor = 8;
				*pu32_prescaler = TIMER_CLK_DIV_8;
				u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
				if (u32_returnValue > 0xffff)
				{
					u16_divisor = 64;
					*pu32_prescaler = TIMER_CLK_DIV_64;
					u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
					if (u32_returnValue > 0xffff)
					{
						u16_divisor = 256;
						*pu32_prescaler = TIMER_CLK_DIV_256;
						u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
						if (u32_returnValue > 0xffff)
						{
							u16_divisor = 1024;
							*pu32_prescaler = TIMER_CLK_DIV_1024;
							u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
						}
					}
				}
			}
		#endif
			break;
		case TIMER3:
		#ifdef USING_TIMER3
			u16_divisor = 1;
			*pu32_prescaler = TIMER_CLK_DIV_1;
			u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
			if (u32_returnValue > 0xffff)
			{
				u16_divisor = 8;
				*pu32_prescaler = TIMER_CLK_DIV_8;
				u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
				if (u32_returnValue > 0xffff)
				{
					u16_divisor = 64;
					*pu32_prescaler = TIMER_CLK_DIV_64;
					u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
					if (u32_returnValue > 0xffff)
					{
						u16_divisor = 256;
						*pu32_prescaler = TIMER_CLK_DIV_256;
						u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
						if (u32_returnValue > 0xffff)
						{
							u16_divisor = 1024;
							*pu32_prescaler = TIMER_CLK_DIV_1024;
							u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
						}
					}
				}
			}
		#endif
			break;
		case TIMER2:
		#ifdef USING_TIMER2
			u16_divisor = 1;
			*pu32_prescaler = TIMER_CLK_DIV_1;
			u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
			if (u32_returnValue > 0xff)
			{
				u16_divisor = 8;
				*pu32_prescaler = TIMER_CLK_DIV_8;
				u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
				if (u32_returnValue > 0xff)
				{
					u16_divisor = 32;
					*pu32_prescaler = TIMER_CLK_DIV_32;
					u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
					if (u32_returnValue > 0xff)
					{
						u16_divisor = 64;
						*pu32_prescaler = TIMER_CLK_DIV_64;
						u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
						if (u32_returnValue > 0xff)
						{
							u16_divisor = 128;
							*pu32_prescaler = TIMER_CLK_DIV_128;
							u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
							if (u32_returnValue > 0xff)
							{
								u16_divisor = 256;
								*pu32_prescaler = TIMER_CLK_DIV_256;
								u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
								if (u32_returnValue > 0xff)
								{
									u16_divisor = 1024;
									*pu32_prescaler = TIMER_CLK_DIV_1024;
									u32_returnValue = SYSTEM_CLOCK_FREQUENCY / u16_divisor / u32_frequency - 1;
								}
							}
						}
					}
				}
			}
		#endif
			break;
	}

	return (u16) u32_returnValue;
}

/************************************************************************/
/* Interrupt handlers                                                   */
/************************************************************************/

#ifdef TIMER0_INTERRUPT_MODE
ISR(TIMER0_OVF_vect)
{
	p_timer0Callbacks[OVERFLOW]();
}

ISR(TIMER0_COMPA_vect)
{
	u8 u8_wgm = checkBit(TCCR0B, WGM02) << 2 | checkBit(TCCR0A, WGM01) << 1 | checkBit(TCCR0A, WGM00);
	if (u8_wgm < 4 && u8_wgm != 2)
		p_timer0Callbacks[COMPARE_MATCH_A]();
	else
		p_timer0Callbacks[OVERFLOW]();
}

ISR(TIMER0_COMPB_vect)
{
	p_timer0Callbacks[COMPARE_MATCH_B]();
}
#endif

#ifdef TIMER1_INTERRUPT_MODE
ISR(TIMER1_OVF_vect)
{
	p_timer1Callbacks[OVERFLOW]();
}

ISR(TIMER1_COMPA_vect)
{
	u8 u8_wgm = checkBit(TCCR1B, WGM13) << 3 | checkBit(TCCR1B, WGM12) << 2 | checkBit(TCCR1A, WGM11) << 1 | checkBit(TCCR1A, WGM10);
	if ((u8_wgm < 8 && u8_wgm != 4) || (u8_wgm >= 8 && u8_wgm % 2 == 0))
		p_timer1Callbacks[COMPARE_MATCH_A]();
	else
		p_timer1Callbacks[OVERFLOW]();
}

ISR(TIMER1_COMPB_vect)
{
	p_timer1Callbacks[COMPARE_MATCH_B]();
}

ISR(TIMER1_CAPT_vect)
{
	u8 u8_wgm = checkBit(TCCR1B, WGM13) << 3 | checkBit(TCCR1B, WGM12) << 2 | checkBit(TCCR1A, WGM11) << 1 | checkBit(TCCR1A, WGM10);
	if (!(u8_wgm >= 8 && u8_wgm % 2 == 0))
		p_timer1Callbacks[EXTERNAL]();
	else
		p_timer1Callbacks[OVERFLOW]();
}
#endif

#ifdef TIMER2_INTERRUPT_MODE
ISR(TIMER2_OVF_vect)
{
	p_timer2Callbacks[OVERFLOW]();
}

ISR(TIMER2_COMPA_vect)
{
	if ((checkBit(TCCR2B, WGM22) == CLEAR) && ((TCCR2A & 0b11) != 0b10))
		p_timer2Callbacks[COMPARE_MATCH_A]();
	else
		p_timer2Callbacks[OVERFLOW]();
}

ISR(TIMER2_COMPB_vect)
{
	p_timer2Callbacks[COMPARE_MATCH_B]();
}
#endif

#ifdef TIMER3_INTERRUPT_MODE
ISR(TIMER3_OVF_vect)
{
	p_timer3Callbacks[OVERFLOW]();
}

ISR(TIMER3_COMPA_vect)
{
	u8 u8_wgm = checkBit(TCCR3B, WGM33) << 3 | checkBit(TCCR3B, WGM32) << 2 | checkBit(TCCR3A, WGM31) << 1 | checkBit(TCCR3A, WGM30);
	if ((u8_wgm < 8 && u8_wgm != 4) || (u8_wgm >= 8 && u8_wgm % 2 == 0))
		p_timer3Callbacks[COMPARE_MATCH_A]();
	else
		p_timer3Callbacks[OVERFLOW]();
}

ISR(TIMER3_COMPB_vect)
{
	p_timer3Callbacks[COMPARE_MATCH_B]();
}

ISR(TIMER3_CAPT_vect)
{
	u8 u8_wgm = checkBit(TCCR3B, WGM33) << 3 | checkBit(TCCR3B, WGM32) << 2 | checkBit(TCCR3A, WGM31) << 1 | checkBit(TCCR3A, WGM30);
	if (!(u8_wgm >= 8 && u8_wgm % 2 == 0))
		p_timer3Callbacks[EXTERNAL]();
	else
		p_timer3Callbacks[OVERFLOW]();
}
#endif

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void timer_init(timer_struct_t s_timer)
{
	u32 u32_prescaler = 0;

	switch (s_timer.peripheral)
	{
		case TIMER0:
		#ifdef USING_TIMER0
			if (s_timer.frequency >= TIMER_CLK_DIV_1)
				u32_prescaler = s_timer.frequency;
			else
			{
				OCR0A = calculateTopRegister(s_timer, s_timer.frequency, &u32_prescaler);
				setBit(&TCCR0A, WGM01);
			}
			switch (u32_prescaler)
			{
				case TIMER_CLK_DIV_1:
					u32_prescaler = 0b001;
					break;
				case TIMER_CLK_DIV_8:
					u32_prescaler = 0b010;
					break;
				case TIMER_CLK_DIV_64:
					u32_prescaler = 0b011;
					break;
				case TIMER_CLK_DIV_256:
					u32_prescaler = 0b100;
					break;
				case TIMER_CLK_DIV_1024:
					u32_prescaler = 0b101;
					break;
			}
			updateBit(&u8_timer0ClockSource, CS02, (u32_prescaler >> 2) & 1);
			updateBit(&u8_timer0ClockSource, CS01, (u32_prescaler >> 1) & 1);
			updateBit(&u8_timer0ClockSource, CS00, (u32_prescaler & 1));
		#endif
			break;
		case TIMER1:
		#ifdef USING_TIMER1
			if (s_timer.frequency >= TIMER_CLK_DIV_1)
				u32_prescaler = s_timer.frequency;
			else
			{
				#ifdef USING_INPUTCAPTURE1
					OCR1A = calculateTopRegister(s_timer, s_timer.frequency, &u32_prescaler);
				#else
					ICR1 = calculateTopRegister(s_timer, s_timer.frequency, &u32_prescaler);
					setBit(&TCCR1B, WGM13);
				#endif
				setBit(&TCCR1B, WGM12);
			}
			switch(u32_prescaler)
			{
				case TIMER_CLK_DIV_1:
					u32_prescaler = 0b001;
					break;
				case TIMER_CLK_DIV_8:
					u32_prescaler = 0b010;
					break;
				case TIMER_CLK_DIV_64:
					u32_prescaler = 0b011;
					break;
				case TIMER_CLK_DIV_256:
					u32_prescaler = 0b100;
					break;
				case TIMER_CLK_DIV_1024:
					u32_prescaler = 0b101;
					break;
			}
			updateBit(&u8_timer1ClockSource, CS12, (u32_prescaler >> 2) & 1);
			updateBit(&u8_timer1ClockSource, CS11, (u32_prescaler >> 1) & 1);
			updateBit(&u8_timer1ClockSource, CS10, (u32_prescaler & 1));
		#endif
			break;
		case TIMER2:
		#ifdef USING_TIMER2
			if (s_timer.frequency >= TIMER_CLK_DIV_1)
				u32_prescaler = s_timer.frequency;
			else
			{
				OCR2A = calculateTopRegister(s_timer, s_timer.frequency, &u32_prescaler);
				setBit(&TCCR2A, WGM21);
			}
			switch(u32_prescaler)
			{
				case TIMER_CLK_DIV_1:
					u32_prescaler = 0b001;
					break;
				case TIMER_CLK_DIV_8:
					u32_prescaler = 0b010;
					break;
				case TIMER_CLK_DIV_32:
					u32_prescaler = 0b011;
					break;
				case TIMER_CLK_DIV_64:
					u32_prescaler = 0b100;
					break;
				case TIMER_CLK_DIV_128:
					u32_prescaler = 0b101;
					break;
				case TIMER_CLK_DIV_256:
					u32_prescaler = 0b110;
					break;
				case TIMER_CLK_DIV_1024:
					u32_prescaler = 0b111;
					break;
			}
			updateBit(&u8_timer2ClockSource, CS22, (u32_prescaler >> 2) & 1);
			updateBit(&u8_timer2ClockSource, CS21, (u32_prescaler >> 1) & 1);
			updateBit(&u8_timer2ClockSource, CS20, (u32_prescaler & 1));
		#endif
			break;
		case TIMER3:
		#ifdef USING_TIMER3
			if (s_timer.frequency >= TIMER_CLK_DIV_1)
				u32_prescaler = s_timer.frequency;
			else
			{
				#ifdef USING_INPUTCAPTURE1
					OCR3A = calculateTopRegister(s_timer, s_timer.frequency, &u32_prescaler);
				#else
					ICR3 = calculateTopRegister(s_timer, s_timer.frequency, &u32_prescaler);
					setBit(&TCCR3B, WGM33);
				#endif
				setBit(&TCCR3B, WGM32);
			}
			switch(u32_prescaler)
			{
				case TIMER_CLK_DIV_1:
					u32_prescaler = 0b001;
					break;
				case TIMER_CLK_DIV_8:
					u32_prescaler = 0b010;
					break;
				case TIMER_CLK_DIV_64:
					u32_prescaler = 0b011;
					break;
				case TIMER_CLK_DIV_256:
					u32_prescaler = 0b100;
					break;
				case TIMER_CLK_DIV_1024:
					u32_prescaler = 0b101;
					break;
			}
			updateBit(&u8_timer3ClockSource, CS32, (u32_prescaler >> 2) & 1);
			updateBit(&u8_timer3ClockSource, CS31, (u32_prescaler >> 1) & 1);
			updateBit(&u8_timer3ClockSource, CS30, (u32_prescaler & 1));
		#endif
			break;
	}
}

void timer_prepareSynchronisedStart()
{
	setBit(&GTCCR, TSM);
	setBit(&GTCCR, PSRSYNC);
}

void timer_synchronisedStart()
{
	clearBit(&GTCCR, TSM);
}

void timer_start(timer_struct_t s_timer)
{
	switch(s_timer.peripheral)
	{
		case TIMER0:
		#ifdef USING_TIMER0
			updateBit(&TCCR0B, CS02, (u8_timer0ClockSource >> 2) & 1);
			updateBit(&TCCR0B, CS01, (u8_timer0ClockSource >> 1) & 1);
			updateBit(&TCCR0B, CS00, (u8_timer0ClockSource & 1));
		#endif
			break;
		case TIMER1:
		#ifdef USING_TIMER1
			updateBit(&TCCR1B, CS12, (u8_timer1ClockSource >> 2) & 1);
			updateBit(&TCCR1B, CS11, (u8_timer1ClockSource >> 1) & 1);
			updateBit(&TCCR1B, CS10, (u8_timer1ClockSource & 1));
		#endif
			break;
		case TIMER2:
		#ifdef USING_TIMER2
			updateBit(&TCCR2B, CS22, (u8_timer2ClockSource >> 2) & 1);
			updateBit(&TCCR2B, CS21, (u8_timer2ClockSource >> 1) & 1);
			updateBit(&TCCR2B, CS20, (u8_timer2ClockSource & 1));
		#endif
			break;
		case TIMER3:
		#ifdef USING_TIMER3
			updateBit(&TCCR3B, CS32, (u8_timer3ClockSource >> 2) & 1);
			updateBit(&TCCR3B, CS31, (u8_timer3ClockSource >> 1) & 1);
			updateBit(&TCCR3B, CS30, (u8_timer3ClockSource & 1));
		#endif
			break;
	}
}

void timer_stop(timer_struct_t s_timer)
{
	switch(s_timer.peripheral)
	{
		case TIMER0:
		#ifdef USING_TIMER0
			u8_timer0ClockSource = TCCR3B & (0b00000111);
		#endif
			break;
		case TIMER1:
		#ifdef USING_TIMER1
			u8_timer1ClockSource = TCCR3B & (0b00000111);
		#endif
			break;
		case TIMER2:
		#ifdef USING_TIMER2
			u8_timer2ClockSource = TCCR3B & (0b00000111);
		#endif
			break;
		case TIMER3:
		#ifdef USING_TIMER3
			u8_timer3ClockSource = TCCR3B & (0b00000111);
		#endif
			break;
	}
}

bool usingCompareRegisterAsMax(timer_struct_t s_timer)
{
	return s_timer.frequency < TIMER_CLK_DIV_1;
}

void timer_setCompareMatch(timer_struct_t s_timer, timer_channel_enum_t e_channel, u8 u8_percent)
{
	switch(s_timer.peripheral)
	{
		case TIMER0:
		#ifdef USING_TIMER0
			if (e_channel == CHANNEL_A && !usingCompareRegisterAsMax(s_timer))
				OCR0A = ruleOfThree(100, u8_percent, 0xff);
			if (e_channel == CHANNEL_B)
			{
				if (!usingCompareRegisterAsMax(s_timer))
					OCR0B = ruleOfThree(100, u8_percent, 0xff);
				else
					OCR0B = ruleOfThree(100, u8_percent, OCR0A);
			}
		#endif
			break;
		case TIMER1:
		#ifdef USING_TIMER1
			if (e_channel == CHANNEL_A)
			{
				if (!usingCompareRegisterAsMax(s_timer))
					OCR1A = ruleOfThree(100, u8_percent, 0xffff);
				#ifndef USING_INPUTCAPTURE1
				else
					OCR1A = ruleOfThree(100, u8_percent, ICR1);
				#endif
			}
			if (e_channel == CHANNEL_B)
			{
				if (usingCompareRegisterAsMax(s_timer))
				#ifdef USING_INPUTCAPTURE1
					OCR1B = ruleOfThree(100, u8_percent, OCR1A);
				#else
					OCR1B = ruleOfThree(100, u8_percent, ICR1);
				#endif
				else
					OCR1B = ruleOfThree(100, u8_percent, 0xffff);
			}
		#endif
			break;
		case TIMER2:
		#ifdef USING_TIMER2
			if (e_channel == CHANNEL_A && !usingCompareRegisterAsMax(s_timer))
				OCR2A = ruleOfThree(100, u8_percent, 0xff);
			if (e_channel == CHANNEL_B)
			{
				if (!usingCompareRegisterAsMax(s_timer))
					OCR2B = ruleOfThree(100, u8_percent, 0xff);
				else
					OCR2B = ruleOfThree(100, u8_percent, OCR2A);
			}
		#endif
			break;
		case TIMER3:
		#ifdef USING_TIMER3
			if (e_channel == CHANNEL_A)
			{
				if (!usingCompareRegisterAsMax(s_timer))
					OCR3A = ruleOfThree(100, u8_percent, 0xffff);
				#ifndef USING_INPUTCAPTURE1
				else
					OCR3A = ruleOfThree(100, u8_percent, ICR3);
				#endif
			}
			if (e_channel == CHANNEL_B)
			{
				if (usingCompareRegisterAsMax(s_timer))
				#ifdef USING_INPUTCAPTURE1
					OCR3B = ruleOfThree(100, u8_percent, OCR3A);
				#else
					OCR3B = ruleOfThree(100, u8_percent, ICR3);
				#endif
				else
					OCR3B = ruleOfThree(100, u8_percent, 0xffff);
			}
		#endif
			break;
	}
}

void timer_enableInterrupt(timer_struct_t s_timer, timer_interruptType_enum_t e_interruptType)
{
	u8 u8_wgm;

	switch (s_timer.peripheral)
	{
		case TIMER0:
		#ifdef TIMER0_INTERRUPT_MODE
			u8_wgm = checkBit(TCCR0B, WGM02) << 2 | checkBit(TCCR0A, WGM01) << 1 | checkBit(TCCR0A, WGM00);
			switch(e_interruptType)
			{
				case OVERFLOW:
					if (u8_wgm < 4 && u8_wgm != 2)
						setBit(&TIMSK0, TOIE0);
					else
						setBit(&TIMSK0, OCIE0A);
					break;
				case COMPARE_MATCH_A:
					if (u8_wgm < 4 && u8_wgm != 2)
						setBit(&TIMSK0, OCIE0A);
					break;
				case COMPARE_MATCH_B:
					setBit(&TIMSK0, OCIE0B);
					break;
				default:
					break;
			}
		#endif
			break;
		case TIMER1:
		#ifdef TIMER1_INTERRUPT_MODE
			u8_wgm = checkBit(TCCR1B, WGM13) << 3 | checkBit(TCCR1B, WGM12) << 2 | checkBit(TCCR1A, WGM11) << 1 | checkBit(TCCR1A, WGM10);
			switch(e_interruptType)
			{
				case OVERFLOW:
					if (u8_wgm < 8 && u8_wgm != 4)
						setBit(&TIMSK1, TOIE1);
					else if (u8_wgm >= 8 && u8_wgm % 2 == 0)
						setBit(&TIMSK1, ICIE1);
					else
						setBit(&TIMSK1, OCIE1A);
					break;
				case COMPARE_MATCH_A:
					if ((u8_wgm < 8 && u8_wgm != 4) || (u8_wgm >= 8 && u8_wgm % 2 == 0))
						setBit(&TIMSK1, OCIE1A);
					break;
				case COMPARE_MATCH_B:
						setBit(&TIMSK1, OCIE1B);
					break;
				case EXTERNAL:
					if (!(u8_wgm >= 8 && u8_wgm % 2 == 0))
						setBit (&TIMSK1, ICIE1);
					break;
			}
		#endif
			break;
		case TIMER2:
		#ifdef TIMER2_INTERRUPT_MODE
			u8_wgm = checkBit(TCCR2B, WGM22) << 2 | checkBit(TCCR2A, WGM21) << 1 | checkBit(TCCR2A, WGM20);
			switch(e_interruptType)
			{
				case OVERFLOW:
					if (u8_wgm < 4 && u8_wgm != 2)
						setBit(&TIMSK2, TOIE2);
					else
						setBit(&TIMSK2, OCIE2A);
					break;
				case COMPARE_MATCH_A:
					if (u8_wgm < 4 && u8_wgm != 2)
						setBit(&TIMSK2, OCIE2A);
					break;
				case COMPARE_MATCH_B:
					setBit(&TIMSK2, OCIE2B);
					break;
				default:
					break;
			}
		#endif
			break;
		case TIMER3:
		#ifdef TIMER3_INTERRUPT_MODE
			u8_wgm = checkBit(TCCR3B, WGM33) << 3 | checkBit(TCCR3B, WGM32) << 2 | checkBit(TCCR3A, WGM31) << 1 | checkBit(TCCR3A, WGM30);
			switch(e_interruptType)
			{
				case OVERFLOW:
					if (u8_wgm < 8 && u8_wgm != 4)
						setBit(&TIMSK3, TOIE3);
					else if (u8_wgm >= 8 && u8_wgm % 2 == 0)
						setBit(&TIMSK3, ICIE3);
					else
						setBit(&TIMSK3, OCIE3A);
					break;
				case COMPARE_MATCH_A:
					if ((u8_wgm < 8 && u8_wgm != 4) || (u8_wgm >= 8 && u8_wgm % 2 == 0))
						setBit(&TIMSK3, OCIE3A);
					break;
				case COMPARE_MATCH_B:
					setBit(&TIMSK3, OCIE3B);
					break;
				case EXTERNAL:
					if (!(u8_wgm >= 8 && u8_wgm % 2 == 0))
						setBit (&TIMSK3, ICIE3);
					break;
			}
		#endif
			break;
	}
}

void timer_disableInterrupt(timer_struct_t s_timer, timer_interruptType_enum_t e_interruptType)
{
	u8 u8_wgm;

	switch (s_timer.peripheral)
	{
		case TIMER0:
		#ifdef TIMER0_INTERRUPT_MODE
			u8_wgm = checkBit(TCCR0B, WGM02) << 2 | checkBit(TCCR0A, WGM01) << 1 | checkBit(TCCR0A, WGM00);
			switch(e_interruptType)
			{
				case OVERFLOW:
					if (u8_wgm < 4 && u8_wgm != 2)
						clearBit(&TIMSK0, TOIE0);
					else
						clearBit(&TIMSK0, OCIE0A);
					break;
				case COMPARE_MATCH_A:
					if (u8_wgm < 4 && u8_wgm != 2)
						clearBit(&TIMSK0, OCIE0A);
					break;
				case COMPARE_MATCH_B:
					clearBit(&TIMSK0, OCIE0B);
					break;
				default:
					break;
			}
		#endif
			break;
		case TIMER1:
		#ifdef TIMER1_INTERRUPT_MODE
			u8_wgm = checkBit(TCCR1B, WGM13) << 3 | checkBit(TCCR1B, WGM12) << 2 | checkBit(TCCR1A, WGM11) << 1 | checkBit(TCCR1A, WGM10);
			switch(e_interruptType)
			{
				case OVERFLOW:
					if (u8_wgm < 8 && u8_wgm != 4)
						clearBit(&TIMSK1, TOIE1);
					else if (u8_wgm >= 8 && u8_wgm % 2 == 0)
						clearBit(&TIMSK1, ICIE1);
					else
						clearBit(&TIMSK1, OCIE1A);
					break;
				case COMPARE_MATCH_A:
					if ((u8_wgm < 8 && u8_wgm != 4) || (u8_wgm >= 8 && u8_wgm % 2 == 0))
						clearBit(&TIMSK1, OCIE1A);
					break;
				case COMPARE_MATCH_B:
					clearBit(&TIMSK1, OCIE1B);
					break;
				case EXTERNAL:
					if (!(u8_wgm >= 8 && u8_wgm % 2 == 0))
						clearBit (&TIMSK1, ICIE1);
					break;
			}
		#endif
			break;
		case TIMER2:
		#ifdef TIMER2_INTERRUPT_MODE
			u8_wgm = checkBit(TCCR2B, WGM22) << 2 | checkBit(TCCR2A, WGM21) << 1 | checkBit(TCCR2A, WGM20);
			switch(e_interruptType)
			{
				case OVERFLOW:
					if (u8_wgm < 4 && u8_wgm != 2)
						clearBit(&TIMSK2, TOIE2);
					else
						clearBit(&TIMSK2, OCIE2A);
					break;
				case COMPARE_MATCH_A:
					if (u8_wgm < 4 && u8_wgm != 2)
						clearBit(&TIMSK2, OCIE2A);
					break;
				case COMPARE_MATCH_B:
					clearBit(&TIMSK2, OCIE2B);
					break;
				default:
					break;
			}
		#endif
			break;
		case TIMER3:
		#ifdef TIMER3_INTERRUPT_MODE
			u8_wgm = checkBit(TCCR3B, WGM33) << 3 | checkBit(TCCR3B, WGM32) << 2 | checkBit(TCCR3A, WGM31) << 1 | checkBit(TCCR3A, WGM30);
			switch(e_interruptType)
			{
				case OVERFLOW:
					if (u8_wgm < 8 && u8_wgm != 4)
						clearBit(&TIMSK3, TOIE3);
					else if (u8_wgm >= 8 && u8_wgm % 2 == 0)
						clearBit(&TIMSK3, ICIE3);
					else
						clearBit(&TIMSK3, OCIE3A);
					break;
				case COMPARE_MATCH_A:
					if ((u8_wgm < 8 && u8_wgm != 4) || (u8_wgm >= 8 && u8_wgm % 2 == 0))
						clearBit(&TIMSK3, OCIE3A);
					break;
				case COMPARE_MATCH_B:
					clearBit(&TIMSK3, OCIE3B);
					break;
				case EXTERNAL:
					if (!(u8_wgm >= 8 && u8_wgm % 2 == 0))
						clearBit (&TIMSK3, ICIE3);
					break;
			}
		#endif
			break;
	}
}

void timer_attachInterrupt(timer_struct_t s_timer, timer_interruptType_enum_t e_interruptType, void (*p_function)(void))
{
	switch (s_timer.peripheral)
	{
		case TIMER0:
		#ifdef TIMER0_INTERRUPT_MODE
			p_timer0Callbacks[e_interruptType] = p_function;
		#endif
			break;
		case TIMER1:
		#ifdef TIMER1_INTERRUPT_MODE
			p_timer1Callbacks[e_interruptType] = p_function;
		#endif
			break;
		case TIMER2:
		#ifdef TIMER2_INTERRUPT_MODE
			p_timer2Callbacks[e_interruptType] = p_function;
		#endif
			break;
		case TIMER3:
		#ifdef TIMER3_INTERRUPT_MODE
			p_timer3Callbacks[e_interruptType] = p_function;
		#endif
			break;
	}
}

void timer_detachInterrupt(timer_struct_t s_timer, timer_interruptType_enum_t e_interruptType)
{
	switch (s_timer.peripheral)
	{
		case TIMER0:
		#ifdef TIMER0_INTERRUPT_MODE
			p_timer0Callbacks[e_interruptType] = 0;
		#endif
			break;
		case TIMER1:
		#ifdef TIMER1_INTERRUPT_MODE
			p_timer1Callbacks[e_interruptType] = 0;
		#endif
			break;
		case TIMER2:
		#ifdef TIMER2_INTERRUPT_MODE
			p_timer2Callbacks[e_interruptType] = 0;
		#endif
			break;
		case TIMER3:
		#ifdef TIMER3_INTERRUPT_MODE
			p_timer3Callbacks[e_interruptType] = 0;
		#endif
			break;
	}
}

void timer_resetValue(timer_struct_t s_timer)
{
	switch (s_timer.peripheral)
	{
		case TIMER0:
		#ifdef USING_TIMER0
			TCNT0 = 0;
		#endif
			break;
		case TIMER1:
		#ifdef USING_TIMER1
			TCNT1 = 0;
		#endif
			break;
		case TIMER2:
		#ifdef USING_TIMER2
			TCNT2 = 0;
		#endif
			break;
		case TIMER3:
		#ifdef USING_TIMER3
			TCNT3 = 0;
		#endif
			break;
	}
}

void timer_setValue(timer_struct_t s_timer, u16 u16_value)
{
	switch (s_timer.peripheral)
	{
		case TIMER0:
		#ifdef USING_TIMER0
			TCNT0 = (u8) u16_value;
		#endif
			break;
		case TIMER1:
		#ifdef USING_TIMER1
			TCNT1 = u16_value;
		#endif
			break;
		case TIMER2:
		#ifdef USING_TIMER2
			TCNT2 = (u8) u16_value;
		#endif
			break;
		case TIMER3:
		#ifdef USING_TIMER3
			TCNT3 = u16_value;
		#endif
			break;
	}
}

u16 timer_getValue(timer_struct_t s_timer)
{
	switch (s_timer.peripheral)
	{
		case TIMER0:
		#ifdef USING_TIMER0
			return TCNT0;
		#endif
			break;
		case TIMER1:
		#ifdef USING_TIMER1
			return TCNT1;
		#endif
			break;
		case TIMER2:
		#ifdef USING_TIMER2
			return TCNT2;
		#endif
			break;
		case TIMER3:
		#ifdef USING_TIMER3
			return TCNT3;
		#endif
			break;
}
return 0;
}

void timer_delayMs(u32 milliseconds)
{
}

void timer_delayUs(u16 microseconds)
{
}