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

/************************************************************************/
/* Internal variables                                                   */
/************************************************************************/

/************************************************************************/
/* Internal functions                                                   */
/************************************************************************/

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void encoder_init(encoder_struct_t *s_encoder, void (*p_function)(void)){
	gpio_init(s_encoder->A);
	gpio_init(s_encoder->B);
	s_encoder->counter = 0;
	s_encoder->lastState = 0;
	s_encoder->currentState = 0;
	gpio_attachInterrupt(s_encoder->A, INTERRUPT_TOGGLE, p_function);
	gpio_attachInterrupt(s_encoder->B, INTERRUPT_TOGGLE, p_function);
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