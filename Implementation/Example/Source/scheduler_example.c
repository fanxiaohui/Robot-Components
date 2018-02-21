/*
 * scheduler_example.c
 *
 * Created: 2/21/2018 7:28:12 PM
 *  Author: Phoenix Ars Technica
 */ 

#include "debug.h"
#include "scheduler.h"
#include "motor.h"
#include <avr/interrupt.h>

motor_struct_t motorLeft;
motor_struct_t motorRight;
bool leftStarted;
bool rightStarted;

void task_100ms()
{
	debug_writeChar('a');
}

void task_1s()
{
	if (!leftStarted)
	{
		motor_speed(motorLeft, 60);
		leftStarted = TRUE;
	}
	else
	{
		motor_speed(motorLeft, 0);
		leftStarted = FALSE;
	}
}

void task_2s()
{
	if (!rightStarted)
	{
		motor_speed(motorRight, 60);
		rightStarted = TRUE;
	}
	else
	{
		motor_speed(motorRight, 0);
		rightStarted = FALSE;
	}
}

void init()
{
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
	
	leftStarted = FALSE;
	rightStarted = FALSE;
	
	timer_struct_t s_timer;
	s_timer.frequency = 1;
	s_timer.peripheral = TIMER3;
	
	scheduler_init(s_timer);
	
	task_struct_t s_task1;
	s_task1.function = task_1s;
	s_task1.period = 1;
	
	task_struct_t s_task2;
	s_task2.function = task_2s;
	s_task2.period = 2;
	
	scheduler_createTask(s_task1);
	scheduler_createTask(s_task2);
	scheduler_enableTask(s_task1);
	scheduler_enableTask(s_task2);
	scheduler_start();
	
	sei();
}

void mainLoop()
{
	while (1)
	{
		scheduler_loop();
	}
}