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

motor_struct_t motors;
motor_struct_t rightMotor;

extern vl53l0x_struct_t s_frontSensor;
extern vl53l0x_struct_t s_leftSensor;
extern vl53l0x_struct_t s_rightSensor;

#include <util/delay.h>

int main()
{
	device_disableJTAG();
	debug_init();
	
	motors.motorEnable.port = PD;
	motors.motorEnable.number = 2;
	motors.motorDirectionA.port = PD;
	motors.motorDirectionA.number = 3;
	motors.motorDirectionB.port = PD;
	motors.motorDirectionB.number = 6;
	motors.motorPWM.base.frequency = 100000;
	motors.motorPWM.base.peripheral = TIMER1;
	motors.motorPWM.signalType = DUTY_CYCLE_VARIABLE;
	motors.motorPWM.mode = FAST_PWM;
	motors.motorPWM.channelA.enabled = TRUE;
	motors.motorPWM.channelA.invertedOutput = FALSE;
	motors.motorPWM.channelB.enabled = TRUE;
	motors.motorPWM.channelB.invertedOutput = FALSE;
	
	motor_init(motors);
	motor_start(motors);
	
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
		distance = vl53l0x_readRangeContinuous(&s_frontSensor);
		if (distance != 0xffff)
		{
			if (distance > 100)
				motor_speed(motors, 60);
			else
				motor_speed(motors, 0);
			/*else
			{
				distance = vl53l0x_readRangeContinuous(&s_rightSensor);
				if (distance != 0xffff && distance > 100)
				{
					motor_direction(motors, RIGHT);
					motor_speed(motors, 80);
					_delay_ms(500);
				}
				else
				{
					distance = vl53l0x_readRangeContinuous(&s_leftSensor);
					if (distance != 0xffff && distance > 100)
					{
						motor_direction(motors, LEFT);
						motor_speed(motors, 80);
						_delay_ms(500);
					}
					else
						motor_speed(motors, 0);
				}
			}*/
			debug_writeDecimal(distance);
			debug_writeNewLine();
		}
	}

	return 0;
}