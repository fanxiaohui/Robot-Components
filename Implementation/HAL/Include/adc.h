/**	@file		adc.h
	@brief		ADC features
	@author		Adrian Grosu
	@version	0.1
	@date		3.09.2017
	@details	Supports the successive approximations ADC 
				Basic flow:
				1. Initialize a @link adc_struct_t @endlink with the resolution (@link adc_struct_t.resolution @endlink), reference voltage (@link adc_struct_t.referenceVoltage @endlink), conversion mode (@link adc_struct_t.conversionMode @endlink), clock prescaler value (@link adc_struct_t.prescaler @endlink)and the channels to be used (@link adc_struct_t.channelEnabled[] @endlink).
				2. Pass it to @link adc_init @endlink.
				3. Call @link adc_start @endlink.
				4. Call @link adc_read @endlink to read the values from the selected ADC channels.
				- Attach a function to the ADC interrupt handler with @link adc_attachInterrupt @endlink prior to calling @link adc_enableInterrupt @endlink.
				- To stop the ADC call @link adc_stop @endlink.
*/


#ifndef ADC_H_
#define ADC_H_

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/
#include "types.h"

/** @example adc_config.h
	This is an example of how to configure the ADC driver
*/
#include "adc_config.h"

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/** Specifies the conversion result resolution.
*/
typedef enum adc_resolution_enum_t
{
/** 8 bit resolution. Uses the 8 MSB from the result*/
	_8BIT_RES = 1,
/** 10 bit resolution. Uses all the bits from the result */
	_10BIT_RES = 0
}adc_resolution_enum_t;

/** Specifies the voltage reference the ADC uses. Check the microcontroller documentation to see how/when to use a different source.
*/
typedef enum adc_reference_enum_t
{
/**	External reference at analog pin */
	AREF			= 0b00,
/**	Supply voltage */
	AVCC			= 0b01,
/**	Internal 1.1V reference */
	INTERNAL_1_1V	= 0b10,
/**	Internal 2.56V reference */
	INTERNAL_2_56V	= 0b11
}adc_reference_enum_t;

/** Specifies the division factor for the ADC clock
*/
typedef enum adc_prescaler_enum_t
{
/** 1:2 division factor from IO clock */
	ADC_PRESCALER_2	 = 0b001,
/** 1:4 division factor from IO clock */
	ADC_PRESCALER_4	 = 0b010,
/** 1:8 division factor from IO clock */
	ADC_PRESCALER_8 	 = 0b011,
/** 1:16 division factor from IO clock */
	ADC_PRESCALER_16  = 0b100,
/** 1:32 division factor from IO clock */
	ADC_PRESCALER_32  = 0b101,
/** 1:64 division factor from IO clock */
	ADC_PRESCALER_64	 = 0b110,
/** 1:128 division factor from IO clock */
	ADC_PRESCALER_128 = 0b111
}adc_prescaler_enum_t;

/** Specifies the conversion mode
*/
typedef enum adc_conversionMode_enum_t
{
/**	One conversion */
	SINGLE_CONVERSION = 0xff,
/**	Continuous conversions */
	FREE_RUNNING = 0b000,
/** Starts another conversion when value at analog comparator pin AIN0 becomes higher than at AIN1 */
	ANALOG_COMPARATOR = 0b001,
/** Starts another conversion when external interrupt occurs on pin INT0 */
	EXTERNAL_INTERRUPT_REQ_0 = 0b010,
/** Starts another conversion when Timer 0 reaches value in output compare register A */
	TC0_COMPARE_MATCH = 0b011,
/** Starts another conversion when Timer 0 overflows from max value to 0 */
	TC0_OVERFLOW = 0b100,
/** Starts another conversion when Timer 1 reaches value in output compare register B */
	TC1_COMPARE_MATCH_B = 0b101,
/** Starts another conversion when Timer 1 overflows from max value to 0 */
	TC1_OVERFLOW = 0b110,
/** Starts another conversion at Timer 1 input capture event */
	TC1_CAPTURE_EVENT = 0b111
}adc_conversionMode_enum_t;

/** ADC configuration structure
*/
typedef struct adc_struct_t
{
/** Conversion result 8 bit or 10 bit resolution */
	adc_resolution_enum_t resolution;

/**	Source of reference voltage */
	adc_reference_enum_t referenceVoltage;

/** Which channels are going to be used*/
	bool channel;

/**	Division factor for ADC clock */
	adc_prescaler_enum_t prescaler;

/** Conversion mode */
	adc_conversionMode_enum_t conversionMode;
}adc_struct_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/


/** Initializes ADC peripheral.
	@remark Must be called before any other ADC function is used.
	@param[in] s_adc: ADC parameters to initialize
*/
void adc_init(adc_struct_t s_adc);

/** Powers up the ADC.
	@pre Must be called after the ADC is initialized(with @link adc_init @endlink).
	@param[in] s_adc: ADC peripheral to use
*/
void adc_start(adc_struct_t s_adc);

/** Shuts down the ADC.
	@pre Must be called after the ADC is initialized(with @link adc_init @endlink).
	@param[in] s_adc: ADC peripheral to start
*/
void adc_stop(adc_struct_t s_adc);

/** Enables ADC interrupts when a conversion is completed
	@pre Must be called after an interrupt is attached(with @link adc_attachInterrupt @endlink).
	@param[in] s_adc: ADC peripheral to stop
*/
void adc_enableInterrupts();

/** Disables ADC interrupts
*/
void adc_disableInterrupts();

/** Sets a callback function to be called on ADC interrupt. Can replace previous callback (no need to call @link adc_detachInterrupt @endlink first).
	@param[in]	p_function: pointer to function to call on interrupt
*/
void adc_attachInterrupt(void (*p_function)(void));

/** Clears callback function on ADC interrupt in case the callback will no longer be used.
*/
void adc_detachInterrupt();


/** Reads the value from a single specified channel
	@pre Must be called after the ADC is initialized(with @link adc_init @endlink).
	@param[in] s_adc: ADC peripheral to use
	@param[in] channel: ADC channel to read from
	@return value converted (16bit)
*/

u16 adc_singleRead(adc_struct_t s_adc);

/** Reads the value from every selected channel.
	@pre Must be called after the ADC is initialized(with @link adc_init @endlink).
	@param[in] s_adc: ADC peripheral to use
	@return pointer to an array which contains the values converted 
*/
u16 *adc_multiRead(adc_struct_t s_adc);

/** Changes the reference voltage
	@pre Must be called after the ADC is initialized(with @link adc_init @endlink).
	@param[in] s_adc: address of the ADC peripheral to use
	@param[in] s_adc.referenceVoltage: reference voltage to change to
*/

void adc_changeRefVoltage(adc_struct_t *s_adc, adc_reference_enum_t referenceVoltage);

#endif /* ADC_H_ */
