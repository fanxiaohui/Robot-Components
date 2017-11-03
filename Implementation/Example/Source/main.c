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
#include "vl53l0x_platform.h"

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
extern VL53L0X_Dev_t MyDevice;
int main(void)
{
	VL53L0X_RangingMeasurementData_t measure;
	VL53L0X_DeviceInfo_t pVL53L0X_DeviceInfo;
	u8 data[10];
	data[9] = 0xbf;
	
	device_disableJTAG();	
	debug_init();
	vl53l0x_begin(VL53L0X_I2C_ADDR);
	
	sei();
	
    while (1)
    {
		vl53l0x_getSingleRangingMeasurement(&measure);
		uart_transmit(s_debugUart, measure.RangeStatus);
		uart_transmit(s_debugUart, measure.RangeMilliMeter >> 8);
		uart_transmit(s_debugUart, measure.RangeMilliMeter & 0xff);
		_delay_ms(1000);
    }
}
