/**	@file		encoder.h
	@brief		Encoder features
	@author		Adrian Grosu
	@version	1.0
	@date		31.10.2017
	@details	Supports encoders with 2 pin outputs
				1. Initialize a @link encoder_struct_t @endlink. You do not need to initialize the structure members beforehand, you only have to declare them and set their values.
				2. Pass it to @link encoder_init @endlink.
				3. Call @link encoder_start @endlink.
				4. Call @link encoder_getCounter @endlink when needed to check the number of impulses
				- You can reset the counter by calling @link encoder_resetCounter @endlink.
				- You can stop the encoders by calling @link encoder_stop @endlink.
	@remark		@link encoder_stop @endlink doesn't reset the value of the counter, it only disables the interrupts.
*/



#ifndef ENCODER_H_
#define ENCODER_H_

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "gpio.h"
#include "math.h"
#include "uart.h"

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/** Encoder configuration structure
*/

typedef struct encoder_struct_t{
/** Counter for the number of impulses generated by the encoder*/
	u32 counter;
/** The last state of the 2 encoder outputs */
	u8 lastState;
/** The current state of the 2 encoder outputs*/
	u8 currentState;
/** First output pin of the encoder */
	gpio_struct_t A;
/** Second output pin of the encoder */
	gpio_struct_t B;
/** Pointer to counter handling function */
	void (*p_countCallback)(void);
}encoder_struct_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Initializes encoder driver.
	@remark		Must be called before any other encoder function
	@param[in]	s_encoder: encoder parameters to initialize
*/
void encoder_init(encoder_struct_t *s_encoder, void (*p_function)(void));

/** Starts encoder driver.
	@pre		Must be called after the encoder was initialized (with @link encoder_init @endlink).
	@param[in]	s_encoder: encoder peripheral to use
*/
void encoder_start(encoder_struct_t s_encoder);

/** Stops encoder driver.
	@pre		Must be called after the encoder was initialized (with @link encoder_init @endlink).
	@param[in]	s_encoder: encoder peripheral to use
*/
void encoder_stop(encoder_struct_t s_encoder);

/** Returns the impulse counter for the specified encoder peripheral.
	@pre		Must be called after the encoder was initialized (with @link encoder_init @endlink).
	@param[in]	s_encoder: encoder peripheral to use
	@return		32 bit unsigned value of the counter
*/
u32 encoder_getCounter(encoder_struct_t s_encoder);

/** Resets the impulse counter for the specified encoder peripheral.
	@pre		Must be called after the encoder was initialized (with @link encoder_init @endlink).
	@param[in]	s_encoder: encoder peripheral to use
*/
void encoder_resetCounter(encoder_struct_t *s_encoder);

double encoder_getDistanceCm(encoder_struct_t s_encoder);

#endif /* ENCODER_H_ */