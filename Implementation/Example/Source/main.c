/** @file main.c
*/

#include "device.h"
#include "timer.h"
#include "uart.h"
#include "motor.h"
#include "encoder.h"

#include <avr/interrupt.h>
#define F_CPU	8000000UL
#include <util/delay.h>

uart_struct_t s_debugUart;

void debug_init()
{
	s_debugUart.peripheral = UART0;
	s_debugUart.baudRate = _9600;
	s_debugUart.frameSize = _8BIT;
	s_debugUart.parityBit = NONE;
	s_debugUart.stopBits = _1BIT;
	s_debugUart.useRx = FALSE;
	s_debugUart.useTx = TRUE;

	uart_init(s_debugUart);
	uart_start(s_debugUart);
}


int main(void)
{
	device_disableJTAG();
	debug_init();
	motor_init();
	//encoder_init();
	//encoder_start();
	sei();
    while (1)
    {
		motor_start();
		/*for(u8 i = 30; i<=50; i+=10){
			motor_speed(i);
			_delay_ms(100);
		}*/
		//_delay_ms(1000);
		//motor_individualDirSpeed(BACKWARD, 50, BACKWARD, 50);
		//_delay_ms(2000);
		//motor_individualDirSpeed(FORWARD, 50, BACKWARD, 50);
		_delay_ms(6000);
		//motor_individualDirSpeed(BACKWARD, 50, FORWARD, 50);
		//_delay_ms(6000);
		motor_stop();
		_delay_ms(1000);
    }
}
