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


extern void Task_ReadEncoders ()
{
	left_EncoderCounter = encoder_getLeft ();
	right_EncoderCounter = encoder_getRight();
}

void sensor_i2c_init()
{
	i2c_struct_t s_i2c;
	s_i2c.frequency = 80000;
	s_i2c.mode = I2C_MASTER;
	i2c_init(s_i2c);
}

int main(void)
{
	u16 u16_distance;
	
	device_disableJTAG();
	debug_init();
	motor_init();
	scheduler_init();
	sensor_i2c_init();
	VL53L0X_init();
	setTimeout(500);
	startContinuous();
	
	sei();
	
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
