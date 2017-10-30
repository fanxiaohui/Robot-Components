/** @file main.c
*/

#include "device.h"
#include "timer.h"
#include "uart.h"
#include "motor.h"
#include "encoder.h"
#include "i2c.h"
#include "VL53L0X.h"
#include "scheduler.h"

#include <avr/interrupt.h>
#define F_CPU	8000000UL
#include <util/delay.h>

uart_struct_t s_debugUart;
timer_struct_t s_schedulerTimer;

volatile u32 milliseconds = 0;

void incrementMillis()
{
	milliseconds++;
}

void scheduler_init()
{
	s_schedulerTimer.frequency = 1000;
	s_schedulerTimer.peripheral = TIMER1;
	
	timer_init(s_schedulerTimer);
	timer_attachInterrupt(s_schedulerTimer, OVERFLOW, incrementMillis);
	timer_enableInterrupt(s_schedulerTimer, OVERFLOW);
	timer_start(s_schedulerTimer);
}
static u32 left_EncoderCounter;
static u32 right_EncoderCounter;
timer_struct_t s_scheduler_timer;
uart_struct_t s_debugUart;

void debug_init()
{
	s_debugUart.baudRate = 9600;
	s_debugUart.frameSize = _8BIT;
	s_debugUart.parityBit = NONE;
	s_debugUart.peripheral = UART0;
	s_debugUart.stopBits = _1BIT;
	s_debugUart.useTx = TRUE;
	s_debugUart.useRx = FALSE;
	uart_init(s_debugUart);
	uart_start(s_debugUart);
}

extern void Task_ReadEncoders ()
{
	left_EncoderCounter = encoder_getLeft ();
	right_EncoderCounter = encoder_getRight();
	uart_transmit(s_debugUart, right_EncoderCounter);
}

void sensor_i2c_init()
{
<<<<<<< HEAD
	if ( left_EncoderCounter > 100 )
	{
			//motor_speed(10);
	}
	else 
	{
		//motor_speed(40);
	}
	
=======
	i2c_struct_t s_i2c;
	s_i2c.frequency = 80000;
	s_i2c.mode = I2C_MASTER;
	i2c_init(s_i2c);
>>>>>>> a9817fae78f48c85f472b13bc04431952ffe64b2
}

int main(void)
{
	u16 u16_distance;
	
	device_disableJTAG();
<<<<<<< HEAD
	encoder_init();
	encoder_start();
	debug_init();
	motor_init();		
	scheduler_inti(s_scheduler_timer,OVERFLOW);
	create_task(1,Task_SetMotorSpeed);
	create_task(0,Task_ReadEncoders);
	activate_task(0,10);
	activate_task(1,15);
	motor_start();
	sei();
	
	uart_transmit(s_debugUart, 'a');
=======
	debug_init();
	motor_init();
	scheduler_init();
	sensor_i2c_init();
	VL53L0X_init();
	setTimeout(500);
	startContinuous();
	
	sei();
	
>>>>>>> a9817fae78f48c85f472b13bc04431952ffe64b2
    while (1)
    {
		u16_distance = readRangeContinuousMillimeters();
		if (timeoutOccurred())
			uart_transmit(s_debugUart, '1');
		else
		{
			uart_transmit(s_debugUart, u16_distance >> 8);
			uart_transmit(s_debugUart, u16_distance & 0xff);
		}
		uart_transmit(s_debugUart, '\n');
    }
}
