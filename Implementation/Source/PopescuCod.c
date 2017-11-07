/** @file main.c
*/

#include "device.h"
#include "uart.h"
#include "Adafruit_VL53L0X.h"


#include <avr/interrupt.h>
#define F_CPU	8000000UL
#include <util/delay.h>

uart_struct_t s_debugUart;

void debug_init()
{
	s_debugUart.baudRate = _9600;
	s_debugUart.frameSize = _8BIT;
	s_debugUart.parityBit = NONE;
	s_debugUart.peripheral = UART0;
	s_debugUart.stopBits = _1BIT;
	s_debugUart.useRx = FALSE;
	s_debugUart.useTx = TRUE;
	uart_init(s_debugUart);
	uart_start(s_debugUart);
}

int main(void)
{
	VL53L0X_RangingMeasurementData_t measure;

	device_disableJTAG();
	debug_init();
	Adafruit_VL53L0X_begin(VL53L0X_I2C_ADDR, FALSE);
	sei();
	
    while (1)
    {
	    uart_transmit(s_debugUart, 'a');
		Adafruit_VL53L0X_getSingleRangingMeasurement(&measure, FALSE);
		uart_transmit(s_debugUart, measure.RangeMilliMeter);
    }
}
