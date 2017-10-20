/*
 * motor.c
 *
 * Created: 20-Oct-17 5:56:20 PM
 *  Author: Adrian
 */ 

#include <motor.h>

gpio_struct_t motorEnable;
gpio_struct_t motorDirectionA;
gpio_struct_t motorDirectionB;
timer_struct_t motorTimer;
pwm_struct_t motorPWM;

void calibrateSpeed(){
	
}


void motor_init(){
	motorEnable.direction = OUTPUT;
	motorEnable.port = PD;
	motorEnable.number = 2;
	motorEnable.pullUp = NO_PULL;
	
	motorDirectionA.direction = OUTPUT;
	motorDirectionA.port = PD;
	motorDirectionA.number = 3;
	motorDirectionA.pullUp = NO_PULL;
	
	motorDirectionB.direction = OUTPUT;
	motorDirectionB.port = PD;
	motorDirectionB.number = 6;
	motorDirectionB.pullUp = NO_PULL;
	
	motorTimer.frequency = 20000;
	motorTimer.peripheral = TIMER1;
	
	motorPWM.base = motorTimer;
	motorPWM.signalType = DUTY_CYCLE_VARIABLE;
	motorPWM.mode = FAST_PWM;
	motorPWM.channelA.enabled = TRUE;
	motorPWM.channelA.invertedOutput = FALSE;
	motorPWM.channelB.enabled = TRUE;
	motorPWM.channelB.invertedOutput = FALSE;
	
	gpio_init(motorEnable);
	gpio_init(motorDirectionA);
	gpio_init(motorDirectionB);
	timer_init(motorTimer);
	pwm_init(&motorPWM);
}

void motor_start(){
	gpio_out_reset(motorDirectionA);
	gpio_out_reset(motorDirectionB);
	gpio_out_set(motorEnable);
	
	pwm_start(motorPWM);
	for(u8 i = 0; i <= 30; i += 10){
		pwm_setDutyCycle(motorPWM, CHANNEL_A, i);
		pwm_setDutyCycle(motorPWM, CHANNEL_B, i);
	}
	
}

void motor_stop(){
	pwm_stop(motorPWM);
	gpio_out_reset(motorEnable);
}

void motor_direction(motorDirection direction){
	switch(direction){
		case FORWARD:
			gpio_out_reset(motorDirectionA);
			gpio_out_reset(motorDirectionB);
			break;
		case BACKWARD:
			gpio_out_set(motorDirectionA);
			gpio_out_set(motorDirectionB);
			break;
		case LEFT:
			gpio_out_reset(motorDirectionA);
			gpio_out_set(motorDirectionB);
			break;
		case RIGHT:
			gpio_out_set(motorDirectionA);
			gpio_out_reset(motorDirectionB);
			break;
	}
}

void motor_speed(u8 speed){
	pwm_setDutyCycle(motorPWM, CHANNEL_A, speed);
	pwm_setDutyCycle(motorPWM, CHANNEL_B, speed);
}

void motor_individualDirSpeed(motorDirection directionA, u8 speedA, motorDirection directionB, u8 speedB){
	
}