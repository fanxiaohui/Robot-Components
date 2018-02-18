/**	@file		motor.c
	@brief		Motor features
	@author		Adrian Grosu
	@version	1.0
	@date		31.10.2017
	@details	See motor.h for details.
*/

/************************************************************************/
/* AVR includes                                                         */
/************************************************************************/


/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "motor.h"

/************************************************************************/
/* Internal functions                                                   */
/************************************************************************/

u8 calibrateSpeed(u8 speed){
	return 0.91*speed;
}

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void motor_init(motor_struct_t s_motor){
	gpio_init(s_motor.motorDirectionA);
	gpio_init(s_motor.motorDirectionB);
	gpio_init(s_motor.motorEnable);
	pwm_init(&s_motor.motorPWM);
}

void motor_start(motor_struct_t s_motor){
	gpio_out_reset(s_motor.motorDirectionA);
	gpio_out_reset(s_motor.motorDirectionB);
	gpio_out_set(s_motor.motorEnable);
	pwm_setDutyCycle(s_motor.motorPWM, CHANNEL_A, 0);
	pwm_setDutyCycle(s_motor.motorPWM, CHANNEL_B, 0);
	pwm_start(s_motor.motorPWM);
}

void motor_stop(motor_struct_t s_motor){
	pwm_stop(s_motor.motorPWM);
	gpio_out_reset(s_motor.motorEnable);
}

void motor_direction(motor_struct_t s_motor, motorDirection direction){
	switch(direction){
		case FORWARD:
			gpio_out_reset(s_motor.motorDirectionA);
			gpio_out_reset(s_motor.motorDirectionB);
			break;
		case BACKWARD:
			gpio_out_set(s_motor.motorDirectionA);
			gpio_out_set(s_motor.motorDirectionB);
			break;
		case LEFT:
			gpio_out_reset(s_motor.motorDirectionA);
			gpio_out_set(s_motor.motorDirectionB);
			break;
		case RIGHT:
			gpio_out_set(s_motor.motorDirectionA);
			gpio_out_reset(s_motor.motorDirectionB);
			break;
	}
}

void motor_speed(motor_struct_t s_motor, u8 speed){
	pwm_setDutyCycle(s_motor.motorPWM, CHANNEL_A, calibrateSpeed(speed));
	pwm_setDutyCycle(s_motor.motorPWM, CHANNEL_B, speed);
}

void motor_individualDirSpeed(motor_struct_t s_motor, motorDirection directionA, u8 speedA, motorDirection directionB, u8 speedB){
	switch(directionA){
		case FORWARD:
			gpio_out_reset(s_motor.motorDirectionA);
			break;
		case BACKWARD:
			gpio_out_set(s_motor.motorDirectionA);
			break;
		default:
			break;
	}
	pwm_setDutyCycle(s_motor.motorPWM, CHANNEL_A, calibrateSpeed(speedA));
	
	switch(directionB){
		case FORWARD:
			gpio_out_reset(s_motor.motorDirectionB);
			break;
		case BACKWARD:
			gpio_out_set(s_motor.motorDirectionB);
			break;
		default:
			break;
	}
	pwm_setDutyCycle(s_motor.motorPWM, CHANNEL_B, speedB);
}