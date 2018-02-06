/**	@file		i2c.c
	@brief		i2c features
	@details	See @link i2c.h @endlink for details
*/

/************************************************************************/
/* AVR includes                                                         */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <compat/twi.h>

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "device.h"
#include "gpio.h"
#include "i2c.h"
#include "math.h"

/************************************************************************/
/* Internal functions                                                   */
/************************************************************************/

u8 calculateTWBR(u32 u32_frequency, u8 *pu8_prescaler)
{
	u32 u32_returnValue;
	u8 u8_divisor = 1;
	*pu8_prescaler = 0;

	u32_returnValue = (F_CPU / u32_frequency - 16) / (2 * u8_divisor);
	if (u32_returnValue > 0xff)
	{
		u8_divisor = 4;
		*pu8_prescaler = 1;
		u32_returnValue = (F_CPU / u32_frequency - 16) / (2 * u8_divisor);
		if (u32_returnValue > 0xff)
		{
			u8_divisor = 16;
			*pu8_prescaler = 2;
			u32_returnValue = (F_CPU / u32_frequency - 16) / (2 * u8_divisor);
			if (u32_returnValue > 0xff)
			{
				u8_divisor = 64;
				*pu8_prescaler = 3;
				u32_returnValue = (F_CPU / u32_frequency - 16) / (2 * u8_divisor);
			}
		}
	}

	return (u8) u32_returnValue;
}

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

u8 i2c_sendStart(u8 u8_address)
{
	u16 timeout=0xFFFF;
	/* Send START condition */
	TWCR |= (1 << TWINT) | (1 << TWSTA) | (checkBit(TWCR, TWEN) << TWEN);

	/* Wait until start condition has been sent */
	while(--timeout > 0)
		if(checkBit(TWCR, TWINT))
			break;
	if(timeout == 0)
		return I2C_BUS_LOCKED;

	timeout=0xFFFF;
	/* Check if start or repeated start condition was sent */
	if (((TW_STATUS & 0xF8) != TW_START) && ((TW_STATUS & 0xF8) != TW_REP_START))
		return (TW_STATUS & 0xF8);

	/* Send device address */
	TWDR = u8_address;
	TWCR = (1<<TWINT) | (checkBit(TWCR, TWEN) << TWEN);

	/* Wail until address has been sent */
	while( --timeout > 0 )
		if(checkBit(TWCR, TWINT))
			break;
	if(timeout == 0)
		return I2C_BUS_LOCKED;

	/* Check if ACK was received */
	if (((TW_STATUS & 0xF8) != TW_MT_SLA_ACK) && ((TW_STATUS & 0xF8) != TW_MR_SLA_ACK))
		return (TW_STATUS & 0xF8);

	return I2C_NO_ERROR;
}

u8 i2c_sendRepStart(u8 u8_address)
{
	return i2c_sendStart(u8_address);
}

void i2c_sendStop(void)
{
	u16 timeout=0xFFFF;
	/* Send stop condition */
	TWCR |= (1 << TWINT) | (1 << TWSTO) | (checkBit(TWCR, TWEN) << TWEN);
	/* Wait until stop condition is sent and bus is released */
	while(--timeout > 0)
		if(!checkBit(TWCR, TWSTO))
			return;
}

u8 i2c_write(u8 u8_data)
{
	u16 timeout=0xFFFF;
	/* Send byte */
	TWDR = u8_data;
	TWCR = (1 << TWINT) | (checkBit(TWCR, TWEN) << TWEN);

	/* Wait until transmission is completed */
	while(--timeout > 0)
		if(checkBit(TWCR, TWINT))
			break;
	if(timeout == 0)
		return I2C_BUS_LOCKED;

	/* Check if ACK was received */
	if (((TW_STATUS & 0xF8) != TW_MT_SLA_ACK) && ((TW_STATUS & 0xF8) != TW_MR_SLA_ACK))
		return (TW_STATUS & 0xF8);

	return I2C_NO_ERROR;
}

u8 i2c_readAck(void)
{
	u16 timeout=0xFFFF;
	/* Receive byte */
	TWCR = (1<<TWINT) | (1<<TWEA) | (checkBit(TWCR, TWEN) << TWEN);

	/* Wait until transmission is completed */
	while(--timeout > 0)
		if(checkBit(TWCR, TWINT))
		return TWDR;

	return I2C_BUS_LOCKED;
}

u8 i2c_readNak(void)
{
	u16 timeout=0xFFFF;
	/* Receive byte */
	TWCR = (1<<TWINT) | (checkBit(TWCR, TWEN) << TWEN);

	/* Wait until transmission is completed */
	while(--timeout > 0)
		if(checkBit(TWCR, TWINT))
			return TWDR;

	return I2C_BUS_LOCKED;
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