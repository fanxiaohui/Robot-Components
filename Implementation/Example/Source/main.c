/** @file main.c
*/

#define F_CPU 8000000UL

#include "debug.h"
#include "device.h"
#include "vl53l0x_example.h"
#include "vl53l0x.h"
#include "surface_sensor_example.h"
#include "gpio.h"
#include "motor_example.h"
#include "motor.h"
#include "encoder.h"
#include "pid.h"

gpio_struct_t s_gpio;
gpio_struct_t s_gpio2;

encoder_struct_t s_encoderLeft;
encoder_struct_t s_encoderRight;

pid_struct_t distancePID;

extern vl53l0x_struct_t s_frontSensor;
extern vl53l0x_struct_t s_leftSensor;
extern vl53l0x_struct_t s_rightSensor;

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

int main()
{
	device_disableJTAG();
	debug_init();
	sei();
	s_encoderLeft.A.direction = INPUT;
	s_encoderLeft.A.number = 4;
	s_encoderLeft.A.port = PC;
	s_encoderLeft.A.pullUp = NO_PULL;
	s_encoderLeft.B.direction = INPUT;
	s_encoderLeft.B.number = 5;
	s_encoderLeft.B.port = PC;
	s_encoderLeft.B.pullUp = NO_PULL;
	
	s_encoderRight.A.direction = INPUT;
	s_encoderRight.A.number = 6;
	s_encoderRight.A.port = PC;
	s_encoderRight.A.pullUp = NO_PULL;
	s_encoderRight.B.direction = INPUT;
	s_encoderRight.B.number = 7;
	s_encoderRight.B.port = PC;
	s_encoderRight.B.pullUp = NO_PULL;
	
	encoder_init(&s_encoderLeft);
	encoder_init(&s_encoderRight);
	encoder_start(s_encoderLeft);
	encoder_start(s_encoderRight);
	encoder_resetCounter(&s_encoderLeft);
	
	distancePID.Kd = 0;
	distancePID.Ki = 0;
	distancePID.Kp = 1;
	pid_init(distancePID);

	motor_turnOn();
	motor_moveForward();
	for(int i = 60; i <= 70; i += 1){
		motor_changeSpeed(i);
		_delay_ms(50);
	}
	distanceSensor_multiInit();
	
	
	
	u16 distance;
	s8 correction;
	
	vl53l0x_start(&s_leftSensor);
	vl53l0x_setAddress(&s_leftSensor, s_leftSensor.address + 1);
	vl53l0x_setMode(&s_leftSensor, VL53L0X_DEFAULT);
	vl53l0x_startContinuous(&s_leftSensor, 0);
	
	vl53l0x_start(&s_rightSensor);
	vl53l0x_setAddress(&s_rightSensor, s_rightSensor.address + 2);
	vl53l0x_setMode(&s_rightSensor, VL53L0X_DEFAULT);
	vl53l0x_startContinuous(&s_rightSensor, 0);
	
	vl53l0x_start(&s_frontSensor);
	vl53l0x_setMode(&s_frontSensor, VL53L0X_DEFAULT);
	vl53l0x_startContinuous(&s_frontSensor, 0);

	//distanceSensor_multiDefaultTest();
	
	while(1)
	{
		//debug_writeDecimal((u16)encoder_getDistanceCm(s_encoderLeft));
		//debug_writeNewLine();
		/*_delay_ms(100);
		motor_moveBackward();
		encoder_resetCounter(&s_encoderLeft);
		while(encoder_getDistanceCm(s_encoderLeft) < 300);
		motor_moveForward();
		encoder_resetCounter(&s_encoderLeft);
		while(encoder_getDistanceCm(s_encoderLeft) < 300);*/
		
		/*_delay_ms(1000);
		motor_rotateLeft();
		_delay_ms(1000);
		motor_rotateRight();
		_delay_ms(1000);
		motor_moveForward();*/
		distance = vl53l0x_readRangeContinuous(&s_frontSensor);
		if (distance != 0xffff)
		{
			distancePID.currentError = (float)((float)distance - 100.0);
			if(distancePID.currentError > 2000){
				distancePID.currentError = 2000.0;
			}
			correction = (u8)pid_getCorrection(distancePID);
			if(correction > 60){
				correction = 60;
			}
			if(correction < 0){
				correction = 0;
			}
			motor_changeSpeed(correction);
			/*if (distance > 100)
			{
				motor_changeSpeed(50);
			}
			else
			{
				motor_changeSpeed(0);
			}*/
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
			//debug_writeDecimal(distance);
			//debug_writeNewLine();
		}
	}

	return 0;
}