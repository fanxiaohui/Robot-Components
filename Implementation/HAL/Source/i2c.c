/**	@file		i2c.h
	@brief		i2c features
	@author		Florin Popescu
	@version	0.1
	@date		01.10.2017
	@details	See i2c.h for details
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
#include "gpio.h"
#include "i2c.h"
#include "math.h"

/************************************************************************/
/* Internal variables                                                   */
/************************************************************************/

/************************************************************************/
/* Internal functions                                                   */
/************************************************************************/

u8 calculateTWBR(u32 u32_frequency, u8 *pu8_prescaler)
{
	u32 u32_returnValue;
	u8 u8_divisor = 1;
	*pu8_prescaler = 0;

	u32_returnValue = (SYSTEM_CLOCK_FREQUENCY / u32_frequency - 16) / (2 * u8_divisor);
	if (u32_returnValue > 0xff)
	{
		u8_divisor = 4;
		*pu8_prescaler = 1;
		u32_returnValue = (SYSTEM_CLOCK_FREQUENCY / u32_frequency - 16) / (2 * u8_divisor);
		if (u32_returnValue > 0xff)
		{
			u8_divisor = 16;
			*pu8_prescaler = 2;
			u32_returnValue = (SYSTEM_CLOCK_FREQUENCY / u32_frequency - 16) / (2 * u8_divisor);
			if (u32_returnValue > 0xff)
			{
				u8_divisor = 64;
				*pu8_prescaler = 3;
				u32_returnValue = (SYSTEM_CLOCK_FREQUENCY / u32_frequency - 16) / (2 * u8_divisor);
			}
		}
	}

	return (u8) u32_returnValue;
}

u8 stopAndGetErrorCode()
{
	u8 u8_returnValue = (TWSR & 0xF8);
	/* Send stop condition. Also clears I2C flag because it writes a 1 to it. */
	setBit(&TWCR, TWSTO);
	return u8_returnValue;
}

/************************************************************************/
/* Interrupt handlers                                                   */
/************************************************************************/

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void i2c_init(i2c_struct_t s_i2c)
{
#ifdef I2C_USING_MASTER
	u8 u8_prescaler;
	/* Set bit rate  */
	TWBR = calculateTWBR(s_i2c.frequency, &u8_prescaler);
	/* Set prescaler */
	updateBit(&TWSR, TWPS0, checkBit(u8_prescaler, 0));
	updateBit(&TWSR, TWPS1, checkBit(u8_prescaler, 1));
#endif
#ifdef I2C_USING_SLAVE
	/* Set slave address */
	TWAR = s_i2c.address << 1;
	/* Set general call enable */
	updateBit(&TWAR, TWGCE, s_i2c.generalCallEnable);
	/* Enable ACK bit transmission */
	setBit(&TWCR, TWEA);
	/* Set slave address mask */
	TWAMR = s_i2c.addressMask << 1;
#endif
#ifdef I2C_INTERRUPT_MODE
	/* Enable I2C interrupt */
	setBit(&TWCR, TWIE);
#endif
	gpio_struct_t s_sdaPin;
	s_sdaPin.port = PC;
	s_sdaPin.number = 1;
	s_sdaPin.direction = INPUT;
	s_sdaPin.pullUp = USE_PULLUP;
	gpio_init(s_sdaPin);

	gpio_struct_t s_sclPin;
	s_sclPin.port = PC;
	s_sclPin.number = 0;
	s_sclPin.direction = INPUT;
	s_sclPin.pullUp = USE_PULLUP;
	gpio_init(s_sclPin);
}

void i2c_start()
{
	setBit(&TWCR, TWEN);
}

void i2c_stop()
{
	clearBit(&TWCR, TWEN);
}

u8 i2c_transmit(u8 u8_address, u8 *au8_data, u8 u8_dataLength)
{
	u8 i = 0;
	/* Send start condition */
	setBit(&TWCR, TWSTA);
	/* Clear I2C flag */
	setBit(&TWCR, TWINT);
	/* Wait for interrupt flag */
	while (!checkBit(TWCR, TWINT));
	/* Check if start condition was issued */
	if ((TWSR & 0xF8) == I2C_START)
	{
		/* Set slave address */
		TWDR = u8_address << 1;
		/* Set write mode */
		clearBit(&TWDR, TWD0);
		/* Clear start condition. Also clears I2C flag because it writes a 1 to it. */
		clearBit(&TWCR, TWSTA);
		/* Wait for interrupt flag */
		while (!checkBit(TWCR, TWINT));
		/* Check if ACK was received */
		if ((TWSR & 0xF8) == I2C_SLAVE_WRITE_ACK)
		{
			/* Transmit data */
			while (i != u8_dataLength)
			{
				/* Set next byte to write */
				TWDR = au8_data[i];
				/* Clear I2C flag */
				setBit(&TWCR, TWINT);
				/* Wait for interrupt flag */
				while (!checkBit(TWCR, TWINT));
				/* Check if slave didn't sent ACK */
				if ((TWSR & 0xF8) != I2C_DATA_WRITE_ACK)
					return stopAndGetErrorCode();
				i++;
			}
		}
		else
			return stopAndGetErrorCode();
	}
	else
		return stopAndGetErrorCode();

	/* Send stop condition. Also clears I2C flag because it writes a 1 to it. */
	setBit(&TWCR, TWSTO);
	/* Wait for stop condition to be sent. */
	while (checkBit(TWCR, TWSTO));

	return I2C_NO_ERROR;
}

u8 i2c_receive(u8 u8_address, u8 *au8_data, u8 u8_dataLength)
{
	u8 i = 0;
	/* Send start condition */
	setBit(&TWCR, TWSTA);
	/* Clear I2C flag */
	setBit(&TWCR, TWINT);
	/* Wait for interrupt flag */
	while (!checkBit(TWCR, TWINT));
	/* Check if start condition was issued */
	if ((TWSR & 0xF8) == I2C_START)
	{
		/* Set slave address */
		TWDR = u8_address << 1;
		/* Set read mode */
		setBit(&TWDR, TWD0);
		/* Clear start condition. Also clears I2C flag because it writes a 1 to it. */
		clearBit(&TWCR, TWSTA);
		/* Wait for interrupt flag */
		while (!checkBit(TWCR, TWINT));
		/* Check if ACK was received */
		if ((TWSR & 0xF8) == I2C_SLAVE_READ_ACK)
		{
			/* Receive data */
			while (i != u8_dataLength)
			{
				/* Transmit ACK after a transfer */
				setBit(&TWCR, TWEA);
				/* Wait for interrupt flag */
				while (!checkBit(TWCR, TWINT));
				/* Check if slave didn't sent ACK */
				if ((TWSR & 0xF8) != I2C_DATA_READ_ACK)
					return stopAndGetErrorCode();
				/* Read next byte */
				au8_data[i] = TWDR;
				i++;
			}
		}
		else
			return stopAndGetErrorCode();
	}
	else
		return stopAndGetErrorCode();

	/* Send NACK after all expected transfers are done. Also clears I2C flag because it writes a 1 to it. */
	clearBit(&TWCR, TWEA);
	/* Wait for interrupt flag */
	while (!checkBit(TWCR, TWINT));
	/* If NACK was sent, all is good */
	if ((TWSR & 0xF8) == I2C_DATA_READ_NACK)
	{
		/* Send stop condition. Also clears I2C flag because it writes a 1 to it. */
		setBit(&TWCR, TWSTO);
		/* Wait for stop condition to be sent. */
		while (checkBit(TWCR, TWSTO));
	}

	return I2C_NO_ERROR;
}

void i2c_enableInterrupts()
{

}

void i2c_disableInterrupts()
{

}

void i2c_attachInterrupt(void (*p_function)(void))
{

}

void i2c_detachInterrupt()
{

}