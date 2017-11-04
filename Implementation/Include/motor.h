/**	@file		motor.h
	@brief		Motor features
	@author		Adrian Grosu
	@version	1.0
	@date		31.10.2017
	@details	Motor driver that supports 2 motors which are controlled with a digital output and a PWM signal for each motor
				1. Initialize a @link motor_struct_t @endlink. You do not need to initialize the structure members beforehand, you only have to declare them and set their values.
				2. Pass it to @link motor_init @endlink.
				3. Call @link motor_start @endlink.
				4. Call @link motor_speed @endlink when needed to change the speed.
				5. Call @link motor_direction @endlink when needed to change the direction.
				- You can call @link motor_individualDirSpeed @endlink to adjust the speed and direction for each of the motors individually
				- You can stop the motors by calling @link motor_stop @endlink.
	@remark		@link motor_individualDirSpeed @endlink only supports FORWARD and BACKWARD values for @link motorDirection @endlink.
*/


#ifndef MOTOR_H_
#define MOTOR_H_

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "pwm.h"
#include "timer.h"
#include "gpio.h"

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/** Motor direction enumeration
*/
typedef enum{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
}motorDirection;


/** Motor configuration structure
*/
typedef struct motor_struct_t{
/** Enable pin of the driver */
	gpio_struct_t motorEnable;
/** Direction pin for the left motor */
	gpio_struct_t motorDirectionA;
/** Direction pin for the right motor */
	gpio_struct_t motorDirectionB;
/** Timer peripheral that specifies the frequency of the PWM signal used*/
	timer_struct_t motorTimer;
/** PWM signal (2 channels) to control the motor speed */
	pwm_struct_t motorPWM;
}motor_struct_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Initializes motor driver.
	@remark		Must be called before any other motor function
	@param[in]	s_motor: motor parameters to initialize
*/
void motor_init(motor_struct_t s_motor);

/** Starts motor driver.
	@pre		Must be called after the motor driver was initialized (with @link motor_init @endlink).
	@param[in]	s_motor: motor parameters to initialize
*/
 


void motor_start(motor_struct_t s_motor);

/** Stops motor driver.
	@pre		Must be called after the motor driver was initialized (with @link motor_init @endlink).
	@param[in]	s_motor: motor peripheral to use
*/
void motor_stop(motor_struct_t s_motor);

/** Changes the direction of the 2 motors to get the desired physical movement.
	@pre		Must be called after the motor driver was initialized (with @link motor_init @endlink).
	@param[in]	s_motor: motor peripheral to use
	@param[in]	direction: direction of the ensemble
*/
void motor_direction(motor_struct_t s_motor, motorDirection direction);

/** Changes the speed of the 2 motors.
	@pre		Must be called after the motor driver was initialized (with @link motor_init @endlink).
	@param[in]	s_motor: motor peripheral to use
	@param[in]	speed: desired speed
*/
void motor_speed(motor_struct_t s_motor, u8 speed);

/** Changes the direction of the 2 motors individually.
	@pre		Must be called after the motor driver was initialized (with @link motor_init @endlink).
	@param[in]	s_motor: motor peripheral to use
	@param[in]	directionA: direction of the left motor
	@param[in]	speedA: speed of the left motor
	@param[in]	directionB: direction of the right motor
	@param[in]	speedA: speed of the right motor
*/
void motor_individualDirSpeed(motor_struct_t s_motor, motorDirection directionA, u8 speedA, motorDirection directionB, u8 speedB);


#endif /* MOTOR_H_ */