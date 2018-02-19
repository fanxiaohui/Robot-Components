/** @file main.c
*/

#define F_CPU 8000000UL

#include "debug.h"
#include "device.h"
#include "vl53l0x_example.h"
#include "vl53l0x.h"
#include "surface_sensor_example.h"
#include "gpio.h"
#include "motor.h"
#include "adc.h"
#include <util/delay.h>

int main()
{
	device_disableJTAG();
	debug_init();
	


	surfaceSensor_init();	
		
	
	while(1)
	{

		surfaceSensor_isBlack();
		surfaceSensor_isWhite();
		/*surfaceSensor_read();*/
		surfaceSensor_stop();
		_delay_ms(100);
	}

	return 0;
}