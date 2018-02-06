/**	@file		vl53l0x.h
	@brief		VL53L0X distance sensor
	@details	Supports only master mode without interrupts.
				Basic flow:
				1. Initialize and start a timer. Make it call @link vl53l0x_incrementTimeoutCounter @endlink every millisecond.
				2. Initialize a @link vl53l0x_struct_t @endlink.
				3. Pass it to @link vl53l0x_init @endlink.
				4. Call @link vl53l0x_start @endlink.
				5. Call @link vl53l0x_startContinuous @endlink to start continuous measurements.
				6. Call @link vl53l0x_readRangeContinuous @endlink to get distance measurements. Measurement hasn't finished yet if return value is 0xFFFF.
				- Optionally you could use @link vl53l0x_setMode @endlink with one of the @link vl53l0x_mode_enum_t @endlink modes to change the measurement duration, accuracy or max range.
				- To stop the sensor (for power saving for instance) call @link vl53l0x_stop @endlink. Remember that continuous ranging has to be started in order to take measurements after calling @link vl53l0x_start @endlink.
*/

#ifndef VL53L0X_h
#define VL53L0X_h

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "gpio.h"

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/**	Default 8bit address of the sensor
*/
#define VL53L0X_ADDRESS_DEFAULT 0b0101001

/**	Ranging modes
*/
typedef enum vl53l0x_mode_enum_t
{
/**	Default ranging mode. Moderate accuracy, 600mm max range, 30ms measurement time */
	VL53L0X_DEFAULT,
/**	High accuracy mode. High accuracy, 600mm max range, 200ms measurement time */
	VL53L0X_MAX_ACCURACY,
/**	High range mode. Low accuracy, 1200mm max range, 33ms measurement time */
	VL53L0X_MAX_RANGE,
/**	High speed mode. Low accuracy, 600mm max range, 20ms measurement time */
	VL53L0X_MAX_SPEED
}vl53l0x_mode_enum_t;

/**	Information related to a ranging measurement
*/
typedef struct vl53l0x_struct_t
{
/**	I2C address. Must call @link vl53l0x_setAddress @endlink to change it.
*/
	u8 address;
/**	I2C request timeout in milliseconds
*/
	u16 i2cTimeout;
/**	Indicates whether an I2C timeout occurred
*/
	bool timedOut;
/**	XSHUT pin which may be used to turn on/off the sensor. Only setting pin and port will be enough. Must be used if more than 1 sensor is connected on the same I2C bus.
	@remark	Do not control this pin directly!
*/
	gpio_struct_t xshutPin;
/**	Start of timeout counter
@remark	Do not modify!
*/
	u16 timeoutStart;
/** Stop variable from manufacturer API, used when starting a measurement.
	@remark	Do not modify!
*/
	u8 stopVariable;
}vl53l0x_struct_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Initializes the sensor
	@param[in]	ps_sensor: sensor to use
*/
void vl53l0x_init(vl53l0x_struct_t* ps_sensor);

/** Starts the sensor
	@param[in]	ps_sensor: sensor to use
	@return		Whether the initialization sequence was completed successfuly
*/
bool vl53l0x_start(vl53l0x_struct_t* ps_sensor);

/**	Puts the sensor in low power mode
	@pre		Must be called after the sensor was initialized (with @link vl53l0x_init @endlink).
	@remark		This doesn't stop the I2C peripheral because other devices may be connected to it.
	@param[in]	ps_sensor: sensor to use
*/
void vl53l0x_stop(vl53l0x_struct_t* ps_sensor);

/** Sets sensor's I2C address
	@pre		Must be called after the sensor was initialized (with @link vl53l0x_init @endlink).
	@remark		Call with 7 bit address
	@param[in]	ps_sensor: sensor to use
	@param[in]	u8_address: New sensor address
*/
void vl53l0x_setAddress(vl53l0x_struct_t* ps_sensor, u8 u8_address);

/** Switches between one of the ranging modes defined by @link vl53l0x_mode_enum_t @endlink
	@pre		Must be called after the sensor was initialized (with @link vl53l0x_init @endlink).
	@param[in]	ps_sensor: sensor to use
	@param[in]	e_mode: mode to set
	@return		Sensor address
*/
bool vl53l0x_setMode(vl53l0x_struct_t* ps_sensor, vl53l0x_mode_enum_t e_mode);

/** Starts continuous ranging measurements. If the argument is 0, continuous back-to-back mode is used (the sensor takes measurements as often as possible); otherwise continuous timed mode is used, with the specified inter-measurement period determining how often the sensor takes a measurement.
	@pre	Must be called after the sensor was initialized (with @link vl53l0x_init @endlink).
	@param[in]	ps_sensor: sensor to use
	@param[in]	u32_rangingPeriod: period in milliseconds between two measurements
*/
void vl53l0x_startContinuous(vl53l0x_struct_t* ps_sensor, u32 u32_rangingPeriod);

/** Stops continuous ranging mode.
	@pre		Must be called if the sensor is in continuous ranging mode (with @link vl53l0x_startContinuous @endlink).
	@param[in]	ps_sensor: sensor to use
*/
void vl53l0x_stopContinuous(vl53l0x_struct_t* ps_sensor);

/**	Returns a range reading when continuous mode is active.
	@pre		Must be called if the sensor is in continuous ranging mode (with @link vl53l0x_startContinuous @endlink).
	@param[in]	ps_sensor: sensor to use
	@return		Range in millimeters
*/
u16 vl53l0x_readRangeContinuous(vl53l0x_struct_t* ps_sensor);

/** Performs a single-shot ranging measurement and returns the result.
	@pre	Must be called after the sensor was initialized (with @link vl53l0x_init @endlink).
	@param[in]	ps_sensor: sensor to use
	@return		Range in millimeters
*/
u16 vl53l0x_readRangeSingle(vl53l0x_struct_t* ps_sensor);

/**	Increments the timing variable used for I2C communication timeouts
	@remark		Call this every millisecond
*/
void vl53l0x_incrementTimeoutCounter();

/** Indicates whether a I2C communication timeout has occurred.
	@param[in]	ps_sensor: sensor to use
	@return		whether timeout occurred
*/
bool vl53l0x_timeoutOccurred(vl53l0x_struct_t* ps_sensor);

#endif /* VL53L0X_H_ */