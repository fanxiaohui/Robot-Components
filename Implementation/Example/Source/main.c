/** @file main.c
*/

#define F_CPU 8000000UL

#include "debug.h"
#include "device.h"
#include "vl53l0x_example.h"

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

int main()
{
	device_disableJTAG();
	debug_init();


	distanceSensor_multiInit();
	distanceSensor_multiDefaultTest();
	
	return 0;
}