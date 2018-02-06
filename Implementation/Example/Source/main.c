/** @file main.c
*/

#include "debug.h"
#include "device.h"
#include "vl53l0x_example.h"

int main()
{
	device_disableJTAG();
	debug_init();
	distanceSensor_init();

	distanceSensor_sleepTest();
	while(1)
	{
	}

	return 0;
}