/*
 * encoder.h
 *
 * Created: 10/21/2017 7:22:36 PM
 *  Author: Adrian
 */ 

#include "gpio.h"
#include "math.h"
#include "uart.h"

#ifndef ENCODER_H_
#define ENCODER_H_


void encoder_init();

void encoder_start();

void encoder_stop();

u8 encoder_get();

u32 encoder_getLeft();

u32 encoder_getRight();

void encoder_reset();


#endif /* ENCODER_H_ */