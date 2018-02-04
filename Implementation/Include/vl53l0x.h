/**	@file		vl53l0x.h
	@brief		VL53L0X distance sensor
	@details	Supports only master mode without interrupts.
				Basic flow:
				1. Initialize a @link i2c_struct_t @endlink.
				2. Pass it to @link i2c_init @endlink.
				3. Call @link i2c_start @endlink.
				4. Use the following sequences to communicate:
					a. Transmission
						- Call @link i2c_sendStart @endlink with the write address of the slave device
						- Call @link i2c_write @endlink with the register to write to
						- Call @link i2c_write @endlink with the data to write to the register
						- Call @link i2c_sendStop @endlink
					a. Reception
						- Call @link i2c_sendStart @endlink with the write address of the slave device
						- Call @link i2c_write @endlink with the register to read from
						- Call @link i2c_sendRepStart @endlink with the read address of the slave device
						- Call @link i2c_read @endlink to read the register
						- Call @link i2c_sendStop @endlink
				- To stop the I2C (for power saving for instance) call @link i2c_stop @endlink.
*/

#ifndef VL53L0X_h
#define VL53L0X_h

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/**	Default 8bit address of the sensor
*/
#define VL53L0X_ADDRESS_DEFAULT 0b0101001

/**	Information related to a ranging measurement
*/
typedef struct vl53l0x_struct_t
{
	
u8 address;
u16 ioTimeout;
bool timedOut;
u16 timeoutStartMs;
u8 stopVariable;
u32 timingBudget;
}vl53l0x_struct_t;

typedef enum { VcselPeriodPreRange, VcselPeriodFinalRange }vl53l0x_vcselPeriodType_enum_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Starts and initializes the sensor
	@return	Whether the initialization was completed successfuly
*/
bool vl53l0x_init(void);

/**	Puts the sensor in low power mode
	@pre	Must be called after the sensor was initialized (with @link vl53l0x_init @endlink).
*/
void vl53l0x_stop(void);

/** Sets sensor's I2C address
	@pre		Must be called after the sensor was initialized (with @link vl53l0x_init @endlink).
	@remark		Call with 7 bit address
	@param[in]	u8_address: New sensor address
*/
void vl53l0x_setAddress(u8 u8_address);

/** Returns the sensor's I2C address
	@pre		Must be called after the sensor was initialized (with @link vl53l0x_init @endlink).
	@remark		Returns a 7 bit address
	@return		Sensor address
*/
u8 vl53l0x_getAddress(void);

/** Sets the return signal rate limit.  This is the minimum amplitude of the signal reflected from the target and received by the sensor necessary for it to report a valid reading. Setting a lower limit increases the potential range of the sensor but also increases the likelihood of getting an inaccurate reading because of reflections from objects other than the intended target. This limit is initialized to 0.25 by default.
	@pre		Must be called after the sensor was initialized (with @link vl53l0x_init @endlink).
	@param[in]	f_limit: limit in MCPS (mega counts per second)
	@return		Whether the requested limit was valid
*/
bool vl53l0x_setSignalRateLimit(float f_limit);

/** Gets the return signal rate limit.  This is the minimum amplitude of the signal reflected from the target and received by the sensor necessary for it to report a valid reading. Setting a lower limit increases the potential range of the sensor but also increases the likelihood of getting an inaccurate reading because of reflections from objects other than the intended target. This limit is initialized to 0.25 by default.
	@pre	Must be called after the sensor was initialized (with @link vl53l0x_init @endlink).
	@return	Limit in MCPS (mega counts per second)
*/
float vl53l0x_getSignalRateLimit(void);

/** Sets the measurement timing budget, which is the time allowed for one measurement. A longer timing budget allows for more accurate measurements. Increasing the budget by a factor of N decreases the range measurement standard deviation by a factor of sqrt(N). Defaults to about 33 milliseconds; the minimum is 20 ms.
	@pre	Must be called after the sensor was initialized (with @link vl53l0x_init @endlink).
	@param[in]	u32_budget: timing budget in microseconds
	@return		Whether the requested budget was valid
*/
bool vl53l0x_setMeasurementTimingBudget(u32 u32_budget);

/** Gets the measurement timing budget, which is the time allowed for one measurement. A longer timing budget allows for more accurate measurements. Increasing the budget by a factor of N decreases the range measurement standard deviation by a factor of sqrt(N). Defaults to about 33 milliseconds; the minimum is 20 ms.
	@pre	Must be called after the sensor was initialized (with @link vl53l0x_init @endlink).
	@return	timing budget in microseconds
*/
u32 vl53l0x_getMeasurementTimingBudget(void);

/** Sets the VCSEL (Vertical Cavity Surface Emitting Laser) pulse period for the given period type(VcselPeriodPreRange or VcselPeriodFinalRange). Longer periods increase the potential range of the sensor. Valid values are (even numbers only):
	Pre: 12 to 18 (initialized to 14 by default)
	Final: 8 to 14 (initialized to 10 by default)
	@pre	Must be called after the sensor was initialized (with @link vl53l0x_init @endlink).
	@param[in]	e_vcselPeriodType: type of VCSEL pulse period
	@param[in]	u8_clocks: number of clocks to pulse
	@return		Whether the requested period was valid
*/
bool vl53l0x_setVcselPulsePeriod(vl53l0x_vcselPeriodType_enum_t e_vcselPeriodType, u8 u8_clocks);

/** Sets the VCSEL (Vertical Cavity Surface Emitting Laser) pulse period for the given period type(VcselPeriodPreRange or VcselPeriodFinalRange). Longer periods increase the potential range of the sensor. Valid values are (even numbers only):
	Pre: 12 to 18 (initialized to 14 by default)
	Final: 8 to 14 (initialized to 10 by default)
	@pre	Must be called after the sensor was initialized (with @link vl53l0x_init @endlink).
	@param[in]	e_vcselPeriodType: type of VCSEL pulse period
	@return		number of clocks
*/
u8 vl53l0x_getVcselPulsePeriod(vl53l0x_vcselPeriodType_enum_t e_vcselPeriodType);

/** Starts continuous ranging measurements. If the argument is 0, continuous back-to-back mode is used (the sensor takes measurements as often as possible); otherwise continuous timed mode is used, with the specified inter-measurement period determining how often the sensor takes a measurement.
	@pre	Must be called after the sensor was initialized (with @link vl53l0x_init @endlink).
	@param[in]	u32_rangingPeriod: period in milliseconds between two measurements
*/
void vl53l0x_startContinuous(u32 u32_rangingPeriod);

/** Stops continuous ranging mode.
	@pre		Must be called if the sensor is in continuous ranging mode (with @link startContinuous @endlink).
*/
void vl53l0x_stopContinuous(void);

/**	Returns a range reading when continuous mode is active.
	@pre		Must be called if the sensor is in continuous ranging mode (with @link startContinuous @endlink).
	@return		Range in millimeters
*/
u16 vl53l0x_readRangeContinuousMillimeters(void);

/** Performs a single-shot ranging measurement and returns the result.
	@pre	Must be called after the sensor was initialized (with @link vl53l0x_init @endlink).
	@return		Range in millimeters
*/
u16 vl53l0x_readRangeSingleMillimeters(void);

/** Sets a timeout period after which I2C read operations will abort if the sensor is not ready. A value of 0 disables the timeout.
	@param[in]	u16_timeout: timeout in milliseconds
*/
void vl53l0x_setTimeout(u16 u16_timeout);

/** Gets the timeout period after which I2C read operations will abort if the sensor is not ready
	@return		timeout in milliseconds
*/
u16 vl53l0x_getTimeout(void);

/** Indicates whether a I2C read timeout has occurred.
	@return		whether timeout occurred
*/
bool vl53l0x_timeoutOccurred(void);

#endif /* VL53L0X_H_ */