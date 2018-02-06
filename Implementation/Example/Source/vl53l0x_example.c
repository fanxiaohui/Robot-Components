/**	@file		vl53l0x_example.c
	@brief		Example usage for VL53L0X distance sensor
	@details	See @link vl53l0x.h @endlink for details
*/

#include "debug.h"
#include "timer.h"
#include "vl53l0x.h"
#include <util/delay.h>

#include <avr/interrupt.h>

timer_struct_t s_timeoutTimer;
vl53l0x_struct_t s_frontSensor;

void distanceSensor_init()
{
	s_timeoutTimer.frequency = 1000;
	s_timeoutTimer.peripheral = TIMER1;
	timer_init(s_timeoutTimer);
	timer_attachInterrupt(s_timeoutTimer, OVERFLOW, vl53l0x_incrementTimeoutCounter);
	timer_enableInterrupt(s_timeoutTimer, OVERFLOW);
	timer_start(s_timeoutTimer);

	s_frontSensor.address = VL53L0X_ADDRESS_DEFAULT;
	s_frontSensor.i2cTimeout = 100;
	s_frontSensor.xshutPin.port = PC;
	s_frontSensor.xshutPin.number = 2;

	sei();

	vl53l0x_init(&s_frontSensor);
}
void distanceSensor_defaultTest()
{
	u16 distance;
	vl53l0x_start(&s_frontSensor);
	vl53l0x_setMode(&s_frontSensor, VL53L0X_DEFAULT);
	vl53l0x_startContinuous(&s_frontSensor, 0);

	while (1)
	{
		/* This can be put in a scheduler if no GPIO pin from the sensor is available */
		distance = vl53l0x_readRangeContinuous(&s_frontSensor);
		if (distance != 0xffff)
			debug_writeDecimal(distance);
	}
}

void distanceSensor_sleepTest()
{
	u8 i = 0;
	u16 distance;

	vl53l0x_start(&s_frontSensor);
	vl53l0x_startContinuous(&s_frontSensor, 30);

	while (1)
	{
		while (i < 100)
		{
			distance = vl53l0x_readRangeContinuous(&s_frontSensor);
			if (distance != 0xffff)
			{
				debug_writeDecimal(distance);
				i++;
			}
		}
		vl53l0x_stop(&s_frontSensor);
		_delay_ms(3000);
		i = 0;
		vl53l0x_start(&s_frontSensor);
		vl53l0x_startContinuous(&s_frontSensor, 30);
	}
}

void distanceSensor_maxAccuracyTest()
{
	u16 distance;
	u16 rangePeriod = 200;

	vl53l0x_start(&s_frontSensor);
	vl53l0x_setMode(&s_frontSensor, VL53L0X_MAX_ACCURACY);
	vl53l0x_startContinuous(&s_frontSensor, rangePeriod);

	while (1)
	{
		distance = vl53l0x_readRangeContinuous(&s_frontSensor);
		if (distance != 0xffff)
			debug_writeDecimal(distance);
	}
}

void distanceSensor_maxRangeTest()
{
	u16 distance;
	u16 rangePeriod = 33;

	vl53l0x_start(&s_frontSensor);
	vl53l0x_setMode(&s_frontSensor, VL53L0X_MAX_RANGE);
	vl53l0x_startContinuous(&s_frontSensor, rangePeriod);

	while (1)
	{
		distance = vl53l0x_readRangeContinuous(&s_frontSensor);
		if (distance != 0xffff)
			debug_writeDecimal(distance);
	}
}

void distanceSensor_maxSpeedTest()
{
	u16 distance;
	u16 rangePeriod = 20;

	vl53l0x_start(&s_frontSensor);
	vl53l0x_setMode(&s_frontSensor, VL53L0X_MAX_SPEED);
	vl53l0x_startContinuous(&s_frontSensor, rangePeriod);

	while (1)
	{
		distance = vl53l0x_readRangeContinuous(&s_frontSensor);
		if (distance != 0xffff)
			debug_writeDecimal(distance);
	}
}

void distanceSensor_singleTest()
{
	u16 distance;

	vl53l0x_start(&s_frontSensor);

	while (1)
	{
		distance = vl53l0x_readRangeSingle(&s_frontSensor);
		debug_writeDecimal(distance);
	}
}

void distanceSensor_obstacleTest()
{
	u16 distance;
	u16 obstacleDistance = 100;

	vl53l0x_start(&s_frontSensor);
	vl53l0x_startContinuous(&s_frontSensor, 30);

	while (1)
	{
		distance = vl53l0x_readRangeContinuous(&s_frontSensor);
		if (distance != 0xffff)
		{
			if (distance < obstacleDistance)
				debug_writeString("Obstacle");
			else
				debug_writeString("Clear");
		}
	}
}
