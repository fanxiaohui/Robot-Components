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
u8 citire = 0;

void surfaceSensor_stop(){
/*Stops the ADC */	

	/*if(surfaceSensor_isBlack()==FALSE && surfaceSensor_isWhite()==FALSE) */
	adc_stop(s_adc);
	
}

void surfaceSensor_start(){
/*Starts the ADC */	
	adc_start(s_adc);
}


void surfaceSensor_read(){
/*Reads the sensor and stores it in variable "citire" */

	citire = surface_read(sensorOne);
	/*debug_writeChar(citire); */

	return citire;
}
void surfaceSensor_init(){
	
	/*ADC initialization */
	s_adc.conversionMode = SINGLE_CONVERSION;
	s_adc.prescaler = ADC_PRESCALER_128;
	
	/*SensorOne initialization */
	sensorOne.led_pin.direction = OUTPUT;
	sensorOne.led_pin.number = 5;
	sensorOne.led_pin.port = PA;
	sensorOne.led_pin.pullUp = NO_PULL;
	sensorOne.transistor_channel.channel = 1;
	sensorOne.transistor_channel.referenceVoltage = AVCC;
	sensorOne.transistor_channel.resolution = _8BIT_RES;

	adc_init(s_adc);
	surfaceSensor_start();

}

 
void surfaceSensor_isBlack(){
	
	citire = surface_read(sensorOne);
	
/* IF the value transformed from ADC is smaller than 65 is BLACK */		
	if(citire < 65){
		/*debug_writeChar('B');*/
		return TRUE;
	}
	else
	    return FALSE;
}

void surfaceSensor_isWhite(){
	
	citire = surface_read(sensorOne);
	
/* IF the value transformed from ADC is bigger than 230 than is WHITE */	
	if (citire > 230){
		/*debug_writeChar('W');*/
		return TRUE;
	}
	else
		return FALSE;
}


