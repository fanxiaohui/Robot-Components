/*
 * surface_sensor_example.h
 *
 * Created: 16-Feb-18 10:32:28 PM
 *  Author: Adrian
 */ 

#include "surface_sensor.h"


#ifndef SURFACE_SENSOR_EXAMPLE_H_
#define SURFACE_SENSOR_EXAMPLE_H_

void surfaceSensor_init();
void surfaceSensor_start();
int surfaceSensor_read();
void surfaceSensor_stop();
bool surfaceSensor_isBlack();
bool surfaceSensor_isWhite();



#endif /* SURFACE_SENSOR_EXAMPLE_H_ */