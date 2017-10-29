/** @file main.c
*/

#include "device.h"
#include "timer.h"
#include "uart.h"
#include "motor.h"
#include "encoder.h"
#include "scheduler.h"


#include <avr/interrupt.h>
#define F_CPU	8000000UL
#include <util/delay.h>

static u32 left_EncoderCounter;
static u32 right_EncoderCounter;
timer_struct_t s_scheduler_timer;


extern void Task_ReadEncoders ()
{
	left_EncoderCounter = encoder_getLeft ();
	right_EncoderCounter = encoder_getRight();
}

extern void Task_SetMotorSpeed()
{
	
	if ( left_EncoderCounter > 0 )
	{
			motor_speed(10);
	}
	else 
	{
		motor_speed(40);
	}
	
}

int main(void)
{
	
	s_scheduler_timer.peripheral = TIMER2;
	s_scheduler_timer.frequency = 1000;
	left_EncoderCounter = 0 ;
	right_EncoderCounter =0 ;
	device_disableJTAG();
	encoder_init();
	encoder_start();
	motor_init();		
	scheduler_inti(s_scheduler_timer,OVERFLOW);
	create_task(1,Task_SetMotorSpeed);
	create_task(0,Task_ReadEncoders);
	activate_task(0,10);
	activate_task(1,15);
	motor_start();
	motor_speed(1);
	sei();
    while (1)
    {
		scheduler();
    }
}
