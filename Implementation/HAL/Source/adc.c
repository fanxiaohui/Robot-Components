/**	@file		adc.c
	@brief		ADC features
	@author		Adrian Grosu
	@version	0.1
	@date		13.08.2017
	@details	See adc.h for details.
*/

/************************************************************************/
/* AVR includes                                                         */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "device.h"
#include "math.h"
#include "adc.h"

/************************************************************************/
/* Internal variables                                                   */
/************************************************************************/

u16 au16_conversionResultsBuffer[8];
void (*p_adcCallback)(void);

/************************************************************************/
/* Interrupt handlers                                                   */
/************************************************************************/

#ifdef ADC_INTERRUPT_MODE
ISR(ADC_vect)
{
	p_adcCallback();
}
#endif

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void adc_init(adc_struct_t s_adc)
{
	/* Set the reference voltage */
	updateBit(&ADMUX, REFS1, (s_adc.referenceVoltage >> 1) & 1);
	updateBit(&ADMUX, REFS0, s_adc.referenceVoltage & 1);
	/* Set the result adjustment */
	updateBit(&ADMUX, ADLAR, s_adc.resolution);
	/* Set the analog input channel (no differential ADC) */
	clearBit(&ADMUX, MUX4);
	clearBit(&ADMUX, MUX3);
	/* Set prescaler division factor */
	updateBit(&ADCSRA, ADPS2, (s_adc.prescaler >> 2) & 1);
	updateBit(&ADCSRA, ADPS1, (s_adc.prescaler >> 1) & 1);
	updateBit(&ADCSRA, ADPS0, s_adc.prescaler & 1);
	/* Set the Trigger Source if Auto Trigger is enabled */
	if(s_adc.conversionMode == SINGLE_CONVERSION)
		clearBit(&ADCSRA, ADATE);
	else
	{
		setBit(&ADCSRA, ADATE);
		updateBit(&ADCSRB, ADTS2, (s_adc.conversionMode >> 2) & 1);
		updateBit(&ADCSRB, ADTS1, (s_adc.conversionMode >> 1) & 1);
		updateBit(&ADCSRB, ADTS0, s_adc.conversionMode & 1);
	}
}

u16 adc_singleRead(adc_struct_t s_adc)
{
	u16 readValue = 0;
	u16 temp = 0;
	ADMUX = s_adc.channel | (ADMUX & 0b11111000);
	if(s_adc.conversionMode == SINGLE_CONVERSION)
	{
		setBit(&ADCSRA, ADSC);
		while(!(ADCSRA & (1<<ADIF)));
	}
	else
	{
		setBit(&ADCSRA, ADIF);
		while(!(ADCSRA & (1<<ADIF)));
		setBit(&ADCSRA, ADIF);
		while(!(ADCSRA & (1<<ADIF)));
		
	}
	switch(s_adc.resolution)
	{
		case _8BIT_RES:
			readValue = ADCH;
			break;
		case _10BIT_RES:
			readValue = ADCL;
			temp = ADCH;
			readValue |= (temp << 8);
			break;
	}
	return readValue;
}

/*u16 *adc_multiRead(adc_struct_t s_adc)
{
	u8 index = 0;
	for(index = 0; index < 8; index++)
		if(s_adc.channelEnabled[index])
			au16_conversionResultsBuffer[index] = adc_singleRead(s_adc, index);
		else
			au16_conversionResultsBuffer[index] = 0;
	return au16_conversionResultsBuffer;
}
*/


void adc_start(adc_struct_t s_adc)
{
	setBit(&ADCSRA, ADEN);
		updateBit(&DIDR0, s_adc.channel, TRUE);
	if (s_adc.conversionMode != SINGLE_CONVERSION)
		setBit(&ADCSRA, ADSC);
}

void adc_stop(adc_struct_t s_adc)
{
	clearBit(&ADCSRA, ADEN);
		updateBit(&DIDR0, s_adc.channel,FALSE);
}

void adc_enableInterrupts()
{
	setBit(&ADCSRA, ADIE);
}

void adc_disableInterrupts()
{
	clearBit(&ADCSRA, ADIE);
}

void adc_attachInterrupt(void (*p_function)(void))
{
	p_adcCallback = p_function;
}

void adc_detachInterrupt()
{
	p_adcCallback = 0;
}

void adc_changeRefVoltage(adc_struct_t *s_adc, adc_reference_enum_t referenceVoltage)
{
	s_adc->referenceVoltage = referenceVoltage;
	updateBit(&ADMUX, REFS1, (s_adc->referenceVoltage >> 1) & 1);
	updateBit(&ADMUX, REFS0, s_adc->referenceVoltage & 1);
	if(s_adc->conversionMode == FREE_RUNNING)
	{
		setBit(&ADCSRA, ADIF);
		while(!(ADCSRA & (1<<ADIF)));
	}
}