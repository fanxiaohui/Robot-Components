/** @file main.c
*/

#include "device.h"
#include "timer.h"
#include "uart.h"
#include "motor.h"

#include <avr/interrupt.h>
#define F_CPU	8000000UL
#include <util/delay.h>

uart_struct_t s_debugUart;

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



int main(void)
{
	device_disableJTAG();
	//debug_init();
	sei();
	motor_init();
    while (1)
    {
		motor_start();
		_delay_ms(3000);
		for(u8 i = 30; i <= 50; i+=5){
			motor_speed(i);
			_delay_ms(100);
		}
		_delay_ms(3000);
		motor_direction(BACKWARD);
		_delay_ms(2000);
		motor_direction(LEFT);
		for(u8 i = 30; i <= 90; i+=10){
			motor_speed(i);
			_delay_ms(100);
		}
		_delay_ms(5000);
		for(u8 i = 30; i <= 90; i+=10){
			motor_speed(i);
			_delay_ms(100);
		}
		motor_direction(RIGHT);
		_delay_ms(5000);
		motor_stop();
		_delay_ms(2000);
    }
}
