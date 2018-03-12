/** @file main.c
*/

#define F_CPU 8000000UL

#include "debug.h"
#include "device.h"
#include "button_example.h"

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

int main()
{
	device_disableJTAG();
	debug_init();
	testButton_init();
	
	sei();

	debug_writeHex(PCMSK3);

	while(1);

	return 0;
}