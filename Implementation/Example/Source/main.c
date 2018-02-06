/** @file main.c
*/

#include "device.h"
#include "motor.h"
#include "encoder.h"
#include "i2c.h"
#include "adc.h"
#include "surface_sensor.h"
#include "debug.h"
#include "vl53l0x_example.h"

surface_struct_t sensor;
motor_struct_t motor;

int main()
{
	device_disableJTAG();
	debug_init();
	distanceSensor_init();

	distanceSensor_maxSpeedTest();
	while(1)
	{
	}

	return 0;
}