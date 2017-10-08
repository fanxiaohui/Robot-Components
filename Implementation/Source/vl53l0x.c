/*
 * vl53l0x.c
 *
 * Created: 10/5/2017 12:12:54 AM
 *  Author: Florin Popescu
 */

#include "vl53l0x.h"
#include "VL53L0X/vl53l0x_api.h"
#include "VL53L0X/vl53l0x_platform.h"
#include <stdlib.h>

#define F_CPU	8000000UL
#include <util/delay.h>

#define VERSION_REQUIRED_MAJOR 1
#define VERSION_REQUIRED_MINOR 0
#define VERSION_REQUIRED_BUILD 1

gpio_struct_t s_xshutPin;
VL53L0X_Dev_t frontSensor;

void vl53l0x_init()
{
	s_xshutPin.port = PC;
	s_xshutPin.number = 2;
	s_xshutPin.direction = OUTPUT;
	s_xshutPin.pullUp = NO_PULL;

	gpio_init(s_xshutPin);

	i2c_struct_t s_i2c;
	s_i2c.frequency = 80000;
	s_i2c.mode = I2C_MASTER;
	i2c_init(s_i2c);
	i2c_start();
	gpio_out_set(s_xshutPin);
	/* Wait 1.2ms for device to power up as datasheet specifies. */
	_delay_ms(1);
	_delay_us(200);

	frontSensor.I2cDevAddr = 41;

	VL53L0X_DataInit(&frontSensor);
	VL53L0X_StaticInit(&frontSensor);
}

void vl53l0x_calibrate()
{
	u32 refSpadCount;
	u8 isApertureSpads;
	u8 vhvSettings;
	u8 phaseCal;
	s32 offsetCalibrationDataMicroMeter;
	f1616 xTalkCompensationRateMegaCps;

	VL53L0X_GetReferenceSpads(&frontSensor, &refSpadCount, &isApertureSpads);
	VL53L0X_SetReferenceSpads(&frontSensor, refSpadCount, isApertureSpads);
	VL53L0X_GetRefCalibration(&frontSensor, &vhvSettings, &phaseCal);
	VL53L0X_SetRefCalibration(&frontSensor, vhvSettings, phaseCal);
	VL53L0X_GetOffsetCalibrationDataMicroMeter(&frontSensor, &offsetCalibrationDataMicroMeter);
	VL53L0X_SetOffsetCalibrationDataMicroMeter(&frontSensor, offsetCalibrationDataMicroMeter);
	VL53L0X_GetXTalkCompensationRateMegaCps(&frontSensor, &xTalkCompensationRateMegaCps);
	VL53L0X_SetXTalkCompensationRateMegaCps(&frontSensor, xTalkCompensationRateMegaCps);
	VL53L0X_SetXTalkCompensationEnable(&frontSensor, 1);
}

void vl53l0x_start()
{
	VL53L0X_SetDeviceMode(&frontSensor, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
	VL53L0X_StartMeasurement(&frontSensor);
}

void vl53l0x_stop()
{
}

u8 vl53l0x_writeRegister(u8 u8_address, u8 u8_register, u8 *au8_data, u8 u8_dataLength)
{
	u8 u8_returnValue = I2C_NO_ERROR;
	u8 u8_returnValue2 = I2C_NO_ERROR;

	u8_returnValue = i2c_transmit(u8_address, &u8_register, 1);
	u8_returnValue2 = i2c_transmit(u8_address, au8_data, u8_dataLength);

	return u8_returnValue | u8_returnValue2;
}

u8 vl53l0x_readRegister(u8 u8_address, u8 u8_register, u8 *au8_data, u8 u8_dataLength)
{
	u8 u8_returnValue = I2C_NO_ERROR;
	u8 u8_returnValue2 = I2C_NO_ERROR;

	u8_returnValue = i2c_transmit(u8_address, &u8_register, 1);
	u8_returnValue2 = i2c_receive(u8_address, au8_data, u8_dataLength);

	return u8_returnValue | u8_returnValue2;
}

u8 b_vl53l0x_testConnection()
{
	u8 u8_register = 0xc0;
	u8 au8_dataIn[1];
	u8 u8_returnValue;

	u8_returnValue = vl53l0x_readRegister(41, u8_register, au8_dataIn, 1);
	if (u8_returnValue == I2C_NO_ERROR)
		return au8_dataIn[0];
	else
		return 0;
}

void vl53l0x_getDeviceID(u8 *major, u8* minor)
{
	VL53L0X_GetProductRevision(&frontSensor, major, minor);
}

u16 vl53l0x_getDistance()
{
	u16 u16_returnValue = 0;
	VL53L0X_RangingMeasurementData_t rangingMeasurementData;

	VL53L0X_GetRangingMeasurementData(&frontSensor, &rangingMeasurementData);
	u16_returnValue = rangingMeasurementData.RangeStatus;

	return u16_returnValue;
}
