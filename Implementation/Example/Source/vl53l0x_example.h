/**	@file		vl53l0x_example.h
	@brief		Example usage for VL53L0X distance sensor
	@details	See @link vl53l0x.h @endlink for details
*/

#ifndef VL53L0X_EXAMPLE_H_
#define VL53L0X_EXAMPLE_H_

void distanceSensor_init();
void distanceSensor_stop();
void distanceSensor_defaultTest();
void distanceSensor_sleepTest();
void distanceSensor_maxAccuracyTest();
void distanceSensor_maxRangeTest();
void distanceSensor_maxSpeedTest();
void distanceSensor_singleTest();
void distanceSensor_obstacleTest();
void distanceSensor_multiInit();
void distanceSensor_multiDefaultTest();

#endif /* VL53L0X_EXAMPLE_H_ */