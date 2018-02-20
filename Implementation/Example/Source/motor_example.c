/*
 * motor_example.c
 *
 * Created: 19-Feb-18 8:36:43 PM
 *  Author: Adrian
 */ 

#include "debug.h"
#include "motor_example.h"
#include <util/delay.h>

motor_struct_t motorLeft;
motor_struct_t motorRight;

void motor_rotateLeft(){
	gpio_out_set(motorLeft.direction);
	gpio_out_reset(motorRight.direction);
}
void motor_rotateRight(){
	gpio_out_reset(motorLeft.direction);
	gpio_out_set(motorRight.direction);
}
void motor_moveForward(){
	gpio_out_reset(motorLeft.direction);
	gpio_out_reset(motorRight.direction);
}
void motor_moveBackward(){
	gpio_out_set(motorLeft.direction);
	gpio_out_set(motorRight.direction);
}
void motor_turnOn(){
	motorLeft.channel = CHANNEL_A;
	motorLeft.direction.direction = OUTPUT;
	motorLeft.direction.number = 6;
	motorLeft.direction.port = PD;
	motorLeft.direction.pullUp = NO_PULL;
	motorLeft.enable.direction = OUTPUT;
	motorLeft.enable.number = 2;
	motorLeft.enable.port = PD;
	motorLeft.enable.pullUp = NO_PULL;
	motorLeft.PWM.base.peripheral = TIMER1;
	motorLeft.PWM.base.frequency = 100000;
	motorLeft.PWM.channelA.enabled = TRUE;
	motorLeft.PWM.channelA.invertedOutput = FALSE;
	motorLeft.PWM.mode = FAST_PWM;
	motorLeft.PWM.signalType = DUTY_CYCLE_VARIABLE;
	
	motorRight.channel = CHANNEL_B;
	motorRight.direction.direction = OUTPUT;
	motorRight.direction.number = 3;
	motorRight.direction.port = PD;
	motorRight.direction.pullUp = NO_PULL;
	motorRight.enable.direction = OUTPUT;
	motorRight.enable.number = 2;
	motorRight.enable.port = PD;
	motorRight.enable.pullUp = NO_PULL;
	motorRight.PWM.base.peripheral = TIMER1;
	motorRight.PWM.base.frequency = 100000;
	motorRight.PWM.channelB.enabled = TRUE;
	motorRight.PWM.channelB.invertedOutput = FALSE;
	motorRight.PWM.mode = FAST_PWM;
	motorRight.PWM.signalType = DUTY_CYCLE_VARIABLE;
	
	motor_init(motorLeft);
	motor_init(motorRight);
	motor_start(motorLeft);
	motor_start(motorRight);
	motor_speed(motorLeft, 60);
	motor_speed(motorRight, 60);
}

void motor_turnOff(){
	motor_stop(motorLeft);
	motor_stop(motorRight);
	motor_driverDisable(motorLeft);
	motor_driverDisable(motorRight);
}

void motor_changeSpeed(u8 speed){
	pwm_setDutyCycle(motorLeft.PWM, motorLeft.channel, speed);
	pwm_setDutyCycle(motorRight.PWM, motorRight.channel, speed);
}

void motor_individualDirSpeed(motorDirection directionA, u8 speedA, motorDirection directionB, u8 speedB){
	switch(directionA){
		case FORWARD:
			gpio_out_reset(motorLeft.direction);
			break;
		case BACKWARD:
			gpio_out_set(motorRight.direction);
			break;
		default:
			break;
	}
	pwm_setDutyCycle(motorLeft.PWM, motorLeft.channel, motor_calibrateSpeed(speedA));
	
	switch(directionB){
		case FORWARD:
			gpio_out_reset(motorRight.direction);
			break;
		case BACKWARD:
			gpio_out_set(motorRight.direction);
			break;
		default:
			break;
	}
	pwm_setDutyCycle(motorRight.PWM, motorRight.channel, speedB);
}