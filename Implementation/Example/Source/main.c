/** @file main.c
*/

#include "device.h"
#include "timer.h"
#include "uart.h"
#include "motor.h"
#include "encoder.h"
#include "i2c.h"
#include "vl53l0x.h"
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
	u8 data[] = {'a', 'b', 'c', 'd'};
	VL53L0X_i2c_init();
	debug_init();
	sei();
	
	i2c_transmit(8, data, 5);
	while (1)
	{
	}
/*
	VL53L0X_RangingMeasurementData_t measure;
	
	vl53l0x_begin(VL53L0X_I2C_ADDR);
	device_disableJTAG();	
	debug_init();
	
	sei();
	
    while (1)
    {
		vl53l0x_getSingleRangingMeasurement(&measure);
		uart_transmit(s_debugUart, measure.RangeStatus);
		uart_transmit(s_debugUart, measure.RangeMilliMeter >> 8);
		uart_transmit(s_debugUart, measure.RangeMilliMeter & 0xff);
		uart_transmit(s_debugUart, '\n');
    }*/
}
