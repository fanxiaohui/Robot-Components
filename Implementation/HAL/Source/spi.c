/**	@file		spi.c
	@brief		SPI features
	@author		Adrian Grosu
	@version	0.1
	@date		09.09.2017
	@details	See spi.h for details.
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
#include "spi.h"

/************************************************************************/
/* Internal variables                                                   */
/************************************************************************/

volatile u8 au8_TBuffer[SPI_T_BUFFER_MAX_SIZE];
volatile u8 au8_RBuffer[SPI_R_BUFFER_MAX_SIZE];
volatile u8 u8_TLastTransmittedIndex;
volatile u8 u8_TLastRequestedIndex;
volatile u8 u8_RLastReceivedIndex;
u8 u8_RLastProcessedIndex;
volatile bool b_RAvailable;
volatile bool b_TAvailable;
void (*p_spiCallback)(void);


/************************************************************************/
/* Interrupt handlers                                                   */
/************************************************************************/

#ifdef SPI_INTERRUPT_MODE
ISR(SPI_STC_vect)
{
	au8_RBuffer[u8_RLastReceivedIndex] = SPDR;
	if (u8_RLastReceivedIndex < SPI_R_BUFFER_MAX_SIZE - 1)
		u8_RLastReceivedIndex++;
	else
		u8_RLastReceivedIndex = 0;
	b_RAvailable = TRUE;
	
	if (u8_TLastTransmittedIndex != u8_TLastRequestedIndex)
	{
		SPDR = au8_TBuffer[u8_TLastTransmittedIndex];
		if (u8_TLastTransmittedIndex < SPI_T_BUFFER_MAX_SIZE - 1)
			u8_TLastTransmittedIndex++;
		else
			u8_TLastTransmittedIndex = 0;
	}
	else
	{
		b_TAvailable = TRUE;
	}
}
#endif

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void spi_init(spi_struct_t s_spi)
{
	/** Sets Master or Slave mode and SPI pins direction*/
	updateBit(&SPCR, MSTR, s_spi.mode & 1);
	switch(s_spi.mode)
	{
		case SPI_MASTER:
			setBit(&DDRB, DDB4);
			setBit(&DDRB, DDB5);
			setBit(&DDRB, DDB7);
			clearBit(&DDRB, DDB6);
			break;
		case SPI_SLAVE:
			clearBit(&DDRB, DDB4);
			clearBit(&DDRB, DDB5);
			clearBit(&DDRB, DDB7);
			setBit(&DDRB, DDB6);
			break;
	}
	/** Sets the data transmission order */
	updateBit(&SPCR, DORD, s_spi.dataOrder & 1);
	/** Sets the SCK idle state */
	updateBit(&SPCR, CPOL, s_spi.sckIdleState & 1);
	/** Sets the SCK data sampling edge */
	updateBit(&SPCR, CPHA, s_spi.sckSampleEdge & 1);
	/** Sets the division factor for the SCK line from the I/O clock*/
	updateBit(&SPCR, SPR1, (s_spi.divisionFactor >> 1) & 1);
	updateBit(&SPCR, SPR0, s_spi.divisionFactor & 1);
	updateBit(&SPSR, SPI2X, s_spi.doubleSpeed & 1);
	if (s_spi.readWrite == READ)
	{
		u8_RLastProcessedIndex = 0;
		u8_RLastReceivedIndex = 0;
		b_RAvailable = FALSE;
	}
	if (s_spi.readWrite == WRITE)
	{
		u8_TLastRequestedIndex = 0;
		u8_TLastTransmittedIndex = 0;
		b_TAvailable = TRUE;
	}
}

void spi_start()
{
	setBit(&SPCR, SPE);
}

void spi_stop()
{
	clearBit(&SPCR, SPE);
}

void spi_enableInterrupts()
{
	setBit(&SPCR, SPIE);
}

void spi_disableInterrupts()
{
	clearBit(&SPCR, SPIE);
}

void spi_attachInterrupt(void (*p_function)(void))
{
	p_spiCallback = p_function;
}

void spi_detachInterrupt()
{
	p_spiCallback = 0;
}

u8 spi_readData()
{
	u8 u8_returnValue = 0;
	#ifdef SPI_INTERRUPT_MODE
		u8_returnValue = au8_RBuffer[u8_RLastProcessedIndex];
		if (u8_RLastProcessedIndex < SPI_R_BUFFER_MAX_SIZE - 1)
			u8_RLastProcessedIndex++;
		else
			u8_RLastProcessedIndex = 0;
		if (u8_RLastProcessedIndex == u8_RLastReceivedIndex)
			b_RAvailable = FALSE;
	#else
		u8_returnValue = spi_transferData(0x0);
	#endif
	return u8_returnValue;
}

void spi_writeData(u8 data)
{
	#ifdef SPI_INTERRUPT_MODE
		au8_TBuffer[u8_TLastRequestedIndex] = data;
		if (u8_TLastRequestedIndex < SPI_T_BUFFER_MAX_SIZE - 1)
			u8_TLastRequestedIndex++;
		else
			u8_TLastRequestedIndex = 0;
		if(b_TAvailable == TRUE)
		{
			if (u8_TLastTransmittedIndex < SPI_T_BUFFER_MAX_SIZE - 1)
				u8_TLastTransmittedIndex++;
			else
				u8_TLastTransmittedIndex = 0;
			b_TAvailable = FALSE;
			SPDR = data;
		}
	#else
		spi_transferData(data);
	#endif
}

u8 spi_transferData(u8 data)
{
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
	data = SPDR;
	return data;
}

bool spi_receiveAvailable()
{
	#ifdef SPI_INTERRUPT_MODE
		return b_RAvailable;
	#endif
	return TRUE;
}