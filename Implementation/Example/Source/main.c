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

gpio_struct_t s_gpio;
gpio_struct_t s_gpio2;

extern vl53l0x_struct_t s_frontSensor;
extern vl53l0x_struct_t s_leftSensor;
extern vl53l0x_struct_t s_rightSensor;

#include <util/delay.h>

int main()
{
	device_disableJTAG();
	debug_init();
	
	distanceSensor_multiInit();
	
	u16 distance;/*
	vl53l0x_start(&s_leftSensor);
	vl53l0x_setAddress(&s_leftSensor, s_leftSensor.address + 1);
	vl53l0x_setMode(&s_leftSensor, VL53L0X_DEFAULT);
	vl53l0x_startContinuous(&s_leftSensor, 0);
	
	vl53l0x_start(&s_rightSensor);
	vl53l0x_setAddress(&s_rightSensor, s_rightSensor.address + 2);
	vl53l0x_setMode(&s_rightSensor, VL53L0X_DEFAULT);
	vl53l0x_startContinuous(&s_rightSensor, 0);*/
	
	vl53l0x_start(&s_frontSensor);
	vl53l0x_setMode(&s_frontSensor, VL53L0X_DEFAULT);
	vl53l0x_startContinuous(&s_frontSensor, 0);

	//distanceSensor_multiDefaultTest();
	
	while(1)
	{
		//surfaceSensor_read();
	}

	return 0;
}