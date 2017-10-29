/*
 * encoder.c
 *
 * Created: 10/21/2017 7:22:43 PM
 *  Author: Adrian
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "encoder.h"

volatile u8 counter;
volatile u32 counterRight;
volatile u32 counterLeft;
volatile u8 lastState;
volatile u8 currentState;
gpio_struct_t encoderLA;
gpio_struct_t encoderLB;
gpio_struct_t encoderRA;
gpio_struct_t encoderRB;
uart_struct_t s_debugUart;

void p_pcInt2Callback()
{
	currentState = PINC & 0xF0;
	switch (lastState ^ currentState)
	{
		case 0x10:
			counterRight++;
			break;
		case 0x20:
			counterRight++;
			break;
		case 0x40:
			counterLeft++;
			break;
		case 0x80:
			counterLeft++;
			break;
		default:
				break;
	}
	lastState = currentState;
}

void encoder_init(){
	counter = 0;
	counterLeft = 0;
	counterRight = 0;
	lastState = 0x00;
	currentState = 0x00;
	encoderLA.direction = INPUT;
	encoderLA.port = PC;
	encoderLA.number = 6;
	encoderLA.pullUp = NO_PULL;
	
	encoderLB.direction = INPUT;
	encoderLB.port = PC;
	encoderLB.number = 7;
	encoderLB.pullUp = NO_PULL;
	
	encoderRA.direction = INPUT;
	encoderRA.port = PC;
	encoderRA.number = 4;
	encoderRA.pullUp = NO_PULL;
	
	encoderRB.direction = INPUT;
	encoderRB.port = PC;
	encoderRB.number = 5;
	encoderRB.pullUp = NO_PULL;
	
	gpio_init(encoderLA);
	gpio_init(encoderLB);
	gpio_init(encoderRA);
	gpio_init(encoderRB);
	gpio_attachInterrupt(encoderLA, INTERRUPT_TOGGLE, p_pcInt2Callback);
	gpio_attachInterrupt(encoderLB, INTERRUPT_TOGGLE, p_pcInt2Callback);
	gpio_attachInterrupt(encoderRA, INTERRUPT_TOGGLE, p_pcInt2Callback);
	gpio_attachInterrupt(encoderRB, INTERRUPT_TOGGLE, p_pcInt2Callback);
}

void encoder_start(){
	gpio_enableInterrupt(encoderLA, INTERRUPT_TOGGLE);
	gpio_enableInterrupt(encoderLB, INTERRUPT_TOGGLE);
	gpio_enableInterrupt(encoderRA, INTERRUPT_TOGGLE);
	gpio_enableInterrupt(encoderRB, INTERRUPT_TOGGLE);
}

void encoder_stop(){
	gpio_disableInterrupt(encoderLA, INTERRUPT_TOGGLE);
	gpio_disableInterrupt(encoderLB, INTERRUPT_TOGGLE);
	gpio_disableInterrupt(encoderRA, INTERRUPT_TOGGLE);
	gpio_disableInterrupt(encoderRB, INTERRUPT_TOGGLE);
}

u32 encoder_getRight(){
	return counterRight;
}

u32 encoder_getLeft(){
	return counterLeft;
}

void encoder_reset(){
	counterLeft = 0;
	counterRight = 0;
	lastState = 0xF0;
}