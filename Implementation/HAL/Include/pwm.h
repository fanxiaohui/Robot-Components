/**	@file		pwm.h
	@brief		PWM features
	@details	Supports all 4 timers as PWM generators, with the following PWM implementations:
				- Fast PWM (the duty cycle starts with the period)
				- Centered PWM (The duty cycle is centered around the middle of the period, but not as the frequency changes; It is centered around the middle of the period even if the frequency changes only for Timer1 and Timer3)
				Basic flow:
				1. Initialize a @link timer_struct_t @endlink with either a fixed (set base frequency to a value of type @link timer_prescaler_enum_t @endlink) or a custom frequency (set to any unsigned value).
				@remark If using @link TIMER0 @endlink or @link TIMER2 @endlink with a custom frequency only OCnB pin will be available for signal generation.
				2. Pass it to @link pwm_init @endlink.
				3. Default duty cycle is 0%. @link pwm_setDutyCycle @endlink can be called before starting the timer to change the starting duty cycle.
				4. Call @link pwm_start @endlink for the channel you want to output on.
				- Call @link pwm_setFrequency @endlink or @link pwm_setDutyCycle @endlink to modify one of these parameters.
				- To stop the timer call @link pwm_stop @endlink.
*/

#ifndef PWM_H_
#define PWM_H_

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "timer.h"
#include "types.h"

/** @example pwm_config.h
	This is an example of how to configure the PWM driver
*/
#include "pwm_config.h"

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/** Specifies the signal type.
*/
typedef enum pwm_signalType_enum_t
{
/**	The signal has a fixed frequency and variable duty cycle */
	DUTY_CYCLE_VARIABLE,
/**	The signal has a fixed duty cycle at 50% and variable frequency. */
	FREQUENCY_VARIABLE,
/**	Both the duty cycle and frequency are variable */
	FULLY_VARIABLE
}pwm_signalType_enum_t;

/** Specifies the PWM mode to use.
*/
typedef enum pwm_mode_enum_t
{
/** The duty cycle starts with the period. This can have twice as high frequencies compared to the centered mode. */
	FAST_PWM,
/** The duty cycle is centered around the middle of the period, but not as the frequency changes. It is centered around the middle of the period even if the frequency changes only for Timer1 and Timer3 */
	CENTERED_PWM
}pwm_mode_enum_t;

/** Channel as part of PWM configuration structure
*/
typedef struct pwm_channel_struct_t
{
/**	Whether output should be enabled for this channel */
	bool enabled;
/** Whether output should be inverted (active low) or not (active high) for this channel */
	bool invertedOutput;
}pwm_channel_struct_t;

/** PWM configuration structure
*/
typedef struct pwm_struct_t
{
/** Base timer configuration structure */
	timer_struct_t base;
/**	What signal type to generate */
	pwm_signalType_enum_t signalType;
/**	Which PWM mode to use */
	pwm_mode_enum_t mode;
/**	Output pin OCxA */
	pwm_channel_struct_t channelA;
/**	Output pin OCxB */
	pwm_channel_struct_t channelB;
}pwm_struct_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Initializes timer peripheral and initializes corresponding GPIO pins as outputs
	@remark		Must be called before any other function for the required timer.
	@remark		Actual timer frequency may differ from the actual one due to division rounding.
	@param[in]	s_pwm: timer parameters to initialize
*/
void pwm_init(pwm_struct_t *s_pwm);

/** Starts timer peripheral
	@pre		Must be called after the timer was initialized (with @link pwm_init @endlink).
	@param[in]	s_pwm: timer peripheral to use
*/
void pwm_start(pwm_struct_t s_pwm);

/** Stops timer peripheral.
	@pre		Must be called after the timer was initialized (with @link pwm_init @endlink).
	@param[in]	s_pwm: timer peripheral to use
*/
void pwm_stop(pwm_struct_t s_pwm);

/** Sets the frequency to be used in PWM mode and updates duty cycle accordingly
	@remark		This changes the frequency for both channel outputs of a timer if using Timer1 or Timer3
	@remark		Actual timer frequency may differ from the expected one due to division rounding.
	@pre		PWM must be initialized in either @link FREQUENCY_VARIABLE @endlink or @link FULLY_VARIABLE @endlink signal type.
	@param[in]	s_pwm: timer peripheral to use
	@param[in]	u32_frequency: frequency in Hz to set
*/
void pwm_setFrequency(pwm_struct_t s_pwm, u32 u32_frequency);

/** Sets the duty cycle to be used in PWM mode
	@remark		Duty cycle can never be 0 due to the way the timer is built, so function will treat the output pin as GPIO and clear it.
	@param[in]	s_pwm: timer peripheral to use
	@param[in]	e_channel: channel output to modify
	@param[in]	u8_percent: duty cycle to set
*/
void pwm_setDutyCycle(pwm_struct_t s_pwm, timer_channel_enum_t e_channel, u8 u8_percent);

#endif /* PWM_H_ */
