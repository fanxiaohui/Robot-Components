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
#include "motor_config.h"

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/** Motor direction enumeration
*/
typedef enum{
	FORWARD,
	BACKWARD
}motorDirection;


/** Motor configuration structure
*/
typedef struct motor_struct_t{
#ifdef MOTOR_ENABLE
/** Enable pin of the driver */
	gpio_struct_t enable;
#endif
/** Direction pin for the motor */
	gpio_struct_t direction;
/** PWM signal to control the motor speed */
	pwm_struct_t PWM;
/** Timer channel for the PWM */
	timer_channel_enum_t channel;
}motor_struct_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Initializes the motor driver.
	@remark		Must be called before any other motor function
	@param[in]	s_motor: motor parameters to initialize
*/
void motor_init(motor_struct_t s_motor);

/** Starts the motor driver.
	@pre		Must be called after the motor driver was initialized (with @link motor_init @endlink).
	@param[in]	s_motor: motor peripheral to use
*/
void motor_start(motor_struct_t s_motor);

/** Stops motor driver.
	@pre		Must be called after the motor driver was initialized (with @link motor_init @endlink).
	@param[in]	s_motor: motor peripheral to use
*/
void motor_stop(motor_struct_t s_motor);

/** Changes the direction of the motor to get the desired physical movement.
	@pre		Must be called after the motor driver was initialized (with @link motor_init @endlink).
	@param[in]	s_motor: motor peripheral to use
	@param[in]	direction: direction of the motor
*/
void motor_direction(motor_struct_t s_motor, motorDirection direction);

/** Changes the speed of the motor.
	@pre		Must be called after the motor driver was initialized (with @link motor_init @endlink).
	@param[in]	s_motor: motor peripheral to use
	@param[in]	speed: desired speed
*/
void motor_speed(motor_struct_t s_motor, u8 speed);

/** Disables the motor driver 
*/
void motor_driverDisable(motor_struct_t s_motor);


#endif /* MOTOR_H_ */