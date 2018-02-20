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
#include "motor_config.h"

/************************************************************************/
/* Internal functions                                                   */
/************************************************************************/


/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void motor_init(motor_struct_t s_motor){
	gpio_init(s_motor.direction);
	gpio_init(s_motor.enable);
	pwm_init(&s_motor.PWM);
}

void motor_start(motor_struct_t s_motor){
	gpio_out_set(s_motor.direction);
	gpio_out_set(s_motor.enable);
	pwm_setDutyCycle(s_motor.PWM, s_motor.channel , 0);
	pwm_start(s_motor.PWM);
}

void motor_stop(motor_struct_t s_motor){
	pwm_setDutyCycle(s_motor.PWM, s_motor.channel, 0);
}

void motor_driverDisable(motor_struct_t s_motor){
	gpio_out_reset(s_motor.enable);
	gpio_out_reset(s_motor.direction);
	pwm_stop(s_motor.PWM);
}

void motor_direction(motor_struct_t s_motor, motorDirection direction){
	switch(direction){
		case FORWARD:
			gpio_out_reset(s_motor.direction);
			break;
		case BACKWARD:
			gpio_out_set(s_motor.direction);
			break;
		default:
			break;
	}
}

void motor_speed(motor_struct_t s_motor, u8 speed){
	pwm_setDutyCycle(s_motor.PWM, s_motor.channel, speed);
}

u8 motor_calibrateSpeed(u8 speed){
	return (u8)CALIBRATION_COEFFICIENT*speed;
}