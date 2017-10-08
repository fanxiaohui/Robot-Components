/*
 * vl53l0x.h
 *
 * Created: 10/5/2017 12:12:45 AM
 *  Author: Florin Popescu
 */ 


#ifndef VL53L0X_H_
#define VL53L0X_H_

#include "gpio.h"
#include "i2c.h"
#include "types.h"

extern gpio_struct_t s_xshutPin;

void vl53l0x_init();
void vl53l0x_calibrate();
void vl53l0x_start();
void vl53l0x_stop();
u8 vl53l0x_writeRegister(u8 u8_address, u8 u8_register, u8 *au8_data, u8 u8_dataLength);
u8 vl53l0x_readRegister(u8 u8_address, u8 u8_register, u8 *au8_data, u8 u8_dataLength);
u8 b_vl53l0x_testConnection();
void vl53l0x_getDeviceID(u8 *major, u8* minor);
u16 vl53l0x_getDistance();

#endif /* VL53L0X_H_ */