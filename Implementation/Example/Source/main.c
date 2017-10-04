/** @file main.c
*/

#include "device.h"
#include "vl53l0x.h"
#include "timer.h"
#include "uart.h"
#include "wdg.h"

#include <avr/interrupt.h>
#define F_CPU	8000000UL
#include <util/delay.h>

uart_struct_t s_debugUart;
timer_struct_t s_schedulerTimer;

u32 milliseconds = 0;

void debug_init()
{
	s_debugUart.peripheral = UART0;
	s_debugUart.baudRate = _19200;
	s_debugUart.frameSize = _8BIT;
	s_debugUart.parityBit = NONE;
	s_debugUart.stopBits = _1BIT;
	s_debugUart.useRx = TRUE;
	s_debugUart.useTx = TRUE;

	uart_init(s_debugUart);
	uart_start(s_debugUart);
}

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

int main(void)
{
	u8 u8_distance = 0;
	device_disableJTAG();
	debug_init();

	vl53l0x_init();

	scheduler_init();
	sei();

	vl53l0x_start();

    while (1)
    {
		u8_distance = vl53l0x_getDistance();
	    uart_transmit(s_debugUart, (u8)(u8_distance >> 8));
	    uart_transmit(s_debugUart, u8_distance);
	    uart_transmit(s_debugUart, '\n');
	    //uart_transmit(s_debugUart, b_vl53l0x_testConnection());
		_delay_ms(100);
    }
}
