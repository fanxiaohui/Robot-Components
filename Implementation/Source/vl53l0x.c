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
/* Internal defines, enums, structs, types                              */
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
volatile u32 u32_milliseconds = 0;

/************************************************************************/
/* Internal functions                                                   */
/************************************************************************/

void startTimeout(vl53l0x_struct_t* ps_sensor)
{
	ps_sensor->timeoutStart = u32_milliseconds;
}

bool checkTimeoutExpired(vl53l0x_struct_t* ps_sensor)
{
	return ps_sensor->i2cTimeout > 0 && ((u32_milliseconds - ps_sensor->timeoutStart) > ps_sensor->i2cTimeout);
}

u8 decodeVcselPeriod(u8 reg_val)
{
	return ((reg_val) + 1) << 1;
}

u8 encodeVcselPeriod(u8 period_pclks)
{
	return ((period_pclks) >> 1) - 1;
}

u32 calcMacroPeriod(u8 vcsel_period_pclks)
{
	/* PLL_period_ps = 1655; macro_period_vclks = 2304 */
	return (((u32) 2304 * (vcsel_period_pclks) * 1655) + 500) / 1000;
}

void writeReg(vl53l0x_struct_t* ps_sensor, u8 reg, u8 value)
{
	i2c_sendStart( (ps_sensor->address << 1) | I2C_WRITE );
	i2c_write(reg);
	i2c_write(value);
	i2c_sendStop();
}

void writeReg16Bit(vl53l0x_struct_t* ps_sensor, u8 reg, u16 value)
{
	i2c_sendStart( (ps_sensor->address << 1) | I2C_WRITE );
	i2c_write(reg);
	i2c_write((value >> 8) & 0xFF);
	i2c_write((value     ) & 0xFF);
	i2c_sendStop();
}

void writeReg32Bit(vl53l0x_struct_t* ps_sensor, u8 reg, u32 value)
{
	i2c_sendStart( (ps_sensor->address << 1) | I2C_WRITE );
	i2c_write(reg);
	i2c_write((value >>24) & 0xFF);
	i2c_write((value >>16) & 0xFF);
	i2c_write((value >> 8) & 0xFF);
	i2c_write((value     ) & 0xFF);
	i2c_sendStop();
}

u8 readReg(vl53l0x_struct_t* ps_sensor, u8 reg)
{
	u8 value;
	i2c_sendStart( (ps_sensor->address << 1) | I2C_WRITE );
	i2c_write( reg );
	i2c_sendRepStart( (ps_sensor->address << 1) | I2C_READ );
	value = i2c_readNak();
	i2c_sendStop();
	return value;
}

u16 readReg16Bit(vl53l0x_struct_t* ps_sensor, u8 reg)
{
	u16 value;
	i2c_sendStart( (ps_sensor->address << 1) | I2C_WRITE );
	i2c_write( reg );
	i2c_sendRepStart( (ps_sensor->address << 1) | I2C_READ );
	value  = i2c_readAck() << 8;
	value |= i2c_readNak();
	i2c_sendStop();
	return value;
}

u32 readReg32Bit(vl53l0x_struct_t* ps_sensor, u8 reg)
{
	u32 value;
	i2c_sendStart( (ps_sensor->address << 1) | I2C_WRITE );
	i2c_write( reg );
	i2c_sendRepStart( (ps_sensor->address << 1) | I2C_READ );
	value  = (u32)i2c_readAck() <<24;
	value |= (u32)i2c_readAck() <<16;
	value |= (u32)i2c_readAck() << 8;
	value |= i2c_readNak();
	i2c_sendStop();
	return value;
}

void writeMulti(vl53l0x_struct_t* ps_sensor, u8 reg, u8 const *src, u8 count)
{
	i2c_sendStart( (ps_sensor->address << 1) | I2C_WRITE );
	i2c_write( reg );
	while ( count-- > 0 )
		i2c_write( *src++ );

	i2c_sendStop();
}

void readMulti(vl53l0x_struct_t* ps_sensor, u8 reg, u8 * dst, u8 count)
{
	i2c_sendStart( (ps_sensor->address << 1) | I2C_WRITE );
	i2c_write( reg );
	i2c_sendRepStart( (ps_sensor->address << 1) | I2C_READ );
	while ( count > 0 )
	{
		if ( count > 1 )
			*dst++ = i2c_readAck();
		else
			*dst++ = i2c_readNak();
		count--;
	}
	i2c_sendStop();
}

bool getSpadInfo(vl53l0x_struct_t* ps_sensor, u8 * count, bool * type_is_aperture)
{
	u8 tmp;

	writeReg(ps_sensor, 0x80, 0x01);
	writeReg(ps_sensor, 0xFF, 0x01);
	writeReg(ps_sensor, 0x00, 0x00);

	writeReg(ps_sensor, 0xFF, 0x06);
	writeReg(ps_sensor, 0x83, readReg(ps_sensor, 0x83) | 0x04);
	writeReg(ps_sensor, 0xFF, 0x07);
	writeReg(ps_sensor, 0x81, 0x01);

	writeReg(ps_sensor, 0x80, 0x01);

	writeReg(ps_sensor, 0x94, 0x6b);
	writeReg(ps_sensor, 0x83, 0x00);
	startTimeout(ps_sensor);
	while (readReg(ps_sensor, 0x83) == 0x00)
	{
		if (checkTimeoutExpired(ps_sensor)) { return FALSE; }
	}
	writeReg(ps_sensor, 0x83, 0x01);
	tmp = readReg(ps_sensor, 0x92);

	*count = tmp & 0x7f;
	*type_is_aperture = (tmp >> 7) & 0x01;

	writeReg(ps_sensor, 0x81, 0x00);
	writeReg(ps_sensor, 0xFF, 0x06);
	writeReg(ps_sensor, 0x83, readReg(ps_sensor, 0x83)  & ~0x04);
	writeReg(ps_sensor, 0xFF, 0x01);
	writeReg(ps_sensor, 0x00, 0x01);

	writeReg(ps_sensor, 0xFF, 0x00);
	writeReg(ps_sensor, 0x80, 0x00);

	return TRUE;
}

void getSequenceStepEnables(vl53l0x_struct_t* ps_sensor, sequenceStepEnables_t* enables)
{
	u8 sequence_config = readReg(ps_sensor, SYSTEM_SEQUENCE_CONFIG);

	enables->tcc          = (sequence_config >> 4) & 0x1;
	enables->dss          = (sequence_config >> 3) & 0x1;
	enables->msrc         = (sequence_config >> 2) & 0x1;
	enables->pre_range    = (sequence_config >> 6) & 0x1;
	enables->final_range  = (sequence_config >> 7) & 0x1;
}

u16 encodeTimeout(u16 timeout_mclks)
{
	/* Format: (LSByte * 2^MSByte) + 1 */

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
	else
	return 0;
}

u16 decodeTimeout(u16 reg_val)
{
	/* Format: (LSByte * 2^MSByte) + 1 */
	return (u16)((reg_val & 0x00FF) << (u16)((reg_val & 0xFF00) >> 8)) + 1;
}

u32 timeoutMclksToMicroseconds(u16 timeout_period_mclks, u8 vcsel_period_pclks)
{
	u32 macro_period_ns = calcMacroPeriod(vcsel_period_pclks);

	return ((timeout_period_mclks * macro_period_ns) + (macro_period_ns / 2)) / 1000;
}

u8 getVcselPulsePeriod(vl53l0x_struct_t* ps_sensor, vcselPeriodType_enum_t e_vcselPeriodType)
{
	if (e_vcselPeriodType == VcselPeriodPreRange)
		return decodeVcselPeriod(readReg(ps_sensor, PRE_RANGE_CONFIG_VCSEL_PERIOD));
	else if (e_vcselPeriodType == VcselPeriodFinalRange)
		return decodeVcselPeriod(readReg(ps_sensor, FINAL_RANGE_CONFIG_VCSEL_PERIOD));
	else
		return 0xff;
}

void getSequenceStepTimeouts(vl53l0x_struct_t* ps_sensor, sequenceStepEnables_t const * enables, sequenceStepTimeouts_t * timeouts)
{
	timeouts->pre_range_vcsel_period_pclks = getVcselPulsePeriod(ps_sensor, VcselPeriodPreRange);

	timeouts->msrc_dss_tcc_mclks = readReg(ps_sensor, MSRC_CONFIG_TIMEOUT_MACROP) + 1;
	timeouts->msrc_dss_tcc_us =
	timeoutMclksToMicroseconds(timeouts->msrc_dss_tcc_mclks,
	timeouts->pre_range_vcsel_period_pclks);

	timeouts->pre_range_mclks =
	decodeTimeout(readReg16Bit(ps_sensor, PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI));
	timeouts->pre_range_us =
	timeoutMclksToMicroseconds(timeouts->pre_range_mclks,
	timeouts->pre_range_vcsel_period_pclks);

	timeouts->final_range_vcsel_period_pclks = getVcselPulsePeriod(ps_sensor, VcselPeriodFinalRange);

	timeouts->final_range_mclks =
	decodeTimeout(readReg16Bit(ps_sensor, FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI));

	if (enables->pre_range)
	timeouts->final_range_mclks -= timeouts->pre_range_mclks;

	timeouts->final_range_us =
	timeoutMclksToMicroseconds(timeouts->final_range_mclks,
	timeouts->final_range_vcsel_period_pclks);
}

u32 timeoutMicrosecondsToMclks(u32 timeout_period_us, u8 vcsel_period_pclks)
{
	u32 macro_period_ns = calcMacroPeriod(vcsel_period_pclks);

	return (((timeout_period_us * 1000) + (macro_period_ns / 2)) / macro_period_ns);
}

bool performSingleRefCalibration(vl53l0x_struct_t* ps_sensor, u8 vhv_init_byte)
{
	writeReg(ps_sensor, SYSRANGE_START, 0x01 | vhv_init_byte);

	startTimeout(ps_sensor);
	while ((readReg(ps_sensor, RESULT_INTERRUPT_STATUS) & 0x07) == 0)
	if (checkTimeoutExpired(ps_sensor))
	return FALSE;

	writeReg(ps_sensor, SYSTEM_INTERRUPT_CLEAR, 0x01);
	writeReg(ps_sensor, SYSRANGE_START, 0x00);

	return TRUE;
}

bool setVcselPulsePeriod(vl53l0x_struct_t* ps_sensor, vcselPeriodType_enum_t e_vcselPeriodType, u8 period_pclks)
{
	u8 vcsel_period_reg = encodeVcselPeriod(period_pclks);

	sequenceStepEnables_t enables;
	sequenceStepTimeouts_t timeouts;

	getSequenceStepEnables(ps_sensor, &enables);
	getSequenceStepTimeouts(ps_sensor, &enables, &timeouts);

	/* Apply specific settings for the requested clock period. Re-calculate and apply timeouts, in macro periods */
	/* When the VCSEL period for the pre or final range is changed, the corresponding timeout must be read from the device using the current VCSEL period, then the new VCSEL period can be applied. The timeout then must be written back to the device using the new VCSEL period. For the MSRC timeout, the same applies - this timeout being dependent on the pre-range vcsel period. */
	if (e_vcselPeriodType == VcselPeriodPreRange)
	{
		/* Set phase check limits */
		switch (period_pclks)
		{
			case 12:
			writeReg(ps_sensor, PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x18);
			break;

			case 14:
			writeReg(ps_sensor, PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x30);
			break;

			case 16:
			writeReg(ps_sensor, PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x40);
			break;

			case 18:
			writeReg(ps_sensor, PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x50);
			break;

			default:
			return FALSE;
		}
		writeReg(ps_sensor, PRE_RANGE_CONFIG_VALID_PHASE_LOW, 0x08);

		/* Apply new VCSEL period */
		writeReg(ps_sensor, PRE_RANGE_CONFIG_VCSEL_PERIOD, vcsel_period_reg);

		/* Update timeouts */
		u16 new_pre_range_timeout_mclks = timeoutMicrosecondsToMclks(timeouts.pre_range_us, period_pclks);
		writeReg16Bit(ps_sensor, PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI, encodeTimeout(new_pre_range_timeout_mclks));
		u16 new_msrc_timeout_mclks = timeoutMicrosecondsToMclks(timeouts.msrc_dss_tcc_us, period_pclks);
		writeReg(ps_sensor, MSRC_CONFIG_TIMEOUT_MACROP, (new_msrc_timeout_mclks > 256) ? 255 : (new_msrc_timeout_mclks - 1));
	}
	else if (e_vcselPeriodType == VcselPeriodFinalRange)
	{
		switch (period_pclks)
		{
			case 8:
			writeReg(ps_sensor, FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x10);
			writeReg(ps_sensor, FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
			writeReg(ps_sensor, GLOBAL_CONFIG_VCSEL_WIDTH, 0x02);
			writeReg(ps_sensor, ALGO_PHASECAL_CONFIG_TIMEOUT, 0x0C);
			writeReg(ps_sensor, 0xFF, 0x01);
			writeReg(ps_sensor, ALGO_PHASECAL_LIM, 0x30);
			writeReg(ps_sensor, 0xFF, 0x00);
			break;

			case 10:
			writeReg(ps_sensor, FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x28);
			writeReg(ps_sensor, FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
			writeReg(ps_sensor, GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
			writeReg(ps_sensor, ALGO_PHASECAL_CONFIG_TIMEOUT, 0x09);
			writeReg(ps_sensor, 0xFF, 0x01);
			writeReg(ps_sensor, ALGO_PHASECAL_LIM, 0x20);
			writeReg(ps_sensor, 0xFF, 0x00);
			break;

			case 12:
			writeReg(ps_sensor, FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x38);
			writeReg(ps_sensor, FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
			writeReg(ps_sensor, GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
			writeReg(ps_sensor, ALGO_PHASECAL_CONFIG_TIMEOUT, 0x08);
			writeReg(ps_sensor, 0xFF, 0x01);
			writeReg(ps_sensor, ALGO_PHASECAL_LIM, 0x20);
			writeReg(ps_sensor, 0xFF, 0x00);
			break;

			case 14:
			writeReg(ps_sensor, FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x48);
			writeReg(ps_sensor, FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
			writeReg(ps_sensor, GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
			writeReg(ps_sensor, ALGO_PHASECAL_CONFIG_TIMEOUT, 0x07);
			writeReg(ps_sensor, 0xFF, 0x01);
			writeReg(ps_sensor, ALGO_PHASECAL_LIM, 0x20);
			writeReg(ps_sensor, 0xFF, 0x00);
			break;

			default:
			return FALSE;
		}

		/* Apply new VCSEL period */
		writeReg(ps_sensor, FINAL_RANGE_CONFIG_VCSEL_PERIOD, vcsel_period_reg);

		/* For the final range timeout, the pre-range timeout must be added. To do this both final and pre-range timeouts must be expressed in macro periods MClks because they have different vcsel periods. */
		u16 new_final_range_timeout_mclks = timeoutMicrosecondsToMclks(timeouts.final_range_us, period_pclks);

		if (enables.pre_range)
		new_final_range_timeout_mclks += timeouts.pre_range_mclks;

		writeReg16Bit(ps_sensor, FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI, encodeTimeout(new_final_range_timeout_mclks));
	}
	else
	return FALSE;

	/* Perform the phase calibration. This is needed after changing on VCSEL period. */
	u8 sequence_config = readReg(ps_sensor, SYSTEM_SEQUENCE_CONFIG);
	writeReg(ps_sensor, SYSTEM_SEQUENCE_CONFIG, 0x02);
	performSingleRefCalibration(ps_sensor, 0x0);
	writeReg(ps_sensor, SYSTEM_SEQUENCE_CONFIG, sequence_config);

	return TRUE;
}

bool setSignalRateLimit(vl53l0x_struct_t* ps_sensor, float f_limit)
{
	if (f_limit < 0 || f_limit > 511.99) { return FALSE; }

	/* Q9.7 fixed point format (9 integer bits, 7 fractional bits) */
	writeReg16Bit(ps_sensor, FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT, f_limit * (1 << 7));
	return TRUE;
}

bool setMeasurementTimingBudget(vl53l0x_struct_t* ps_sensor, u32 u32_budget)
{
	sequenceStepEnables_t enables;
	sequenceStepTimeouts_t timeouts;

	u16 const StartOverhead      = 1320;
	u16 const EndOverhead        = 960;
	u16 const MsrcOverhead       = 660;
	u16 const TccOverhead        = 590;
	u16 const DssOverhead        = 690;
	u16 const PreRangeOverhead   = 660;
	u16 const FinalRangeOverhead = 550;

	u32 const MinTimingBudget = 20000;

	if (u32_budget < MinTimingBudget) { return FALSE; }

	u32 used_budget_us = StartOverhead + EndOverhead;

	getSequenceStepEnables(ps_sensor, &enables);
	getSequenceStepTimeouts(ps_sensor, &enables, &timeouts);

	if (enables.tcc)
	used_budget_us += (timeouts.msrc_dss_tcc_us + TccOverhead);

	if (enables.dss)
	used_budget_us += 2 * (timeouts.msrc_dss_tcc_us + DssOverhead);
	else if (enables.msrc)
	used_budget_us += (timeouts.msrc_dss_tcc_us + MsrcOverhead);

	if (enables.pre_range)
	used_budget_us += (timeouts.pre_range_us + PreRangeOverhead);

	if (enables.final_range)
	{
		used_budget_us += FinalRangeOverhead;

		/* Note that the final range timeout is determined by the timing budget and the sum of all other timeouts within the sequence. If there is no room for the final range timeout, then an error will be set. Otherwise the remaining time will be applied to the final range. */
		if (used_budget_us > u32_budget)
		return FALSE;

		u32 final_range_timeout_us = u32_budget - used_budget_us;

		/* For the final range timeout, the pre-range timeout must be added. To do this both final and pre-range timeouts must be expressed in macro periods MClks because they have different vcsel periods. */
		u16 final_range_timeout_mclks = timeoutMicrosecondsToMclks(final_range_timeout_us, timeouts.final_range_vcsel_period_pclks);

		if (enables.pre_range)
		final_range_timeout_mclks += timeouts.pre_range_mclks;

		writeReg16Bit(ps_sensor, FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI,
		encodeTimeout(final_range_timeout_mclks));
	}
	return TRUE;
}

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void vl53l0x_init(vl53l0x_struct_t* ps_sensor)
{

	s_i2cInterface.frequency = 400000;
	i2c_init(s_i2cInterface);
	i2c_start();

	ps_sensor->i2cTimeout = 0;
	ps_sensor->timedOut = FALSE;

	gpio_init(ps_sensor->xshutPin);
	gpio_setDirectionOutput(&ps_sensor->xshutPin);
	gpio_out_set(ps_sensor->xshutPin);
}

bool vl53l0x_start(vl53l0x_struct_t* ps_sensor)
{
	writeReg(ps_sensor, VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV, readReg(ps_sensor, VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV) | 0x01);

	/* Set I2C standard mode */
	writeReg(ps_sensor, 0x88, 0x00);

	writeReg(ps_sensor, 0x80, 0x01);
	writeReg(ps_sensor, 0xFF, 0x01);
	writeReg(ps_sensor, 0x00, 0x00);
	ps_sensor->stopVariable = readReg(ps_sensor, 0x91);
	writeReg(ps_sensor, 0x00, 0x01);
	writeReg(ps_sensor, 0xFF, 0x00);
	writeReg(ps_sensor, 0x80, 0x00);

	/* Disable SIGNAL_RATE_MSRC (bit 1) and SIGNAL_RATE_PRE_RANGE (bit 4) limit checks */
	writeReg(ps_sensor, MSRC_CONFIG_CONTROL, readReg(ps_sensor, MSRC_CONFIG_CONTROL) | 0x12);

	/* Set final range signal rate limit to 0.25 MCPS (million counts per second) */
	setSignalRateLimit(ps_sensor, 0.25);

	writeReg(ps_sensor, SYSTEM_SEQUENCE_CONFIG, 0xFF);

	u8 spad_count;
	bool spad_type_is_aperture;
	if (!getSpadInfo(ps_sensor, &spad_count, &spad_type_is_aperture)) { return FALSE; }

	/* Read SPAD map */
	u8 ref_spad_map[6];
	readMulti(ps_sensor, GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

	writeReg(ps_sensor, 0xFF, 0x01);
	writeReg(ps_sensor, DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00);
	writeReg(ps_sensor, DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C);
	writeReg(ps_sensor, 0xFF, 0x00);
	writeReg(ps_sensor, GLOBAL_CONFIG_REF_EN_START_SELECT, 0xB4);

	u8 first_spad_to_enable = spad_type_is_aperture ? 12 : 0;
	u8 spads_enabled = 0;

	for (u8 i = 0; i < 48; i++)
	{
		if (i < first_spad_to_enable || spads_enabled == spad_count)
			/* This bit is lower than the first one that should be enabled, or (reference_spad_count) bits have already been enabled, so zero this bit */
			ref_spad_map[i / 8] &= ~(1 << (i % 8));
		else if ((ref_spad_map[i / 8] >> (i % 8)) & 0x1)
			spads_enabled++;
	}

	writeMulti(ps_sensor, GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

	/* Default tuning settings */
	writeReg(ps_sensor, 0xFF, 0x01);
	writeReg(ps_sensor, 0x00, 0x00);

	writeReg(ps_sensor, 0xFF, 0x00);
	writeReg(ps_sensor, 0x09, 0x00);
	writeReg(ps_sensor, 0x10, 0x00);
	writeReg(ps_sensor, 0x11, 0x00);

	writeReg(ps_sensor, 0x24, 0x01);
	writeReg(ps_sensor, 0x25, 0xFF);
	writeReg(ps_sensor, 0x75, 0x00);

	writeReg(ps_sensor, 0xFF, 0x01);
	writeReg(ps_sensor, 0x4E, 0x2C);
	writeReg(ps_sensor, 0x48, 0x00);
	writeReg(ps_sensor, 0x30, 0x20);

	writeReg(ps_sensor, 0xFF, 0x00);
	writeReg(ps_sensor, 0x30, 0x09);
	writeReg(ps_sensor, 0x54, 0x00);
	writeReg(ps_sensor, 0x31, 0x04);
	writeReg(ps_sensor, 0x32, 0x03);
	writeReg(ps_sensor, 0x40, 0x83);
	writeReg(ps_sensor, 0x46, 0x25);
	writeReg(ps_sensor, 0x60, 0x00);
	writeReg(ps_sensor, 0x27, 0x00);
	writeReg(ps_sensor, 0x50, 0x06);
	writeReg(ps_sensor, 0x51, 0x00);
	writeReg(ps_sensor, 0x52, 0x96);
	writeReg(ps_sensor, 0x56, 0x08);
	writeReg(ps_sensor, 0x57, 0x30);
	writeReg(ps_sensor, 0x61, 0x00);
	writeReg(ps_sensor, 0x62, 0x00);
	writeReg(ps_sensor, 0x64, 0x00);
	writeReg(ps_sensor, 0x65, 0x00);
	writeReg(ps_sensor, 0x66, 0xA0);

	writeReg(ps_sensor, 0xFF, 0x01);
	writeReg(ps_sensor, 0x22, 0x32);
	writeReg(ps_sensor, 0x47, 0x14);
	writeReg(ps_sensor, 0x49, 0xFF);
	writeReg(ps_sensor, 0x4A, 0x00);

	writeReg(ps_sensor, 0xFF, 0x00);
	writeReg(ps_sensor, 0x7A, 0x0A);
	writeReg(ps_sensor, 0x7B, 0x00);
	writeReg(ps_sensor, 0x78, 0x21);

	writeReg(ps_sensor, 0xFF, 0x01);
	writeReg(ps_sensor, 0x23, 0x34);
	writeReg(ps_sensor, 0x42, 0x00);
	writeReg(ps_sensor, 0x44, 0xFF);
	writeReg(ps_sensor, 0x45, 0x26);
	writeReg(ps_sensor, 0x46, 0x05);
	writeReg(ps_sensor, 0x40, 0x40);
	writeReg(ps_sensor, 0x0E, 0x06);
	writeReg(ps_sensor, 0x20, 0x1A);
	writeReg(ps_sensor, 0x43, 0x40);

	writeReg(ps_sensor, 0xFF, 0x00);
	writeReg(ps_sensor, 0x34, 0x03);
	writeReg(ps_sensor, 0x35, 0x44);

	writeReg(ps_sensor, 0xFF, 0x01);
	writeReg(ps_sensor, 0x31, 0x04);
	writeReg(ps_sensor, 0x4B, 0x09);
	writeReg(ps_sensor, 0x4C, 0x05);
	writeReg(ps_sensor, 0x4D, 0x04);

	writeReg(ps_sensor, 0xFF, 0x00);
	writeReg(ps_sensor, 0x44, 0x00);
	writeReg(ps_sensor, 0x45, 0x20);
	writeReg(ps_sensor, 0x47, 0x08);
	writeReg(ps_sensor, 0x48, 0x28);
	writeReg(ps_sensor, 0x67, 0x00);
	writeReg(ps_sensor, 0x70, 0x04);
	writeReg(ps_sensor, 0x71, 0x01);
	writeReg(ps_sensor, 0x72, 0xFE);
	writeReg(ps_sensor, 0x76, 0x00);
	writeReg(ps_sensor, 0x77, 0x00);

	writeReg(ps_sensor, 0xFF, 0x01);
	writeReg(ps_sensor, 0x0D, 0x01);

	writeReg(ps_sensor, 0xFF, 0x00);
	writeReg(ps_sensor, 0x80, 0x01);
	writeReg(ps_sensor, 0x01, 0xF8);

	writeReg(ps_sensor, 0xFF, 0x01);
	writeReg(ps_sensor, 0x8E, 0x01);
	writeReg(ps_sensor, 0x00, 0x01);
	writeReg(ps_sensor, 0xFF, 0x00);
	writeReg(ps_sensor, 0x80, 0x00);

	/* Set interrupt config to new sample ready */
	writeReg(ps_sensor, SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04);
	writeReg(ps_sensor, GPIO_HV_MUX_ACTIVE_HIGH, readReg(ps_sensor, GPIO_HV_MUX_ACTIVE_HIGH) & ~0x10); // active low
	writeReg(ps_sensor, SYSTEM_INTERRUPT_CLEAR, 0x01);

	/* Disable Minimum Signal Rate Check and Target CentreCheck by default */
	writeReg(ps_sensor, SYSTEM_SEQUENCE_CONFIG, 0xE8);

	/* Set default timing budget */
	setMeasurementTimingBudget(ps_sensor, 30);

	/* perform calibrations */
	writeReg(ps_sensor, SYSTEM_SEQUENCE_CONFIG, 0x01);
	if (!performSingleRefCalibration(ps_sensor, 0x40)) { return FALSE; }
	writeReg(ps_sensor, SYSTEM_SEQUENCE_CONFIG, 0x02);
	if (!performSingleRefCalibration(ps_sensor, 0x00)) { return FALSE; }

	/* Restore the previous Sequence Config */
	writeReg(ps_sensor, SYSTEM_SEQUENCE_CONFIG, 0xE8);

	return TRUE;
}

void vl53l0x_stop(vl53l0x_struct_t* ps_sensor)
{
	gpio_out_reset(ps_sensor->xshutPin);
}

void vl53l0x_setAddress(vl53l0x_struct_t* ps_sensor, u8 u8_address)
{
	writeReg(ps_sensor, I2C_SLAVE_DEVICE_ADDRESS, u8_address & 0x7F );
	ps_sensor->address = u8_address;
}

bool vl53l0x_setMode(vl53l0x_struct_t* ps_sensor, vl53l0x_mode_enum_t e_mode)
{
	bool result = FALSE;

	switch (e_mode)
	{
		case VL53L0X_DEFAULT:
			result = setSignalRateLimit(ps_sensor, 0.25);
			if (result == TRUE)
			{
				/* Not working! */
				/*result = vl53l0x_setVcselPulsePeriod(ps_sensor, VcselPeriodPreRange, 14);
				if (result == TRUE)
				{
					result = vl53l0x_setVcselPulsePeriod(ps_sensor, VcselPeriodFinalRange, 10);
					if (result == TRUE)
					{*/
						result = setMeasurementTimingBudget(ps_sensor, 30 * 1000UL);
				/*	}
				}*/
			}
			break;
		case VL53L0X_MAX_ACCURACY:
			result = setSignalRateLimit(ps_sensor, 0.25);
			if (result == TRUE)
			{
				/* Not working! */
				/*result = vl53l0x_setVcselPulsePeriod(ps_sensor, VcselPeriodPreRange, 14);
				if (result == TRUE)
				{
					result = vl53l0x_setVcselPulsePeriod(ps_sensor, VcselPeriodFinalRange, 10);
					if (result == TRUE)
					{*/
						result = setMeasurementTimingBudget(ps_sensor, 200 * 1000UL);
				/*	}
				}*/
			}
			break;
		case VL53L0X_MAX_RANGE:
			result = setSignalRateLimit(ps_sensor, 0.1);
			if (result == TRUE)
			{
				/* Not working! */
				/*result = vl53l0x_setVcselPulsePeriod(ps_sensor, VcselPeriodPreRange, 18);
				if (result == TRUE)
				{
					result = vl53l0x_setVcselPulsePeriod(ps_sensor, VcselPeriodFinalRange, 14);
					if (result == TRUE)
					{*/
						result = setMeasurementTimingBudget(ps_sensor, 33 * 1000UL);
				/*	}
				}*/
			}
			break;
		case VL53L0X_MAX_SPEED:
			result = setSignalRateLimit(ps_sensor, 0.25);
			if (result == TRUE)
			{
				/* Not working! */
				/*result = vl53l0x_setVcselPulsePeriod(ps_sensor, VcselPeriodPreRange, 14);
				if (result == TRUE)
				{
					result = vl53l0x_setVcselPulsePeriod(ps_sensor, VcselPeriodFinalRange, 10);
					if (result == TRUE)
					{*/
						result = setMeasurementTimingBudget(ps_sensor, 20 * 1000UL);
				/*	}
				}*/
			}
			break;
	}

	return result;
}

void vl53l0x_startContinuous(vl53l0x_struct_t* ps_sensor, u32 u32_rangingPeriod)
{
	writeReg(ps_sensor, 0x80, 0x01);
	writeReg(ps_sensor, 0xFF, 0x01);
	writeReg(ps_sensor, 0x00, 0x00);
	writeReg(ps_sensor, 0x91, ps_sensor->stopVariable);
	writeReg(ps_sensor, 0x00, 0x01);
	writeReg(ps_sensor, 0xFF, 0x00);
	writeReg(ps_sensor, 0x80, 0x00);

	if (u32_rangingPeriod != 0)
	{
		/* Continuous timed mode */

		u16 osc_calibrate_val = readReg16Bit(ps_sensor, OSC_CALIBRATE_VAL);

		if (osc_calibrate_val != 0)
			u32_rangingPeriod *= osc_calibrate_val;

		writeReg32Bit(ps_sensor, SYSTEM_INTERMEASUREMENT_PERIOD, u32_rangingPeriod);
		writeReg(ps_sensor, SYSRANGE_START, 0x04);
	}
	else
	{
		/* Continuous back-to-back mode */
		writeReg(ps_sensor, SYSRANGE_START, 0x02);
	}
}

void vl53l0x_stopContinuous(vl53l0x_struct_t* ps_sensor)
{
	writeReg(ps_sensor, SYSRANGE_START, 0x01);
	writeReg(ps_sensor, 0xFF, 0x01);
	writeReg(ps_sensor, 0x00, 0x00);
	writeReg(ps_sensor, 0x91, 0x00);
	writeReg(ps_sensor, 0x00, 0x01);
	writeReg(ps_sensor, 0xFF, 0x00);
}

u16 vl53l0x_readRangeContinuous(vl53l0x_struct_t* ps_sensor)
{
	u16 temp;
	if ((readReg(ps_sensor, RESULT_INTERRUPT_STATUS) & 0x07) == 0)
	{
		temp = 0xFFFF;
	}
	else
	{
		temp = readReg16Bit(ps_sensor, RESULT_RANGE_STATUS + 10);
		writeReg(ps_sensor, SYSTEM_INTERRUPT_CLEAR, 0x01);
	}

	return temp;
}

u16 vl53l0x_readRangeSingle(vl53l0x_struct_t* ps_sensor)
{
	u16 temp;

	writeReg(ps_sensor, 0x80, 0x01);
	writeReg(ps_sensor, 0xFF, 0x01);
	writeReg(ps_sensor, 0x00, 0x00);
	writeReg(ps_sensor, 0x91, ps_sensor->stopVariable);
	writeReg(ps_sensor, 0x00, 0x01);
	writeReg(ps_sensor, 0xFF, 0x00);
	writeReg(ps_sensor, 0x80, 0x00);
	writeReg(ps_sensor, SYSRANGE_START, 0x01);
	/*Wait until start bit has been cleared */
	startTimeout(ps_sensor);
	while (readReg(ps_sensor, SYSRANGE_START) & 0x01)
		if (checkTimeoutExpired(ps_sensor))
		{
			ps_sensor->timedOut = TRUE;
			return 0xffff;
		}

	startTimeout(ps_sensor);
	while ((readReg(ps_sensor, RESULT_INTERRUPT_STATUS) & 0x07) == 0)
		if (checkTimeoutExpired(ps_sensor))
		{
			ps_sensor->timedOut = TRUE;
			return 0xffff;
		}

	temp = readReg16Bit(ps_sensor, RESULT_RANGE_STATUS + 10);
	writeReg(ps_sensor, SYSTEM_INTERRUPT_CLEAR, 0x01);

	return temp;
}

void vl53l0x_incrementTimeoutCounter()
{
	u32_milliseconds++;
}

bool vl53l0x_timeoutOccurred(vl53l0x_struct_t* ps_sensor)
{
	bool tmp = ps_sensor->timedOut;
	ps_sensor->timedOut = FALSE;
	return tmp;
}
