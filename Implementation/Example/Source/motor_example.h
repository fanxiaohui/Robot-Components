/*
 * motor_example.h
 *
 * Created: 19-Feb-18 8:36:32 PM
 *  Author: Adrian
 */ 

#include "motor.h"

#ifndef MOTOR_EXAMPLE_H_
#define MOTOR_EXAMPLE_H_

void motor_rotateLeft();
void motor_rotateRight();
void motor_moveForward();
void motor_moveBackward();
void motor_turnOn();
void motor_turnOff();
void motor_changeSpeed(u8 speed);
void motor_individualDirSpeed(motorDirection directionA, u8 speedA, motorDirection directionB, u8 speedB);

#endif /* MOTOR_EXAMPLE_H_ */