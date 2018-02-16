/*
 * surface_sensor.h
 *
 * Created: 11/4/2017 1:12:20 AM
 *  Author: Andrei
 */ 

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/




#ifndef SURFACE_SENSOR_H_
#define SURFACE_SENSOR_H_

#include "types.h"
#include "adc.h"
#include "gpio.h"

typedef struct surface_struct_t
{
	adc_channel_struct_t transistor_channel;
	gpio_struct_t led_pin;
}surface_struct_t;

/** Initializes the surface sensor
*/
void surface_sensor_init(surface_struct_t surface_struct);

/** Reads data from the surface sensor
*/
u8 surface_read(surface_struct_t surface_struct);

/** Checks if the color is White
*/
bool surface_IsWhite(surface_struct_t surface_struct);

/** Checks if the color is Black
*/
bool surface_IsBlack(surface_struct_t surface_struct);

/** Disables a pin change interrupt.
	@param[in]	white_treshold: Value over which the code can consider the value is white
*/
void surface_setWhiteTreshold(u8 white_treshold);

/** Disables a pin change interrupt.
	@param[in]	black_treshold: Value over which the code can consider the value is black
*/
void surface_setBlackTreshold(u8 black_treshold);

/** Get min and max values for the treshold
*/
void surface_getCalibrationValues(surface_struct_t surface_struct);

/** Sets the min and max for the treshold
*/
void surface_setCalibrationValues();

#endif /* SURFACE_SENSOR_H_ */