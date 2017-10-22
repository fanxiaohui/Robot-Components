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

void p_pcInt2Callback(){
	/*currentState = PINC & 0xF0;
	if(((currentState & 0x20) == 0) && ((lastState & 0x20) == 1)){
		counterRight++;
		lastState &= ~(0x20);
	}
	else{
		if((lastState & 0x20) == 0){
			lastState |= 0x20;
		}
	}
	
	if(((currentState & 0x10) == 0) && ((lastState & 0x10) == 1)){
		counterRight++;
		lastState &= ~(0x10);
	}
	else{
		if((lastState & 0x10) == 0){
			lastState |= 0x10;
		}
	if(((currentState & 0x40) == 0) && ((lastState & 0x40) == 1)){
		counterLeft++;
		lastState &= ~(0x40);
	}
	else{
		if((lastState & 0x40) == 0){
			lastState |= 0x40;
		}
	}
	
	if(((currentState & 0x80) == 0) && ((lastState & 0x80) == 1)){
		counterLeft++;
		lastState &= ~(0x80);
	}
	else{
		if((lastState & 0x80) == 0){
			lastState |= 0x80;
		}
	}*/
	uart_transmit(s_debugUart, 'x');
	uart_transmit(s_debugUart, ' ');
	uart_transmit(s_debugUart, counter);
	uart_transmit(s_debugUart, '\n');
	counter++;
}

void encoder_init(){
	counter = 0;
	counterLeft = 0;
	counterRight = 0;
	lastState = 0xF0;
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
	
	s_debugUart.peripheral = UART0;
	s_debugUart.baudRate = _9600;
	s_debugUart.frameSize = _8BIT;
	s_debugUart.parityBit = NONE;
	s_debugUart.stopBits = _1BIT;
	s_debugUart.useRx = FALSE;
	s_debugUart.useTx = TRUE;

	uart_init(s_debugUart);
	uart_start(s_debugUart);
}

void encoder_stop(){
	gpio_disableInterrupt(encoderLA, INTERRUPT_TOGGLE);
	gpio_disableInterrupt(encoderLB, INTERRUPT_TOGGLE);
	gpio_disableInterrupt(encoderRA, INTERRUPT_TOGGLE);
	gpio_disableInterrupt(encoderRB, INTERRUPT_TOGGLE);
}

u8 encoder_get(){
	return counter;
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