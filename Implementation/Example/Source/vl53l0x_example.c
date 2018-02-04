/**	@file		vl53l0x_example.c
	@brief		Example usage for VL53L0X distance sensor
	@details	See @link vl53l0x.h @endlink for details
*/

#include "debug.h"
#include "timer.h"
#include "vl53l0x.h"

#include <avr/interrupt.h>

timer_struct_t s_timeoutTimer;
vl53l0x_struct_t s_frontSensor;
volatile u32 u32_milliseconds = 0;

void incrementMillis()
{
	u32_milliseconds++;
}

void distanceSensor_init()
{
	s_timeoutTimer.frequency = 1000;
	s_timeoutTimer.peripheral = TIMER1;
	timer_init(s_timeoutTimer);
	timer_attachInterrupt(s_timeoutTimer, OVERFLOW, incrementMillis);
	timer_enableInterrupt(s_timeoutTimer, OVERFLOW);
	timer_start(s_timeoutTimer);
	
	sei();

	vl53l0x_init();
}

void distanceSensor_stop()
{
	vl53l0x_stop();
}


void distanceSensor_defaultTest()
{
	u16 distance;
	vl53l0x_startContinuous(30);

	while (1)
	{
		/* This can be put in a scheduler if no GPIO pin from the sensor is available */
		distance = vl53l0x_readRangeContinuousMillimeters();
		if (distance != 0xffff)
			debug_writeDecimal(distance);
	}
}

void distanceSensor_maxAccuracyTest()
{
	u16 distance;
	u16 rangePeriod = 200;
	vl53l0x_startContinuous(rangePeriod);
	vl53l0x_setSignalRateLimit(0.25);
	vl53l0x_setMeasurementTimingBudget(rangePeriod * 1000);

	while (1)
	{
		distance = vl53l0x_readRangeContinuousMillimeters();
		if (distance != 0xffff)
			debug_writeDecimal(distance);
	}
}

void distanceSensor_maxRangeTest()
{
	u16 distance;
	u16 rangePeriod = 33;
	vl53l0x_startContinuous(rangePeriod);
	vl53l0x_setSignalRateLimit(0.1);
	vl53l0x_setVcselPulsePeriod(VcselPeriodPreRange, 18);
	vl53l0x_setVcselPulsePeriod(VcselPeriodFinalRange, 14);
	vl53l0x_setMeasurementTimingBudget(rangePeriod * 1000);

	while (1)
	{
		distance = vl53l0x_readRangeContinuousMillimeters();
		if (distance != 0xffff)
			debug_writeDecimal(distance);
	}
}

void distanceSensor_maxSpeedTest()
{
	u16 distance;
	u16 rangePeriod = 20;
	vl53l0x_startContinuous(rangePeriod);
	vl53l0x_setSignalRateLimit(0.25);
	vl53l0x_setMeasurementTimingBudget(rangePeriod * 1000);

	while (1)
	{
		distance = vl53l0x_readRangeContinuousMillimeters();
		if (distance != 0xffff)
			debug_writeDecimal(distance);
	}
}

void distanceSensor_singleTest()
{
	u16 distance;

	while (1)
	{
		distance = vl53l0x_readRangeSingleMillimeters();
		debug_writeDecimal(distance);
	}
}

void distanceSensor_obstacleTest()
{
	u16 distance;
	u16 obstacleDistance = 100;

	vl53l0x_startContinuous(30);

	while (1)
	{
		distance = vl53l0x_readRangeContinuousMillimeters();
		if (distance != 0xffff)
		{
			if (distance < obstacleDistance)
				debug_writeString("Obstacle");
			else
				debug_writeString("Clear");
		}
	}
}
