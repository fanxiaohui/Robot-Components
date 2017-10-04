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
void vl53l0x_start();
void vl53l0x_stop();
bool b_vl53l0x_testConnection();
u16 vl53l0x_getDistance();

#define ADDRESS_DEFAULT 0b0101001

#define VL53L0X_SYSRANGE_START									0x00

#define VL53L0X_SYSTEM_THRESH_HIGH								0x0C
#define VL53L0X_SYSTEM_THRESH_LOW								0x0E

#define VL53L0X_SYSTEM_SEQUENCE_CONFIG							0x01
#define VL53L0X_SYSTEM_RANGE_CONFIG								0x09
#define VL53L0X_SYSTEM_INTERMEASUREMENT_PERIOD					0x04

#define VL53L0X_SYSTEM_INTERRUPT_CONFIG_GPIO					0x0A

#define VL53L0X_GPIO_HV_MUX_ACTIVE_HIGH							0x84

#define VL53L0X_SYSTEM_INTERRUPT_CLEAR							0x0B

#define VL53L0X_RESULT_INTERRUPT_STATUS							0x13
#define VL53L0X_RESULT_RANGE_STATUS								0x14

#define VL53L0X_RESULT_CORE_AMBIENT_WINDOW_EVENTS_RTN			0xBC
#define VL53L0X_RESULT_CORE_RANGING_TOTAL_EVENTS_RTN			0xC0
#define VL53L0X_RESULT_CORE_AMBIENT_WINDOW_EVENTS_REF			0xD0
#define VL53L0X_RESULT_CORE_RANGING_TOTAL_EVENTS_REF			0xD4
#define VL53L0X_RESULT_PEAK_SIGNAL_RATE_REF						0xB6

#define VL53L0X_ALGO_PART_TO_PART_RANGE_OFFSET_MM				0x28

#define VL53L0X_I2C_SLAVE_DEVICE_ADDRESS						0x8A

#define VL53L0X_MSRC_CONFIG_CONTROL								0x60

#define VL53L0X_PRE_RANGE_CONFIG_MIN_SNR						0x27
#define VL53L0X_PRE_RANGE_CONFIG_VALID_PHASE_LOW				0x56
#define VL53L0X_PRE_RANGE_CONFIG_VALID_PHASE_HIGH				0x57
#define VL53L0X_PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT				0x64

#define VL53L0X_FINAL_RANGE_CONFIG_MIN_SNR						0x67
#define VL53L0X_FINAL_RANGE_CONFIG_VALID_PHASE_LOW				0x47
#define VL53L0X_FINAL_RANGE_CONFIG_VALID_PHASE_HIGH				0x48
#define VL53L0X_FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT		0x44

#define VL53L0X_PRE_RANGE_CONFIG_SIGMA_THRESH_HI				0x61
#define VL53L0X_PRE_RANGE_CONFIG_SIGMA_THRESH_LO				0x62

#define VL53L0X_PRE_RANGE_CONFIG_VCSEL_PERIOD					0x50
#define VL53L0X_PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI				0x51
#define VL53L0X_PRE_RANGE_CONFIG_TIMEOUT_MACROP_LO				0x52

#define VL53L0X_SYSTEM_HISTOGRAM_BIN							0x81
#define VL53L0X_HISTOGRAM_CONFIG_INITIAL_PHASE_SELECT			0x33
#define VL53L0X_HISTOGRAM_CONFIG_READOUT_CTRL					0x55

#define VL53L0X_FINAL_RANGE_CONFIG_VCSEL_PERIOD					0x70
#define VL53L0X_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI			0x71
#define VL53L0X_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_LO			0x72
#define VL53L0X_CROSSTALK_COMPENSATION_PEAK_RATE_MCPS			0x20

#define VL53L0X_MSRC_CONFIG_TIMEOUT_MACROP						0x46

#define VL53L0X_SOFT_RESET_GO2_SOFT_RESET_N						0xBF
#define VL53L0X_IDENTIFICATION_MODEL_ID							0xC0
#define VL53L0X_IDENTIFICATION_REVISION_ID						0xC2

#define VL53L0X_OSC_CALIBRATE_VAL								0xF8

#define VL53L0X_GLOBAL_CONFIG_VCSEL_WIDTH						0x32
#define VL53L0X_GLOBAL_CONFIG_SPAD_ENABLES_REF_0				0xB0
#define VL53L0X_GLOBAL_CONFIG_SPAD_ENABLES_REF_1				0xB1
#define VL53L0X_GLOBAL_CONFIG_SPAD_ENABLES_REF_2				0xB2
#define VL53L0X_GLOBAL_CONFIG_SPAD_ENABLES_REF_3				0xB3
#define VL53L0X_GLOBAL_CONFIG_SPAD_ENABLES_REF_4				0xB4
#define VL53L0X_GLOBAL_CONFIG_SPAD_ENABLES_REF_5				0xB5

#define VL53L0X_GLOBAL_CONFIG_REF_EN_START_SELECT				0xB6
#define VL53L0X_DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD				0x4E
#define VL53L0X_DYNAMIC_SPAD_REF_EN_START_OFFSET				0x4F
#define VL53L0X_POWER_MANAGEMENT_GO1_POWER_FORCE				0x80

#define VL53L0X_VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV				0x89

#define VL53L0X_ALGO_PHASECAL_LIM								0x30
#define VL53L0X_ALGO_PHASECAL_CONFIG_TIMEOUT					0x30

typedef enum vcselPeriodType
{
	VcselPeriodPreRange,
	VcselPeriodFinalRange
}vcselPeriodType;

void setAddress(u8 new_addr);
u8 getAddress(void);

void writeReg(u8 reg, u8 value);
void writeReg16Bit(u8 reg, u16 value);
void writeReg32Bit(u8 reg, u32 value);
u8 readReg(u8 reg);
u16 readReg16Bit(u8 reg);
u32 readReg32Bit(u8 reg);

void writeMulti(u8 reg, u8 const * src, u8 count);
void readMulti(u8 reg, u8 * dst, u8 count);

bool setSignalRateLimit(float limit_Mcps);
float getSignalRateLimit(void);

bool setMeasurementTimingBudget(u32 budget_us);
u32 getMeasurementTimingBudget(void);

bool setVcselPulsePeriod(vcselPeriodType type, u8 period_pclks);
u8 getVcselPulsePeriod(vcselPeriodType type);

void startContinuous(u32 period_ms);
void stopContinuous(void);
u16 readRangeContinuousMillimeters(void);
u16 readRangeSingleMillimeters(void);

void setTimeout(u16 timeout);
u16 getTimeout(void);
bool timeoutOccurred(void);

typedef struct SequenceStepEnables_t
{
	bool tcc, msrc, dss, pre_range, final_range;
}SequenceStepEnables_t;

typedef struct SequenceStepTimeouts_t
{
	u16 pre_range_vcsel_period_pclks, final_range_vcsel_period_pclks;

	u16 msrc_dss_tcc_mclks, pre_range_mclks, final_range_mclks;
	u32 msrc_dss_tcc_us,    pre_range_us,    final_range_us;
}SequenceStepTimeouts_t;

bool getSpadInfo(u8 * count, bool * type_is_aperture);

void getSequenceStepEnables(SequenceStepEnables_t * enables);
void getSequenceStepTimeouts(SequenceStepEnables_t const * enables, SequenceStepTimeouts_t * timeouts);

bool performSingleRefCalibration(u8 vhv_init_byte);

u16 decodeTimeout(u16 value);
u16 encodeTimeout(u16 timeout_mclks);
u32 timeoutMclksToMicroseconds(u16 timeout_period_mclks, u8 vcsel_period_pclks);
u32 timeoutMicrosecondsToMclks(u32 timeout_period_us, u8 vcsel_period_pclks);

#endif /* VL53L0X_H_ */