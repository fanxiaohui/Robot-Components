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

extern encoder_struct_t s_encoderLeft;
extern encoder_struct_t s_encoderRight;

/************************************************************************/
/* Internal functions                                                   */
/************************************************************************/

void encoder_increment()
{
	switch(s_encoderLeft.A.port){
		case PA:
			s_encoderLeft.currentState = ((checkBit(PINA, s_encoderLeft.A.number)) << s_encoderLeft.A.number) | ((checkBit(PINA, s_encoderLeft.B.number)) << s_encoderLeft.B.number);
			break;
		case PB:
			s_encoderLeft.currentState = ((checkBit(PINB, s_encoderLeft.A.number)) << s_encoderLeft.A.number) | ((checkBit(PINB, s_encoderLeft.B.number)) << s_encoderLeft.B.number);
			break;
		case PC:
			s_encoderLeft.currentState = ((checkBit(PINC, s_encoderLeft.A.number)) << s_encoderLeft.A.number) | ((checkBit(PINC, s_encoderLeft.B.number)) << s_encoderLeft.B.number);
			break;
		case PD:
			s_encoderLeft.currentState = ((checkBit(PIND, s_encoderLeft.A.number)) << s_encoderLeft.A.number) | ((checkBit(PIND, s_encoderLeft.B.number)) << s_encoderLeft.B.number);
			break;
		default:
			break;
	}
	switch(s_encoderRight.A.port){
		case PA:
			s_encoderRight.currentState = ((checkBit(PINA, s_encoderRight.A.number)) << s_encoderRight.A.number) | ((checkBit(PINA, s_encoderRight.B.number)) << s_encoderRight.B.number);
			break;
		case PB:
			s_encoderRight.currentState = ((checkBit(PINB, s_encoderRight.A.number)) << s_encoderRight.A.number) | ((checkBit(PINB, s_encoderRight.B.number)) << s_encoderRight.B.number);
			break;
		case PC:
			s_encoderRight.currentState = ((checkBit(PINC, s_encoderRight.A.number)) << s_encoderRight.A.number) | ((checkBit(PINC, s_encoderRight.B.number)) << s_encoderRight.B.number);
			break;
		case PD:
			s_encoderRight.currentState = ((checkBit(PIND, s_encoderRight.A.number)) << s_encoderRight.A.number) | ((checkBit(PIND, s_encoderRight.B.number)) << s_encoderRight.B.number);
			break;
		default:
			break;
	}
	if(((s_encoderLeft.lastState | s_encoderRight.lastState) ^ (s_encoderLeft.currentState | s_encoderRight.currentState)) == (1 << s_encoderLeft.A.number)){
		s_encoderLeft.counter++;
	}
	else if(((s_encoderLeft.lastState | s_encoderRight.lastState) ^ (s_encoderLeft.currentState | s_encoderRight.currentState)) == (1 << s_encoderLeft.B.number)){
		s_encoderLeft.counter++;
	}
	else if(((s_encoderLeft.lastState | s_encoderRight.lastState) ^ (s_encoderLeft.currentState | s_encoderRight.currentState)) == (1 << s_encoderRight.A.number)){
		s_encoderRight.counter++;
	}
	else if(((s_encoderLeft.lastState | s_encoderRight.lastState) ^ (s_encoderLeft.currentState | s_encoderRight.currentState)) == (1 << s_encoderRight.B.number)){
		s_encoderRight.counter++;
	}
	s_encoderLeft.lastState = s_encoderLeft.currentState;
	s_encoderRight.lastState = s_encoderRight.currentState;
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