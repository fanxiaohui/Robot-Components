/**	@file		inputCapture.c
	@brief		Input Capture features
	@details	See @link inputCapture.h @endlink for details.
*/

/************************************************************************/
/* AVR includes                                                         */
/************************************************************************/

#include <avr/io.h>

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "inputCapture.h"
#include "math.h"

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void inputCapture_init(inputCapture_struct_t s_inputCapture)
{
	timer_init(s_inputCapture.base);
	inputCapture_setEdge(s_inputCapture, s_inputCapture.edge);
	switch(s_inputCapture.base.peripheral)
	{
		case TIMER1:
		#ifdef USING_INPUTCAPTURE1
			updateBit(&ACSR, ACIC, s_inputCapture.source);
			updateBit(&TCCR1B, ICNC1, s_inputCapture.useNoiceCanceling);
		#endif
			break;
		case TIMER3:
		#ifdef USING_INPUTCAPTURE3
			updateBit(&TCCR3B, ICNC3, s_inputCapture.useNoiceCanceling);
		#endif
			break;
		default:
			break;
	}
}

void inputCapture_setEdge(inputCapture_struct_t s_inputCapture, inputCapture_edge_enum_t e_edge)
{
	switch(s_inputCapture.base.peripheral)
	{
		case TIMER1:
		#ifdef USING_INPUTCAPTURE1
			setBit(&TCCR1B, CS12);
			setBit(&TCCR1B, CS11);
			updateBit(&TCCR1B, CS10, e_edge);
		#endif
			break;
		case TIMER3:
		#ifdef USING_INPUTCAPTURE3
			setBit(&TCCR3B, CS32);
			setBit(&TCCR3B, CS31);
			updateBit(&TCCR3B, CS30, e_edge);
		#endif
			break;
		default:
			break;
	}
}

void inputCapture_start(inputCapture_struct_t s_inputCapture)
{
	timer_start(s_inputCapture.base);
}

void inputCapture_stop(inputCapture_struct_t s_inputCapture)
{
	timer_stop(s_inputCapture.base);
}

void inputCapture_enableInterrupt(inputCapture_struct_t s_inputCapture, timer_interruptType_enum_t e_interruptType)
{
	timer_enableInterrupt(s_inputCapture.base, e_interruptType);
}

void inputCapture_disableInterrupt(inputCapture_struct_t s_inputCapture, timer_interruptType_enum_t e_interruptType)
{
	timer_disableInterrupt(s_inputCapture.base, e_interruptType);
}

void inputCapture_attachInterrupt(inputCapture_struct_t s_inputCapture, timer_interruptType_enum_t e_interruptType, void (*p_function)(void))
{
	timer_attachInterrupt(s_inputCapture.base, e_interruptType, p_function);
}

void inputCapture_detachInterrupt(inputCapture_struct_t s_inputCapture, timer_interruptType_enum_t e_interruptType)
{
	timer_detachInterrupt(s_inputCapture.base, e_interruptType);
}

void inputCapture_resetValue(inputCapture_struct_t s_inputCapture)
{
	switch(s_inputCapture.base.peripheral)
	{
		case TIMER1:
		#ifdef USING_INPUTCAPTURE1
			ICR1 = 0;
		#endif
			break;
		case TIMER3:
		#ifdef USING_INPUTCAPTURE3
			ICR3 = 0;
		#endif
			break;
		default:
			break;
	}
}

void inputCapture_setValue(inputCapture_struct_t s_inputCapture, u16 u16_value)
{
	switch(s_inputCapture.base.peripheral)
	{
		case TIMER1:
		#ifdef USING_INPUTCAPTURE1
			ICR1 = u16_value;
		#endif
			break;
		case TIMER3:
		#ifdef USING_INPUTCAPTURE3
			ICR3 = u16_value;
		#endif
			break;
		default:
			break;
	}
}

u16 inputCapture_getValue(inputCapture_struct_t s_inputCapture)
{
	switch(s_inputCapture.base.peripheral)
	{
		case TIMER1:
		#ifdef USING_INPUTCAPTURE1
			return ICR1;
		#endif
			break;
		case TIMER3:
		#ifdef USING_INPUTCAPTURE3
			return ICR3;
		#endif
			break;
		default:
			return 0;
	}
	return 0;
}
