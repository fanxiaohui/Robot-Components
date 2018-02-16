/*
 * surface_sensor_example.c
 *
 * Created: 16-Feb-18 10:32:37 PM
 *  Author: Adrian
 */ 

#include "debug.h"
#include "adc.h"
#include "surface_sensor.h"

adc_config_struct_t s_adc;
surface_struct_t sensorOne;
surface_struct_t sensorTwo;

void surfaceSensor_stop(){
	adc_stop(s_adc);
}

void surfaceSensor_start(){
	adc_start(s_adc);
}

void surfaceSensor_read(){
	u8 aux = 0;
	aux = surface_read(sensorOne);
	debug_writeChar(aux);
	aux = surface_read(sensorTwo);
	debug_writeChar(aux);
}

void surfaceSensor_init(){
	s_adc.conversionMode = SINGLE_CONVERSION;
	s_adc.prescaler = ADC_PRESCALER_128;
	
	sensorOne.led_pin.direction = OUTPUT;
	sensorOne.led_pin.number = 1;
	sensorOne.led_pin.port = PB;
	sensorOne.led_pin.pullUp = NO_PULL;
	sensorOne.transistor_channel.channel = 6;
	sensorOne.transistor_channel.referenceVoltage = AVCC;
	sensorOne.transistor_channel.resolution = _8BIT_RES;
	
	sensorTwo.led_pin.direction = OUTPUT;
	sensorTwo.led_pin.number = 0;
	sensorTwo.led_pin.port = PA;
	sensorTwo.led_pin.pullUp = NO_PULL;
	sensorTwo.transistor_channel.channel = 7;
	sensorTwo.transistor_channel.referenceVoltage = AVCC;
	sensorTwo.transistor_channel.resolution = _8BIT_RES;
	adc_init(s_adc);
	surfaceSensor_start();
}
