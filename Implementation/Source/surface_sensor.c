/*
 * surface_sensor.c
 *
 * Created: 11/4/2017 1:17:08 AM
 *  Author: Andrei
 */ 
 
#include "surface_sensor.h"
#include "adc.h"
#include "gpio.h"
#include "types.h"

extern adc_config_struct_t s_adc;

u8 white_treshold = 100, black_treshold = 200;
u8 max = 0;
u8 min = 255;


void surface_sensor_init(surface_struct_t surface_struct)
{
	//adc_init(surface_struct.transistor_channel);
	//adc_start(surface_struct.transistor_channel);

	surface_struct.led_pin.direction = OUTPUT;

	gpio_init(surface_struct.led_pin);
}

u8 surface_read(surface_struct_t surface_struct)
{
	u8 i = 0;
	gpio_out_set(surface_struct.led_pin);

	for(i = 0; i < 200; i++)
	{
		asm("nop");
	}
	
	u8 read = adc_singleRead(s_adc, surface_struct.transistor_channel);
	//gpio_out_reset(surface_struct.led_pin);

	return read;
}

bool surface_IsWhite(surface_struct_t surface_struct)
{
	u8 read = surface_read(surface_struct);
	
	if(read <= white_treshold)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}	
}

bool surface_IsBlack(surface_struct_t surface_struct)
{
	u8 read = surface_read(surface_struct);

	if(read > black_treshold)
	{
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}
void surface_setWhiteTreshold(u8 treshold)
{
	white_treshold = treshold + 80;
}

void surface_setBlackTreshold(u8 treshold)
{
	black_treshold = treshold - 50;
}

void surface_getCalibrationValues(surface_struct_t surface_struct)
{
	u8 temp = surface_read(surface_struct);

	if( temp > max)
	{
		max = temp;
	}
	else if(temp < min)
	{
		min = temp;
	}
}

void surface_setCalibrationValues()
{
	surface_setWhiteTreshold(min);
	surface_setBlackTreshold(max);
}