/**	@file		vl53l0x.c
	@brief		VL53L0X distance sensor
	@details	See @link vl53l0x.h @endlink for details.
*/

/************************************************************************/
/* AVR includes                                                         */
/************************************************************************/

#include <stdint.h>

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "i2c.h"
#include "vl53l0x.h"

/************************************************************************/
/* Internal , enums, structs, types                                     */
/************************************************************************/

#define	SYSRANGE_START								0x00
#define SYSTEM_THRESH_HIGH                          0x0C
#define SYSTEM_THRESH_LOW                           0x0E
#define SYSTEM_SEQUENCE_CONFIG                      0x01
#define SYSTEM_RANGE_CONFIG                         0x09
#define SYSTEM_INTERMEASUREMENT_PERIOD              0x04
#define SYSTEM_INTERRUPT_CONFIG_GPIO                0x0A
#define GPIO_HV_MUX_ACTIVE_HIGH                     0x84
#define SYSTEM_INTERRUPT_CLEAR                      0x0B
#define RESULT_INTERRUPT_STATUS                     0x13
#define RESULT_RANGE_STATUS                         0x14
#define RESULT_CORE_AMBIENT_WINDOW_EVENTS_RTN       0xBC
#define RESULT_CORE_RANGING_TOTAL_EVENTS_RTN        0xC0
#define RESULT_CORE_AMBIENT_WINDOW_EVENTS_REF       0xD0
#define RESULT_CORE_RANGING_TOTAL_EVENTS_REF        0xD4
#define RESULT_PEAK_SIGNAL_RATE_REF                 0xB6
#define ALGO_PART_TO_PART_RANGE_OFFSET_MM           0x28
#define I2C_SLAVE_DEVICE_ADDRESS                    0x8A
#define MSRC_CONFIG_CONTROL                         0x60
#define PRE_RANGE_CONFIG_MIN_SNR                    0x27
#define PRE_RANGE_CONFIG_VALID_PHASE_LOW            0x56
#define PRE_RANGE_CONFIG_VALID_PHASE_HIGH           0x57
#define PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT          0x64
#define FINAL_RANGE_CONFIG_MIN_SNR                  0x67
#define FINAL_RANGE_CONFIG_VALID_PHASE_LOW          0x47
#define FINAL_RANGE_CONFIG_VALID_PHASE_HIGH         0x48
#define FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT 0x44
#define PRE_RANGE_CONFIG_SIGMA_THRESH_HI            0x61
#define PRE_RANGE_CONFIG_SIGMA_THRESH_LO            0x62
#define PRE_RANGE_CONFIG_VCSEL_PERIOD               0x50
#define PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI          0x51
#define PRE_RANGE_CONFIG_TIMEOUT_MACROP_LO          0x52
#define SYSTEM_HISTOGRAM_BIN                        0x81
#define HISTOGRAM_CONFIG_INITIAL_PHASE_SELECT       0x33
#define HISTOGRAM_CONFIG_READOUT_CTRL               0x55
#define FINAL_RANGE_CONFIG_VCSEL_PERIOD             0x70
#define FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI        0x71
#define FINAL_RANGE_CONFIG_TIMEOUT_MACROP_LO        0x72
#define CROSSTALK_COMPENSATION_PEAK_RATE_MCPS       0x20
#define MSRC_CONFIG_TIMEOUT_MACROP                  0x46
#define SOFT_RESET_GO2_SOFT_RESET_N                 0xBF
#define IDENTIFICATION_MODEL_ID                     0xC0
#define IDENTIFICATION_REVISION_ID                  0xC2
#define OSC_CALIBRATE_VAL                           0xF8
#define GLOBAL_CONFIG_VCSEL_WIDTH                   0x32
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_0            0xB0
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_1            0xB1
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_2            0xB2
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_3            0xB3
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_4            0xB4
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_5            0xB5
#define GLOBAL_CONFIG_REF_EN_START_SELECT           0xB6
#define DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD         0x4E
#define DYNAMIC_SPAD_REF_EN_START_OFFSET            0x4F
#define POWER_MANAGEMENT_GO1_POWER_FORCE            0x80
#define VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV           0x89
#define ALGO_PHASECAL_LIM                           0x30
#define ALGO_PHASECAL_CONFIG_TIMEOUT                0x30

typedef struct sequenceStepEnables_t
{
	u8 tcc, msrc, dss, pre_range, final_range;
}sequenceStepEnables_t;

typedef struct sequenceStepTimeouts_t
{
	u16 pre_range_vcsel_period_pclks, final_range_vcsel_period_pclks;

	u16 msrc_dss_tcc_mclks, pre_range_mclks, final_range_mclks;
	u32 msrc_dss_tcc_us,    pre_range_us,    final_range_us;
}sequenceStepTimeouts_t;

/************************************************************************/
/* Internal variables                                                   */
/************************************************************************/

i2c_struct_t s_i2cInterface;
extern volatile u32 u32_milliseconds;

//---------------------------------------------------------
// Local variables within this file (private)
//---------------------------------------------------------
u8 g_i2cAddr = VL53L0X_ADDRESS_DEFAULT;
u16 g_ioTimeout = 0;  // no timeout
u8 g_isTimeout = 0;
u16 g_timeoutStartMs;
u8 g_stopVariable; // read by init and used when starting measurement; is StopVariable field of VL53L0X_DevData_t structure in API
u32 g_measTimBudUs;

//---------------------------------------------------------
// Locally used functions (private)
//---------------------------------------------------------
//--------------------------------------------------------- 

void startTimeout()
{
	g_timeoutStartMs = u32_milliseconds;
}

// Check if timeout is enabled (set to nonzero value) and has expired
bool checkTimeoutExpired()
{
	return g_ioTimeout > 0 && (u32_milliseconds - g_timeoutStartMs) > g_ioTimeout;
}

// Decode VCSEL (vertical cavity surface emitting laser) pulse period in PCLKs
// from register value
// based on VL53L0X_decode_vcsel_period()
u8 decodeVcselPeriod(u8 reg_val)
{
	return ((reg_val) + 1) << 1;
}

// Encode VCSEL pulse period register value from period in PCLKs
// based on VL53L0X_encode_vcsel_period()
u8 encodeVcselPeriod(u8 period_pclks)
{
	return ((period_pclks) >> 1) - 1;
}

// Calculate macro period in *nanoseconds* from VCSEL period in PCLKs
// based on VL53L0X_calc_macro_period_ps()
// PLL_period_ps = 1655; macro_period_vclks = 2304
u32 calcMacroPeriod(u8 vcsel_period_pclks)
{
	return (((u32) 2304 * (vcsel_period_pclks) * 1655) + 500) / 1000;
}


bool getSpadInfo(u8 *count, bool *type_is_aperture);
void getSequenceStepEnables(sequenceStepEnables_t * enables);
void getSequenceStepTimeouts(sequenceStepEnables_t const * enables, sequenceStepTimeouts_t * timeouts);
bool performSingleRefCalibration(u8 vhv_init_byte);
static u16 decodeTimeout(u16 value);
static u16 encodeTimeout(u16 timeout_mclks);
static u32 timeoutMclksToMicroseconds(u16 timeout_period_mclks, u8 vcsel_period_pclks);
static u32 timeoutMicrosecondsToMclks(u32 timeout_period_us, u8 vcsel_period_pclks);

//---------------------------------------------------------
// I2C communication Functions
//---------------------------------------------------------
// Write an 8-bit register
void writeReg(u8 reg, u8 value) {
	i2c_sendStart( (g_i2cAddr << 1) | I2C_WRITE );
	i2c_write(reg);
	i2c_write(value);
	i2c_sendStop();
}

// Write a 16-bit register
void writeReg16Bit(u8 reg, u16 value){
	i2c_sendStart( (g_i2cAddr << 1) | I2C_WRITE );
	i2c_write(reg);
	i2c_write((value >> 8) & 0xFF);
	i2c_write((value     ) & 0xFF);
	i2c_sendStop();
}

// Write a 32-bit register
void writeReg32Bit(u8 reg, u32 value){
	i2c_sendStart( (g_i2cAddr << 1) | I2C_WRITE );
	i2c_write(reg);
	i2c_write((value >>24) & 0xFF);
	i2c_write((value >>16) & 0xFF);
	i2c_write((value >> 8) & 0xFF);
	i2c_write((value     ) & 0xFF);
	i2c_sendStop();
}

// Read an 8-bit register
u8 readReg(u8 reg) {
	u8 value;
	i2c_sendStart( (g_i2cAddr << 1) | I2C_WRITE );
	i2c_write( reg );
	i2c_sendRepStart( (g_i2cAddr << 1) | I2C_READ );
	value = i2c_readNak();
	i2c_sendStop();
	return value;
}

// Read a 16-bit register
u16 readReg16Bit(u8 reg) {
	u16 value;
	i2c_sendStart( (g_i2cAddr << 1) | I2C_WRITE );
	i2c_write( reg );
	i2c_sendRepStart( (g_i2cAddr << 1) | I2C_READ );
	value  = i2c_readAck() << 8;
	value |= i2c_readNak();
	i2c_sendStop();
	return value;
}

// Read a 32-bit register
u32 readReg32Bit(u8 reg) {
	u32 value;
	i2c_sendStart( (g_i2cAddr << 1) | I2C_WRITE );
	i2c_write( reg );
	i2c_sendRepStart( (g_i2cAddr << 1) | I2C_READ );
	value  = (u32)i2c_readAck() <<24;
	value |= (u32)i2c_readAck() <<16;
	value |= (u32)i2c_readAck() << 8;
	value |= i2c_readNak();
	i2c_sendStop();
	return value;
}

// Write an arbitrary number of bytes from the given array to the sensor,
// starting at the given register
void writeMulti(u8 reg, u8 const *src, u8 count){
	i2c_sendStart( (g_i2cAddr << 1) | I2C_WRITE );
	i2c_write( reg );
	while ( count-- > 0 ) {
		i2c_write( *src++ );
	}
	i2c_sendStop();
}

// Read an arbitrary number of bytes from the sensor, starting at the given
// register, into the given array
void readMulti(u8 reg, u8 * dst, u8 count) {
	i2c_sendStart( (g_i2cAddr << 1) | I2C_WRITE );
	i2c_write( reg );
	i2c_sendRepStart( (g_i2cAddr << 1) | I2C_READ );
	while ( count > 0 ) {
		if ( count > 1 ){
			*dst++ = i2c_readAck();
			} else {
			*dst++ = i2c_readNak();
		}
		count--;
	}
	i2c_sendStop();
}

void vl53l0x_stop()
{
	
}

// Public Methods //////////////////////////////////////////////////////////////

void vl53l0x_setAddress(u8 u8_address) {
	writeReg( I2C_SLAVE_DEVICE_ADDRESS, u8_address & 0x7F );
	g_i2cAddr = u8_address;
}

u8 vl53l0x_getAddress() {
	return g_i2cAddr;
}

// Initialize sensor using sequence based on VL53L0X_DataInit(),
// VL53L0X_StaticInit(), and VL53L0X_PerformRefCalibration().
// This function does not perform reference SPAD calibration
// (VL53L0X_PerformRefSpadManagement()), since the API user manual says that it
// is performed by ST on the bare modules; it seems like that should work well
// enough unless a cover glass is added.
// If io_2v8 (optional) is TRUE or not given, the sensor is configured for 2V8
// mode.
bool vl53l0x_init(void){
	
	s_i2cInterface.frequency = 400000;
	i2c_init(s_i2cInterface);
	i2c_start();

	// VL53L0X_DataInit() begin

	writeReg(VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV, readReg(VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV) | 0x01);

	// "Set I2C standard mode"
	writeReg(0x88, 0x00);

	writeReg(0x80, 0x01);
	writeReg(0xFF, 0x01);
	writeReg(0x00, 0x00);
	g_stopVariable = readReg(0x91);
	writeReg(0x00, 0x01);
	writeReg(0xFF, 0x00);
	writeReg(0x80, 0x00);

	// disable SIGNAL_RATE_MSRC (bit 1) and SIGNAL_RATE_PRE_RANGE (bit 4) limit checks
	writeReg(MSRC_CONFIG_CONTROL, readReg(MSRC_CONFIG_CONTROL) | 0x12);

	// set final range signal rate limit to 0.25 MCPS (million counts per second)
	vl53l0x_setSignalRateLimit(0.25);

	writeReg(SYSTEM_SEQUENCE_CONFIG, 0xFF);

	// VL53L0X_DataInit() end

	// VL53L0X_StaticInit() begin

	u8 spad_count;
	bool spad_type_is_aperture;
	if (!getSpadInfo(&spad_count, &spad_type_is_aperture)) { return FALSE; }

	// The SPAD map (RefGoodSpadMap) is read by VL53L0X_get_info_from_device() in
	// the API, but the same data seems to be more easily readable from
	// GLOBAL_CONFIG_SPAD_ENABLES_REF_0 through _6, so read it from there
	u8 ref_spad_map[6];
	readMulti(GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

	// -- VL53L0X_set_reference_spads() begin (assume NVM values are valid)

	writeReg(0xFF, 0x01);
	writeReg(DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00);
	writeReg(DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C);
	writeReg(0xFF, 0x00);
	writeReg(GLOBAL_CONFIG_REF_EN_START_SELECT, 0xB4);

	u8 first_spad_to_enable = spad_type_is_aperture ? 12 : 0; // 12 is the first aperture spad
	u8 spads_enabled = 0;

	for (u8 i = 0; i < 48; i++)
	{
		if (i < first_spad_to_enable || spads_enabled == spad_count)
		{
			// This bit is lower than the first one that should be enabled, or
			// (reference_spad_count) bits have already been enabled, so zero this bit
			ref_spad_map[i / 8] &= ~(1 << (i % 8));
		}
		else if ((ref_spad_map[i / 8] >> (i % 8)) & 0x1)
		{
			spads_enabled++;
		}
	}

	writeMulti(GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

	// -- VL53L0X_set_reference_spads() end

	// -- VL53L0X_load_tuning_settings() begin
	// DefaultTuningSettings from vl53l0x_tuning.h

	writeReg(0xFF, 0x01);
	writeReg(0x00, 0x00);

	writeReg(0xFF, 0x00);
	writeReg(0x09, 0x00);
	writeReg(0x10, 0x00);
	writeReg(0x11, 0x00);

	writeReg(0x24, 0x01);
	writeReg(0x25, 0xFF);
	writeReg(0x75, 0x00);

	writeReg(0xFF, 0x01);
	writeReg(0x4E, 0x2C);
	writeReg(0x48, 0x00);
	writeReg(0x30, 0x20);

	writeReg(0xFF, 0x00);
	writeReg(0x30, 0x09);
	writeReg(0x54, 0x00);
	writeReg(0x31, 0x04);
	writeReg(0x32, 0x03);
	writeReg(0x40, 0x83);
	writeReg(0x46, 0x25);
	writeReg(0x60, 0x00);
	writeReg(0x27, 0x00);
	writeReg(0x50, 0x06);
	writeReg(0x51, 0x00);
	writeReg(0x52, 0x96);
	writeReg(0x56, 0x08);
	writeReg(0x57, 0x30);
	writeReg(0x61, 0x00);
	writeReg(0x62, 0x00);
	writeReg(0x64, 0x00);
	writeReg(0x65, 0x00);
	writeReg(0x66, 0xA0);

	writeReg(0xFF, 0x01);
	writeReg(0x22, 0x32);
	writeReg(0x47, 0x14);
	writeReg(0x49, 0xFF);
	writeReg(0x4A, 0x00);

	writeReg(0xFF, 0x00);
	writeReg(0x7A, 0x0A);
	writeReg(0x7B, 0x00);
	writeReg(0x78, 0x21);

	writeReg(0xFF, 0x01);
	writeReg(0x23, 0x34);
	writeReg(0x42, 0x00);
	writeReg(0x44, 0xFF);
	writeReg(0x45, 0x26);
	writeReg(0x46, 0x05);
	writeReg(0x40, 0x40);
	writeReg(0x0E, 0x06);
	writeReg(0x20, 0x1A);
	writeReg(0x43, 0x40);

	writeReg(0xFF, 0x00);
	writeReg(0x34, 0x03);
	writeReg(0x35, 0x44);

	writeReg(0xFF, 0x01);
	writeReg(0x31, 0x04);
	writeReg(0x4B, 0x09);
	writeReg(0x4C, 0x05);
	writeReg(0x4D, 0x04);

	writeReg(0xFF, 0x00);
	writeReg(0x44, 0x00);
	writeReg(0x45, 0x20);
	writeReg(0x47, 0x08);
	writeReg(0x48, 0x28);
	writeReg(0x67, 0x00);
	writeReg(0x70, 0x04);
	writeReg(0x71, 0x01);
	writeReg(0x72, 0xFE);
	writeReg(0x76, 0x00);
	writeReg(0x77, 0x00);

	writeReg(0xFF, 0x01);
	writeReg(0x0D, 0x01);

	writeReg(0xFF, 0x00);
	writeReg(0x80, 0x01);
	writeReg(0x01, 0xF8);

	writeReg(0xFF, 0x01);
	writeReg(0x8E, 0x01);
	writeReg(0x00, 0x01);
	writeReg(0xFF, 0x00);
	writeReg(0x80, 0x00);

	// -- VL53L0X_load_tuning_settings() end

	// "Set interrupt config to new sample ready"
	// -- VL53L0X_SetGpioConfig() begin

	writeReg(SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04);
	writeReg(GPIO_HV_MUX_ACTIVE_HIGH, readReg(GPIO_HV_MUX_ACTIVE_HIGH) & ~0x10); // active low
	writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01);

	// -- VL53L0X_SetGpioConfig() end

	g_measTimBudUs = vl53l0x_getMeasurementTimingBudget();

	// "Disable MSRC and TCC by default"
	// MSRC = Minimum Signal Rate Check
	// TCC = Target CentreCheck
	// -- VL53L0X_SetSequenceStepEnable() begin

	writeReg(SYSTEM_SEQUENCE_CONFIG, 0xE8);

	// -- VL53L0X_SetSequenceStepEnable() end

	// "Recalculate timing budget"
	vl53l0x_setMeasurementTimingBudget(g_measTimBudUs);

	// VL53L0X_StaticInit() end

	// VL53L0X_PerformRefCalibration() begin (VL53L0X_perform_ref_calibration())

	// -- VL53L0X_perform_vhv_calibration() begin

	writeReg(SYSTEM_SEQUENCE_CONFIG, 0x01);
	if (!performSingleRefCalibration(0x40)) { return FALSE; }

	// -- VL53L0X_perform_vhv_calibration() end

	// -- VL53L0X_perform_phase_calibration() begin

	writeReg(SYSTEM_SEQUENCE_CONFIG, 0x02);
	if (!performSingleRefCalibration(0x00)) { return FALSE; }

	// -- VL53L0X_perform_phase_calibration() end

	// "restore the previous Sequence Config"
	writeReg(SYSTEM_SEQUENCE_CONFIG, 0xE8);

	// VL53L0X_PerformRefCalibration() end

	return TRUE;
}

// Set the return signal rate limit check value in units of MCPS (mega counts
// per second). "This represents the amplitude of the signal reflected from the
// target and detected by the device"; setting this limit presumably determines
// the minimum measurement necessary for the sensor to report a valid reading.
// Setting a lower limit increases the potential range of the sensor but also
// seems to increase the likelihood of getting an inaccurate reading because of
// unwanted reflections from objects other than the intended target.
// Defaults to 0.25 MCPS as initialized by the ST API and this library.
bool vl53l0x_setSignalRateLimit(float f_limit)
{
	if (f_limit < 0 || f_limit > 511.99) { return FALSE; }

	// Q9.7 fixed point format (9 integer bits, 7 fractional bits)
	writeReg16Bit(FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT, f_limit * (1 << 7));
	return TRUE;
}

// Get the return signal rate limit check value in MCPS
float vl53l0x_getSignalRateLimit(void)
{
	return (float)readReg16Bit(FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT) / (1 << 7);
}

// Set the measurement timing budget in microseconds, which is the time allowed
// for one measurement; the ST API and this library take care of splitting the
// timing budget among the sub-steps in the ranging sequence. A longer timing
// budget allows for more accurate measurements. Increasing the budget by a
// factor of N decreases the range measurement standard deviation by a factor of
// sqrt(N). Defaults to about 33 milliseconds; the minimum is 20 ms.
// based on VL53L0X_set_measurement_timing_budget_micro_seconds()
bool vl53l0x_setMeasurementTimingBudget(u32 u32_budget)
{
	sequenceStepEnables_t enables;
	sequenceStepTimeouts_t timeouts;

	u16 const StartOverhead      = 1320; // note that this is different than the value in get_
	u16 const EndOverhead        = 960;
	u16 const MsrcOverhead       = 660;
	u16 const TccOverhead        = 590;
	u16 const DssOverhead        = 690;
	u16 const PreRangeOverhead   = 660;
	u16 const FinalRangeOverhead = 550;

	u32 const MinTimingBudget = 20000;

	if (u32_budget < MinTimingBudget) { return FALSE; }

	u32 used_budget_us = StartOverhead + EndOverhead;

	getSequenceStepEnables(&enables);
	getSequenceStepTimeouts(&enables, &timeouts);

	if (enables.tcc)
	{
		used_budget_us += (timeouts.msrc_dss_tcc_us + TccOverhead);
	}

	if (enables.dss)
	{
		used_budget_us += 2 * (timeouts.msrc_dss_tcc_us + DssOverhead);
	}
	else if (enables.msrc)
	{
		used_budget_us += (timeouts.msrc_dss_tcc_us + MsrcOverhead);
	}

	if (enables.pre_range)
	{
		used_budget_us += (timeouts.pre_range_us + PreRangeOverhead);
	}

	if (enables.final_range)
	{
		used_budget_us += FinalRangeOverhead;

		// "Note that the final range timeout is determined by the timing
		// budget and the sum of all other timeouts within the sequence.
		// If there is no room for the final range timeout, then an error
		// will be set. Otherwise the remaining time will be applied to
		// the final range."

		if (used_budget_us > u32_budget)
		{
			// "Requested timeout too big."
			return FALSE;
		}

		u32 final_range_timeout_us = u32_budget - used_budget_us;

		// set_sequence_step_timeout() begin
		// (SequenceStepId == VL53L0X_SEQUENCESTEP_FINAL_RANGE)

		// "For the final range timeout, the pre-range timeout
		//  must be added. To do this both final and pre-range
		//  timeouts must be expressed in macro periods MClks
		//  because they have different vcsel periods."

		u16 final_range_timeout_mclks =
		timeoutMicrosecondsToMclks(final_range_timeout_us,
		timeouts.final_range_vcsel_period_pclks);

		if (enables.pre_range)
		{
			final_range_timeout_mclks += timeouts.pre_range_mclks;
		}

		writeReg16Bit(FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI,
		encodeTimeout(final_range_timeout_mclks));

		// set_sequence_step_timeout() end

		g_measTimBudUs = u32_budget; // store for internal reuse
	}
	return TRUE;
}

// Get the measurement timing budget in microseconds
// based on VL53L0X_get_measurement_timing_budget_micro_seconds()
// in us
u32 vl53l0x_getMeasurementTimingBudget(void)
{
	sequenceStepEnables_t enables;
	sequenceStepTimeouts_t timeouts;

	u16 const StartOverhead     = 1910; // note that this is different than the value in set_
	u16 const EndOverhead        = 960;
	u16 const MsrcOverhead       = 660;
	u16 const TccOverhead        = 590;
	u16 const DssOverhead        = 690;
	u16 const PreRangeOverhead   = 660;
	u16 const FinalRangeOverhead = 550;

	// "Start and end overhead times always present"
	u32 budget_us = StartOverhead + EndOverhead;

	getSequenceStepEnables(&enables);
	getSequenceStepTimeouts(&enables, &timeouts);

	if (enables.tcc)
	{
		budget_us += (timeouts.msrc_dss_tcc_us + TccOverhead);
	}

	if (enables.dss)
	{
		budget_us += 2 * (timeouts.msrc_dss_tcc_us + DssOverhead);
	}
	else if (enables.msrc)
	{
		budget_us += (timeouts.msrc_dss_tcc_us + MsrcOverhead);
	}

	if (enables.pre_range)
	{
		budget_us += (timeouts.pre_range_us + PreRangeOverhead);
	}

	if (enables.final_range)
	{
		budget_us += (timeouts.final_range_us + FinalRangeOverhead);
	}

	g_measTimBudUs = budget_us; // store for internal reuse
	return budget_us;
}

// Set the VCSEL (vertical cavity surface emitting laser) pulse period for the
// given period type (pre-range or final range) to the given value in PCLKs.
// Longer periods seem to increase the potential range of the sensor.
// Valid values are (even numbers only):
//  pre:  12 to 18 (initialized default: 14)
//  final: 8 to 14 (initialized default: 10)
// based on VL53L0X_set_vcsel_pulse_period()
bool vl53l0x_setVcselPulsePeriod(vl53l0x_vcselPeriodType_enum_t e_vcselPeriodType, u8 period_pclks)
{
	u8 vcsel_period_reg = encodeVcselPeriod(period_pclks);

	sequenceStepEnables_t enables;
	sequenceStepTimeouts_t timeouts;

	getSequenceStepEnables(&enables);
	getSequenceStepTimeouts(&enables, &timeouts);

	// "Apply specific settings for the requested clock period"
	// "Re-calculate and apply timeouts, in macro periods"

	// "When the VCSEL period for the pre or final range is changed,
	// the corresponding timeout must be read from the device using
	// the current VCSEL period, then the new VCSEL period can be
	// applied. The timeout then must be written back to the device
	// using the new VCSEL period.
	//
	// For the MSRC timeout, the same applies - this timeout being
	// dependant on the pre-range vcsel period."


	if (e_vcselPeriodType == VcselPeriodPreRange)
	{
		// "Set phase check limits"
		switch (period_pclks)
		{
			case 12:
			writeReg(PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x18);
			break;

			case 14:
			writeReg(PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x30);
			break;

			case 16:
			writeReg(PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x40);
			break;

			case 18:
			writeReg(PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x50);
			break;

			default:
			// invalid period
			return FALSE;
		}
		writeReg(PRE_RANGE_CONFIG_VALID_PHASE_LOW, 0x08);

		// apply new VCSEL period
		writeReg(PRE_RANGE_CONFIG_VCSEL_PERIOD, vcsel_period_reg);

		// update timeouts

		// set_sequence_step_timeout() begin
		// (SequenceStepId == VL53L0X_SEQUENCESTEP_PRE_RANGE)

		u16 new_pre_range_timeout_mclks =
		timeoutMicrosecondsToMclks(timeouts.pre_range_us, period_pclks);

		writeReg16Bit(PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI,
		encodeTimeout(new_pre_range_timeout_mclks));

		// set_sequence_step_timeout() end

		// set_sequence_step_timeout() begin
		// (SequenceStepId == VL53L0X_SEQUENCESTEP_MSRC)

		u16 new_msrc_timeout_mclks =
		timeoutMicrosecondsToMclks(timeouts.msrc_dss_tcc_us, period_pclks);

		writeReg(MSRC_CONFIG_TIMEOUT_MACROP,
		(new_msrc_timeout_mclks > 256) ? 255 : (new_msrc_timeout_mclks - 1));

		// set_sequence_step_timeout() end
	}
	else if (e_vcselPeriodType == VcselPeriodFinalRange)
	{
		switch (period_pclks)
		{
			case 8:
			writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x10);
			writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
			writeReg(GLOBAL_CONFIG_VCSEL_WIDTH, 0x02);
			writeReg(ALGO_PHASECAL_CONFIG_TIMEOUT, 0x0C);
			writeReg(0xFF, 0x01);
			writeReg(ALGO_PHASECAL_LIM, 0x30);
			writeReg(0xFF, 0x00);
			break;

			case 10:
			writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x28);
			writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
			writeReg(GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
			writeReg(ALGO_PHASECAL_CONFIG_TIMEOUT, 0x09);
			writeReg(0xFF, 0x01);
			writeReg(ALGO_PHASECAL_LIM, 0x20);
			writeReg(0xFF, 0x00);
			break;

			case 12:
			writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x38);
			writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
			writeReg(GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
			writeReg(ALGO_PHASECAL_CONFIG_TIMEOUT, 0x08);
			writeReg(0xFF, 0x01);
			writeReg(ALGO_PHASECAL_LIM, 0x20);
			writeReg(0xFF, 0x00);
			break;

			case 14:
			writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x48);
			writeReg(FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
			writeReg(GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
			writeReg(ALGO_PHASECAL_CONFIG_TIMEOUT, 0x07);
			writeReg(0xFF, 0x01);
			writeReg(ALGO_PHASECAL_LIM, 0x20);
			writeReg(0xFF, 0x00);
			break;

			default:
			// invalid period
			return FALSE;
		}

		// apply new VCSEL period
		writeReg(FINAL_RANGE_CONFIG_VCSEL_PERIOD, vcsel_period_reg);

		// update timeouts

		// set_sequence_step_timeout() begin
		// (SequenceStepId == VL53L0X_SEQUENCESTEP_FINAL_RANGE)

		// "For the final range timeout, the pre-range timeout
		//  must be added. To do this both final and pre-range
		//  timeouts must be expressed in macro periods MClks
		//  because they have different vcsel periods."

		u16 new_final_range_timeout_mclks =
		timeoutMicrosecondsToMclks(timeouts.final_range_us, period_pclks);

		if (enables.pre_range)
		{
			new_final_range_timeout_mclks += timeouts.pre_range_mclks;
		}

		writeReg16Bit(FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI,
		encodeTimeout(new_final_range_timeout_mclks));

		// set_sequence_step_timeout end
	}
	else
	{
		// invalid type
		return FALSE;
	}

	// "Finally, the timing budget must be re-applied"

	vl53l0x_setMeasurementTimingBudget(g_measTimBudUs);

	// "Perform the phase calibration. This is needed after changing on vcsel period."
	// VL53L0X_perform_phase_calibration() begin

	u8 sequence_config = readReg(SYSTEM_SEQUENCE_CONFIG);
	writeReg(SYSTEM_SEQUENCE_CONFIG, 0x02);
	performSingleRefCalibration(0x0);
	writeReg(SYSTEM_SEQUENCE_CONFIG, sequence_config);

	// VL53L0X_perform_phase_calibration() end

	return TRUE;
}

// Get the VCSEL pulse period in PCLKs for the given period type.
// based on VL53L0X_get_vcsel_pulse_period()
u8 vl53l0x_getVcselPulsePeriod(vl53l0x_vcselPeriodType_enum_t e_vcselPeriodType)
{
	if (e_vcselPeriodType == VcselPeriodPreRange)
	{
		return decodeVcselPeriod(readReg(PRE_RANGE_CONFIG_VCSEL_PERIOD));
	}
	else if (e_vcselPeriodType == VcselPeriodFinalRange)
	{
		return decodeVcselPeriod(readReg(FINAL_RANGE_CONFIG_VCSEL_PERIOD));
	}
	else { return 255; }
}

// Start continuous ranging measurements. If period_ms (optional) is 0 or not
// given, continuous back-to-back mode is used (the sensor takes measurements as
// often as possible); otherwise, continuous timed mode is used, with the given
// inter-measurement period in milliseconds determining how often the sensor
// takes a measurement.
// based on VL53L0X_StartMeasurement()
void vl53l0x_startContinuous(u32 u32_rangingPeriod)
{
	writeReg(0x80, 0x01);
	writeReg(0xFF, 0x01);
	writeReg(0x00, 0x00);
	writeReg(0x91, g_stopVariable);
	writeReg(0x00, 0x01);
	writeReg(0xFF, 0x00);
	writeReg(0x80, 0x00);

	if (u32_rangingPeriod != 0)
	{
		// continuous timed mode

		// VL53L0X_SetInterMeasurementPeriodMilliSeconds() begin

		u16 osc_calibrate_val = readReg16Bit(OSC_CALIBRATE_VAL);

		if (osc_calibrate_val != 0)
		{
			u32_rangingPeriod *= osc_calibrate_val;
		}

		writeReg32Bit(SYSTEM_INTERMEASUREMENT_PERIOD, u32_rangingPeriod);

		// VL53L0X_SetInterMeasurementPeriodMilliSeconds() end

		writeReg(SYSRANGE_START, 0x04); // VL53L0X_REG_SYSRANGE_MODE_TIMED
	}
	else
	{
		// continuous back-to-back mode
		writeReg(SYSRANGE_START, 0x02); // VL53L0X_REG_SYSRANGE_MODE_BACKTOBACK
	}
}

// Stop continuous measurements
// based on VL53L0X_StopMeasurement()
void vl53l0x_stopContinuous(void)
{
	writeReg(SYSRANGE_START, 0x01); // VL53L0X_REG_SYSRANGE_MODE_SINGLESHOT

	writeReg(0xFF, 0x01);
	writeReg(0x00, 0x00);
	writeReg(0x91, 0x00);
	writeReg(0x00, 0x01);
	writeReg(0xFF, 0x00);
}

// Returns a range reading in millimeters when continuous mode is active
// (readRangeSingleMillimeters() also calls this function after starting a
// single-shot range measurement)
// extraStats provides additional info for this measurment. Set to 0 if not needed.
u16 vl53l0x_readRangeContinuousMillimeters(void)
{
	u16 temp;
	if ((readReg(RESULT_INTERRUPT_STATUS) & 0x07) == 0)
	{
		temp = 0xFFFF;
	}
	else
	{
		temp = readReg16Bit(RESULT_RANGE_STATUS + 10);
		writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01);
	}
	
	return temp;
}

// Performs a single-shot range measurement and returns the reading in
// millimeters
// based on VL53L0X_PerformSingleRangingMeasurement()
// extraStats provides additional info for this measurment. Set to 0 if not needed.
u16 vl53l0x_readRangeSingleMillimeters(void) {
	writeReg(0x80, 0x01);
	writeReg(0xFF, 0x01);
	writeReg(0x00, 0x00);
	writeReg(0x91, g_stopVariable);
	writeReg(0x00, 0x01);
	writeReg(0xFF, 0x00);
	writeReg(0x80, 0x00);
	writeReg(SYSRANGE_START, 0x01);
	// "Wait until start bit has been cleared"
	startTimeout();
	while (readReg(SYSRANGE_START) & 0x01){
		if (checkTimeoutExpired()){
			g_isTimeout = TRUE;
			return 65535;
		}
	}
	
	u16 temp;
	startTimeout();
	while ((readReg(RESULT_INTERRUPT_STATUS) & 0x07) == 0) {
		if (checkTimeoutExpired())
		{
			g_isTimeout = TRUE;
			return 65535;
		}
	}
	temp = readReg16Bit(RESULT_RANGE_STATUS + 10);
	writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01);
	
	return temp;
}

// Did a timeout occur in one of the read functions since the last call to
// timeoutOccurred()?
bool vl53l0x_timeoutOccurred()
{
	bool tmp = g_isTimeout;
	g_isTimeout = FALSE;
	return tmp;
}

void vl53l0x_setTimeout(u16 u16_timeout){
	g_ioTimeout = u16_timeout;
}

u16 vl53l0x_getTimeout(void){
	return g_ioTimeout;
}

// Private Methods /////////////////////////////////////////////////////////////

// Get reference SPAD (single photon avalanche diode) count and type
// based on VL53L0X_get_info_from_device(),
// but only gets reference SPAD count and type
bool getSpadInfo(u8 * count, bool * type_is_aperture)
{
	u8 tmp;

	writeReg(0x80, 0x01);
	writeReg(0xFF, 0x01);
	writeReg(0x00, 0x00);

	writeReg(0xFF, 0x06);
	writeReg(0x83, readReg(0x83) | 0x04);
	writeReg(0xFF, 0x07);
	writeReg(0x81, 0x01);

	writeReg(0x80, 0x01);

	writeReg(0x94, 0x6b);
	writeReg(0x83, 0x00);
	startTimeout();
	while (readReg(0x83) == 0x00)
	{
		if (checkTimeoutExpired()) { return FALSE; }
	}
	writeReg(0x83, 0x01);
	tmp = readReg(0x92);

	*count = tmp & 0x7f;
	*type_is_aperture = (tmp >> 7) & 0x01;

	writeReg(0x81, 0x00);
	writeReg(0xFF, 0x06);
	writeReg(0x83, readReg(0x83)  & ~0x04);
	writeReg(0xFF, 0x01);
	writeReg(0x00, 0x01);

	writeReg(0xFF, 0x00);
	writeReg(0x80, 0x00);

	return TRUE;
}

// Get sequence step enables
// based on VL53L0X_GetSequenceStepEnables()
void getSequenceStepEnables(sequenceStepEnables_t * enables)
{
	u8 sequence_config = readReg(SYSTEM_SEQUENCE_CONFIG);

	enables->tcc          = (sequence_config >> 4) & 0x1;
	enables->dss          = (sequence_config >> 3) & 0x1;
	enables->msrc         = (sequence_config >> 2) & 0x1;
	enables->pre_range    = (sequence_config >> 6) & 0x1;
	enables->final_range  = (sequence_config >> 7) & 0x1;
}

// Get sequence step timeouts
// based on get_sequence_step_timeout(),
// but gets all timeouts instead of just the requested one, and also stores
// intermediate values
void getSequenceStepTimeouts(sequenceStepEnables_t const * enables, sequenceStepTimeouts_t * timeouts)
{
	timeouts->pre_range_vcsel_period_pclks = vl53l0x_getVcselPulsePeriod(VcselPeriodPreRange);

	timeouts->msrc_dss_tcc_mclks = readReg(MSRC_CONFIG_TIMEOUT_MACROP) + 1;
	timeouts->msrc_dss_tcc_us =
	timeoutMclksToMicroseconds(timeouts->msrc_dss_tcc_mclks,
	timeouts->pre_range_vcsel_period_pclks);

	timeouts->pre_range_mclks =
	decodeTimeout(readReg16Bit(PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI));
	timeouts->pre_range_us =
	timeoutMclksToMicroseconds(timeouts->pre_range_mclks,
	timeouts->pre_range_vcsel_period_pclks);

	timeouts->final_range_vcsel_period_pclks = vl53l0x_getVcselPulsePeriod(VcselPeriodFinalRange);

	timeouts->final_range_mclks =
	decodeTimeout(readReg16Bit(FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI));

	if (enables->pre_range)
	{
		timeouts->final_range_mclks -= timeouts->pre_range_mclks;
	}

	timeouts->final_range_us =
	timeoutMclksToMicroseconds(timeouts->final_range_mclks,
	timeouts->final_range_vcsel_period_pclks);
}

// Decode sequence step timeout in MCLKs from register value
// based on VL53L0X_decode_timeout()
// Note: the original function returned a u32, but the return value is
// always stored in a u16.
u16 decodeTimeout(u16 reg_val)
{
	// format: "(LSByte * 2^MSByte) + 1"
	return (u16)((reg_val & 0x00FF) <<
	(u16)((reg_val & 0xFF00) >> 8)) + 1;
}

// Encode sequence step timeout register value from timeout in MCLKs
// based on VL53L0X_encode_timeout()
// Note: the original function took a u16, but the argument passed to it
// is always a u16.
u16 encodeTimeout(u16 timeout_mclks)
{
	// format: "(LSByte * 2^MSByte) + 1"

	u32 ls_byte = 0;
	u16 ms_byte = 0;

	if (timeout_mclks > 0)
	{
		ls_byte = timeout_mclks - 1;

		while ((ls_byte & 0xFFFFFF00) > 0)
		{
			ls_byte >>= 1;
			ms_byte++;
		}

		return (ms_byte << 8) | (ls_byte & 0xFF);
	}
	else { return 0; }
}

// Convert sequence step timeout from MCLKs to microseconds with given VCSEL period in PCLKs
// based on VL53L0X_calc_timeout_us()
u32 timeoutMclksToMicroseconds(u16 timeout_period_mclks, u8 vcsel_period_pclks)
{
	u32 macro_period_ns = calcMacroPeriod(vcsel_period_pclks);

	return ((timeout_period_mclks * macro_period_ns) + (macro_period_ns / 2)) / 1000;
}

// Convert sequence step timeout from microseconds to MCLKs with given VCSEL period in PCLKs
// based on VL53L0X_calc_timeout_mclks()
u32 timeoutMicrosecondsToMclks(u32 timeout_period_us, u8 vcsel_period_pclks)
{
	u32 macro_period_ns = calcMacroPeriod(vcsel_period_pclks);

	return (((timeout_period_us * 1000) + (macro_period_ns / 2)) / macro_period_ns);
}


// based on VL53L0X_perform_single_ref_calibration()
bool performSingleRefCalibration(u8 vhv_init_byte)
{
	writeReg(SYSRANGE_START, 0x01 | vhv_init_byte); // VL53L0X_REG_SYSRANGE_MODE_START_STOP

	startTimeout();
	while ((readReg(RESULT_INTERRUPT_STATUS) & 0x07) == 0)
	{
		if (checkTimeoutExpired()) { return FALSE; }
	}

	writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01);

	writeReg(SYSRANGE_START, 0x00);

	return TRUE;
}