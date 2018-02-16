/** @file main.c
*/

#define F_CPU 8000000UL

#include "debug.h"
#include "device.h"
#include "vl53l0x_example.h"
#include "surface_sensor_example.h"
#include "gpio.h"

gpio_struct_t s_gpio;
gpio_struct_t s_gpio2;

#include <util/delay.h>

int main()
{
	device_disableJTAG();
	debug_init();
	//surfaceSensor_init();
	s_gpio.direction = OUTPUT;
	s_gpio.number = 0;
	s_gpio.port = PB;
	s_gpio.pullUp = NO_PULL;
	s_gpio2.direction = OUTPUT;
	s_gpio2.number = 1;
	s_gpio2.port = PB;
	s_gpio2.pullUp = NO_PULL;
	gpio_out_set(s_gpio);
	gpio_out_set(s_gpio2);
	while(1)
	{
		_delay_ms(1000);
		//surfaceSensor_read();
	}

	return 0;
}