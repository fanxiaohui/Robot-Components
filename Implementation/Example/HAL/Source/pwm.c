/**	@file		pwm.c
	@brief		PWM features
	@author		Florin Popescu
	@version	1.0
	@date		08.09.2017
	@details	See pwm.h for details.
*/

/************************************************************************/
/* AVR includes                                                         */
/************************************************************************/

#include <avr/io.h>

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "gpio.h"
#include "math.h"
#include "pwm.h"

/************************************************************************/
/* Internal variables                                                   */
/************************************************************************/

#ifdef USING_PWM0
u8 u8_pwm0Percent;
#endif
#ifdef USING_PWM1
u8 u8_pwm1aPercent;
u8 u8_pwm1bPercent;
#endif
#ifdef USING_PWM2
u8 u8_pwm2Percent;
#endif
#ifdef USING_PWM3
u8 u8_pwm3aPercent;
u8 u8_pwm3bPercent;
#endif

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void pwm_init(pwm_struct_t *s_pwm)
{
	gpio_struct_t s_outputPin;
	s_outputPin.direction = OUTPUT;

	timer_init(s_pwm->base);
	switch(s_pwm->base.peripheral)
	{
		case TIMER0:
		#ifdef USING_PWM0
			if (!usingCompareRegisterAsMax(s_pwm->base))
				clearBit(&TCCR0B, WGM02);
			else
			{
				setBit(&TCCR0B, WGM02);
				s_pwm->channelA.enabled = FALSE;
			}
			switch(s_pwm->mode)
			{
				case FAST_PWM:
					setBit(&TCCR0A, WGM01);
					setBit(&TCCR0A, WGM00);
					break;
				case CENTERED_PWM:
					clearBit(&TCCR0A, WGM01);
					setBit(&TCCR0A, WGM00);
					break;
				default:
					break;
			}
			if (s_pwm->channelA.enabled)
			{
				setBit(&TCCR0A, COM0A1);
				updateBit(&TCCR0A, COM0A0, s_pwm->channelA.invertedOutput);
				s_outputPin.port = PB;
				s_outputPin.number = 3;
				gpio_init(s_outputPin);
			}
			if (s_pwm->channelB.enabled)
			{
				setBit(&TCCR0A, COM0B1);
				updateBit(&TCCR0A, COM0B0, s_pwm->channelB.invertedOutput);
				s_outputPin.port = PB;
				s_outputPin.number = 4;
				gpio_init(s_outputPin);
			}
			u8_pwm0Percent = 0;
		#endif
			break;
		case TIMER1:
		#ifdef USING_PWM1
			/* OCR register is set as max value from timer_init. In PWM however ICR register is used as max value. */
			if (OCR1A != 0)
				ICR1 = OCR1A;
			else
				ICR1 = 0xffff;
			setBit(&TCCR1B, WGM13);
			clearBit(&TCCR1A, WGM10);
			switch(s_pwm->mode)
			{
				case FAST_PWM:
					setBit(&TCCR1B, WGM12);
					setBit(&TCCR1A, WGM11);
					break;
				case CENTERED_PWM:
					clearBit(&TCCR1B, WGM12);
					if (s_pwm->signalType != DUTY_CYCLE_VARIABLE)
						clearBit(&TCCR1A, WGM11);
					else
						setBit(&TCCR1A, WGM11);
					break;
			}
			if (s_pwm->channelA.enabled)
			{
				setBit(&TCCR1A, COM1A1);
				updateBit(&TCCR1A, COM1A0, s_pwm->channelA.invertedOutput);
				s_outputPin.port = PD;
				s_outputPin.number = 5;
				gpio_init(s_outputPin);
			}
			if (s_pwm->channelB.enabled)
			{
				setBit(&TCCR1A, COM1B1);
				updateBit(&TCCR1A, COM1B0, s_pwm->channelB.invertedOutput);
				s_outputPin.port = PD;
				s_outputPin.number = 4;
				gpio_init(s_outputPin);
			}
			u8_pwm1aPercent = 0;
			u8_pwm1bPercent = 0;
		#endif
			break;
		case TIMER2:
		#ifdef USING_PWM2
			/* Set Timer2 as running synchronously with I/O clock to prevent further issues. */
			clearBit(&ASSR, AS2);
			if (!usingCompareRegisterAsMax(s_pwm->base))
				clearBit(&TCCR2B, WGM22);
			else
				setBit(&TCCR2B, WGM22);
			switch(s_pwm->mode)
			{
				case FAST_PWM:
					setBit(&TCCR2A, WGM21);
					setBit(&TCCR2A, WGM20);
					break;
				case CENTERED_PWM:
					clearBit(&TCCR2A, WGM21);
					setBit(&TCCR2A, WGM20);
					break;
				default:
					break;
			}
			if (s_pwm->channelA.enabled)
			{
				setBit(&TCCR2A, COM2A1);
				updateBit(&TCCR2A, COM2A0, s_pwm->channelA.invertedOutput);
				s_outputPin.port = PD;
				s_outputPin.number = 7;
				gpio_init(s_outputPin);
			}
			if (s_pwm->channelB.enabled)
			{
				setBit(&TCCR2A, COM2B1);
				updateBit(&TCCR2A, COM2B0, s_pwm->channelB.invertedOutput);
				s_outputPin.port = PD;
				s_outputPin.number = 6;
				gpio_init(s_outputPin);
			}
			u8_pwm2Percent = 0;
		#endif
			break;
		case TIMER3:
		#ifdef USING_PWM3
			/* OCR register is set as max value from timer_init. In PWM however ICR register is used as max value. */
			if (OCR3A != 0)
				ICR3 = OCR3A;
			else
				ICR3 = 0xffff;
			setBit(&TCCR3B, WGM33);
			clearBit(&TCCR3A, WGM30);
			switch(s_pwm->mode)
			{
				case FAST_PWM:
					setBit(&TCCR3B, WGM32);
					setBit(&TCCR3A, WGM31);
					break;
				case CENTERED_PWM:
					clearBit(&TCCR3B, WGM32);
					if (s_pwm->signalType != DUTY_CYCLE_VARIABLE)
						clearBit(&TCCR3A, WGM31);
					else
						setBit(&TCCR3A, WGM31);
					break;
			}
			if (s_pwm->channelA.enabled)
			{
				setBit(&TCCR3A, COM3A1);
				updateBit(&TCCR3A, COM3A0, s_pwm->channelA.invertedOutput);
				s_outputPin.port = PB;
				s_outputPin.number = 6;
				gpio_init(s_outputPin);
			}
			if (s_pwm->channelB.enabled)
			{
				setBit(&TCCR3A, COM3B1);
				updateBit(&TCCR3A, COM3B0, s_pwm->channelB.invertedOutput);
				s_outputPin.port = PB;
				s_outputPin.number = 7;
				gpio_init(s_outputPin);
			}
			u8_pwm3aPercent = 0;
			u8_pwm3bPercent = 0;
		#endif
			break;
	}
}

void pwm_start(pwm_struct_t s_pwm)
{
	timer_start(s_pwm.base);
}

void pwm_stop(pwm_struct_t s_pwm)
{
	timer_stop(s_pwm.base);
}

void pwm_setFrequency(pwm_struct_t s_pwm, u32 u32_frequency)
{
	u32 u32_prescaler;
	u16 u16_newTopRegisterValue = calculateTopRegister(s_pwm.base, u32_frequency, &u32_prescaler);

	switch(s_pwm.base.peripheral)
	{
		case TIMER0:
		#ifdef USING_PWM0
			OCR0A = (u8) u16_newTopRegisterValue;
			updateBit(&TCCR0B, CS02, (u32_prescaler >> 2) & 1);
			updateBit(&TCCR0B, CS01, (u32_prescaler >> 1) & 1);
			updateBit(&TCCR0B, CS00, (u32_prescaler & 1));
			OCR0B = (u8) ruleOfThree(100, u8_pwm0Percent, OCR0A);
		#endif
			break;
		case TIMER1:
		#ifdef USING_PWM1
			ICR1 = u16_newTopRegisterValue;
			updateBit(&TCCR1B, CS12, (u32_prescaler >> 2) & 1);
			updateBit(&TCCR1B, CS11, (u32_prescaler >> 1) & 1);
			updateBit(&TCCR1B, CS10, (u32_prescaler & 1));
			OCR1A = ruleOfThree(100, u8_pwm1aPercent, ICR1);
			OCR1B = ruleOfThree(100, u8_pwm1bPercent, ICR1);
		#endif
			break;
		case TIMER2:
		#ifdef USING_PWM2
			OCR2A = (u8) u16_newTopRegisterValue;
			updateBit(&TCCR2B, CS22, (u32_prescaler >> 2) & 1);
			updateBit(&TCCR2B, CS21, (u32_prescaler >> 1) & 1);
			updateBit(&TCCR2B, CS20, (u32_prescaler & 1));
			OCR2B = (u8) ruleOfThree(100, u8_pwm2Percent, OCR2A);
		#endif
			break;
		case TIMER3:
		#ifdef USING_PWM3
			ICR3 = u16_newTopRegisterValue;
			updateBit(&TCCR3B, CS32, (u32_prescaler >> 2) & 1);
			updateBit(&TCCR3B, CS31, (u32_prescaler >> 1) & 1);
			updateBit(&TCCR3B, CS30, (u32_prescaler & 1));
			OCR3A = ruleOfThree(100, u8_pwm3aPercent, ICR3);
			OCR3B = ruleOfThree(100, u8_pwm3bPercent, ICR3);
		#endif
			break;
	}
}

void pwm_setDutyCycle(pwm_struct_t s_pwm, timer_channel_enum_t e_channel, u8 u8_percent)
{
	switch(s_pwm.base.peripheral)
	{
		case TIMER0:
		#ifdef USING_PWM0
			if (e_channel == CHANNEL_A && !usingCompareRegisterAsMax(s_pwm.base))
				OCR0A = ruleOfThree(100, u8_percent, 0xff);
			if (e_channel == CHANNEL_B)
			{
				u8_pwm0Percent = u8_percent;
				if (!usingCompareRegisterAsMax(s_pwm.base))
					OCR0B = ruleOfThree(100, u8_percent, 0xff);
				else
					OCR0B = ruleOfThree(100, u8_percent, OCR0A);
			}
		#endif
			break;
		case TIMER1:
		#ifdef USING_PWM1
			if (e_channel == CHANNEL_A)
			{
				u8_pwm1aPercent = u8_percent;
				OCR1A = ruleOfThree(100, u8_percent, ICR1);
			}
			if (e_channel == CHANNEL_B)
			{
				u8_pwm1bPercent = u8_percent;
				OCR1B = ruleOfThree(100, u8_percent, ICR1);
			}
		#endif
			break;
		case TIMER2:
		#ifdef USING_PWM2
			if (e_channel == CHANNEL_A && !usingCompareRegisterAsMax(s_pwm.base))
			OCR2A = ruleOfThree(100, u8_percent, 0xff);
			if (e_channel == CHANNEL_B)
			{
				u8_pwm2Percent = u8_percent;
				if (!usingCompareRegisterAsMax(s_pwm.base))
					OCR2B = ruleOfThree(100, u8_percent, 0xff);
				else
					OCR2B = ruleOfThree(100, u8_percent, OCR2A);
			}
		#endif
			break;
		case TIMER3:
		#ifdef USING_PWM3
			if (e_channel == CHANNEL_A)
			{
				u8_pwm3aPercent = u8_percent;
				OCR3A = ruleOfThree(100, u8_percent, ICR3);
			}
			if (e_channel == CHANNEL_B)
			{
				u8_pwm3bPercent = u8_percent;
				OCR3B = ruleOfThree(100, u8_percent, ICR3);
			}
		#endif
			break;
	}
}
