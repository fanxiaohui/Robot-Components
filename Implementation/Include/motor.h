/*
 * motor.h
 *
 * Created: 20-Oct-17 5:56:12 PM
 *  Author: Adrian
 */ 

#include "pwm.h"
#include "timer.h"
#include "gpio.h"

typedef enum{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
}motorDirection;


#ifndef MOTOR_H_
#define MOTOR_H_

void motor_init();

void motor_start();

void motor_stop();

void motor_direction(motorDirection direction);

void motor_speed(u8 speed);

void motor_individualDirSpeed(motorDirection directionA, u8 speedA, motorDirection directionB, u8 speedB);


#endif /* MOTOR_H_ */