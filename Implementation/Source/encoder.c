/**	@file		encoder.c
	@brief		Encoder features
	@author		Adrian Grosu
	@version	1.0
	@date		31.10.2017
	@details	See encoder.h for details.
*/

/************************************************************************/
/* AVR includes                                                         */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "encoder.h"
#include "encoder_config.h"
#include "math.h"
#include "debug.h"

/************************************************************************/
/* Internal variables                                                   */
/************************************************************************/

extern encoder_struct_t s_leftEncoder;
extern encoder_struct_t s_rightEncoder;

/************************************************************************/
/* Internal functions                                                   */
/************************************************************************/

void encoder_increment()
{
	switch(s_leftEncoder.A.port){
		case PA:
			s_leftEncoder.currentState = ((checkBit(PINA, s_leftEncoder.A.number)) << s_leftEncoder.A.number) | ((checkBit(PINA, s_leftEncoder.B.number)) << s_leftEncoder.B.number);
			break;
		case PB:
			s_leftEncoder.currentState = ((checkBit(PINB, s_leftEncoder.A.number)) << s_leftEncoder.A.number) | ((checkBit(PINB, s_leftEncoder.B.number)) << s_leftEncoder.B.number);
			break;
		case PC:
			s_leftEncoder.currentState = ((checkBit(PINC, s_leftEncoder.A.number)) << s_leftEncoder.A.number) | ((checkBit(PINC, s_leftEncoder.B.number)) << s_leftEncoder.B.number);
			break;
		case PD:
			s_leftEncoder.currentState = ((checkBit(PIND, s_leftEncoder.A.number)) << s_leftEncoder.A.number) | ((checkBit(PIND, s_leftEncoder.B.number)) << s_leftEncoder.B.number);
			break;
		default:
			break;
	}
	switch(s_rightEncoder.A.port){
		case PA:
			s_rightEncoder.currentState = ((checkBit(PINA, s_rightEncoder.A.number)) << s_rightEncoder.A.number) | ((checkBit(PINA, s_rightEncoder.B.number)) << s_rightEncoder.B.number);
			break;
		case PB:
			s_rightEncoder.currentState = ((checkBit(PINB, s_rightEncoder.A.number)) << s_rightEncoder.A.number) | ((checkBit(PINB, s_rightEncoder.B.number)) << s_rightEncoder.B.number);
			break;
		case PC:
			s_rightEncoder.currentState = ((checkBit(PINC, s_rightEncoder.A.number)) << s_rightEncoder.A.number) | ((checkBit(PINC, s_rightEncoder.B.number)) << s_rightEncoder.B.number);
			break;
		case PD:
			s_rightEncoder.currentState = ((checkBit(PIND, s_rightEncoder.A.number)) << s_rightEncoder.A.number) | ((checkBit(PIND, s_rightEncoder.B.number)) << s_rightEncoder.B.number);
			break;
		default:
			break;
	}
	if(((s_leftEncoder.lastState | s_rightEncoder.lastState) ^ (s_leftEncoder.currentState | s_rightEncoder.currentState)) == (1 << s_leftEncoder.A.number)){
		s_leftEncoder.counter++;
	}
	else if(((s_leftEncoder.lastState | s_rightEncoder.lastState) ^ (s_leftEncoder.currentState | s_rightEncoder.currentState)) == (1 << s_leftEncoder.B.number)){
		s_leftEncoder.counter++;
	}
	else if(((s_leftEncoder.lastState | s_rightEncoder.lastState) ^ (s_leftEncoder.currentState | s_rightEncoder.currentState)) == (1 << s_rightEncoder.A.number)){
		s_rightEncoder.counter++;
	}
	else if(((s_leftEncoder.lastState | s_rightEncoder.lastState) ^ (s_leftEncoder.currentState | s_rightEncoder.currentState)) == (1 << s_rightEncoder.B.number)){
		s_rightEncoder.counter++;
	}
	s_leftEncoder.lastState = s_leftEncoder.currentState;
	s_rightEncoder.lastState = s_rightEncoder.currentState;
}

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void encoder_init(encoder_struct_t *s_encoder){
	gpio_init(s_encoder->A);
	gpio_init(s_encoder->B);
	s_encoder->counter = 0;
	s_encoder->lastState = 0;
	s_encoder->currentState = 0;
	gpio_attachInterrupt(s_encoder->A, INTERRUPT_TOGGLE, encoder_increment);
	gpio_attachInterrupt(s_encoder->B, INTERRUPT_TOGGLE, encoder_increment);
}

void encoder_start(encoder_struct_t s_encoder){
	gpio_enableInterrupt(s_encoder.A, INTERRUPT_TOGGLE);
	gpio_enableInterrupt(s_encoder.B, INTERRUPT_TOGGLE);
}

void encoder_stop(encoder_struct_t s_encoder){
	gpio_disableInterrupt(s_encoder.A, INTERRUPT_TOGGLE);
	gpio_disableInterrupt(s_encoder.B, INTERRUPT_TOGGLE);
}

u32 encoder_getCounter(encoder_struct_t s_encoder){
	return s_encoder.counter;
}

void encoder_resetCounter(encoder_struct_t *s_encoder){
	s_encoder->counter = 0;
	s_encoder->currentState = 0;
	s_encoder->lastState = 0;
}

double encoder_getDistanceCm(encoder_struct_t s_encoder){
	return ((double) s_encoder.counter /(double)(GEAR_RATIO * 12)) * (double)(3.141592 * WHEEL_DIAMETER);
}