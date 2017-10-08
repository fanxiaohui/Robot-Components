/**	@file		i2c.h
	@brief		I2C features
	@author		Florin Popescu
	@version	0.1
	@date		01.10.2017
	@details
*/

#ifndef I2C_H_
#define I2C_H_

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/
#include "types.h"

/** @example i2c_config.h
	This is an example of how to configure the I2C driver
*/
#include "i2c_config.h"

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/
#define I2C_NO_ERROR			0x00

#define I2C_LOST_ARBITRATION	0x38

#define I2C_START				0x08
#define I2C_REPEATED_START		0x10

#define I2C_SLAVE_WRITE_ACK		0x18
#define I2C_SLAVE_WRITE_NACK	0x20
#define I2C_DATA_WRITE_ACK		0x28
#define I2C_DATA_WRITE_NACK		0x30

#define I2C_SLAVE_READ_ACK		0x40
#define I2C_SLAVE_READ_NACK		0x48
#define I2C_DATA_READ_ACK		0x50
#define I2C_DATA_READ_NACK		0x58

/** Specifies if the I2C module is set in Master or Slave mode.
*/
typedef enum i2c_master_enum_t
{
	I2C_SLAVE = 0,
	I2C_MASTER = 1
}i2c_master_enum_t;

/** I2C configuration structure
*/
typedef struct i2c_struct_t
{
/** Master or Slave operation mode */
	i2c_master_enum_t mode;

#ifdef I2C_USING_MASTER
/**	SCL frequency to be used */
	u32 frequency;
#endif

#ifdef I2C_USING_SLAVE
/**	Slave address to be used */
	u8 address;

/**	Slave address mask. Bits set here are ignored on the slave address match. e.g. if slave address mask is 0x02 and slave address 0x26, both 0x26 and 0x27 will match. */
	u8 addressMask;

/**	General call enable */
	bool generalCallEnable;
#endif
}i2c_struct_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Initialized I2C peripheral.
	@remark Must be called before any other I2C function is used.
	@param[in] s_i2c: I2C peripheral to use
*/
void i2c_init(i2c_struct_t s_i2c);

/** Starts I2C peripheral.
	@pre Must be called after the I2C is initialized (with @link i2c_init @endlink).
*/

void i2c_start();

/** Stops I2C peripheral.
	@pre Must be called after the I2C is initialized (with @link i2c_init @endlink).
*/
void i2c_stop();

/** Transmits data to a slave.
	@pre		Must be called after the I2C was initialized (with @link i2c_init @endlink).
	@param[in]	u8_address: slave address to send to
	@param[in]	au8_data: pointer to array containing data to send
	@param[in]	u8_dataLength: number of bytes to send
	@return		error code
*/
u8 i2c_transmit(u8 u8_address, u8 *au8_data, u8 u8_dataLength);

/** Receives data from a slave.
	@pre		Must be called after the I2C was initialized (with @link i2c_init @endlink).
	@param[in]	u8_address: slave address to receive from
	@param[in]	au8_data: pointer to array big enough to hold data to receive
	@param[in]	u8_dataLength: number of bytes to receive
	@return		error code
*/
u8 i2c_receive(u8 u8_address, u8 *au8_data, u8 u8_dataLength);

/** Enables I2C interrupt
	@pre Must be called after an interrupt is attached(with @link i2c_attachInterrupt @endlink).
*/
void i2c_enableInterrupts();

/** Disables I2C interrupt
*/
void i2c_disableInterrupts();

/** Sets a callback function to be called on I2C interrupt. Can replace previous callback (no need to call @link i2c_detachInterrupt @endlink first).
	@param[in]	p_function: pointer to function to call on interrupt
*/
void i2c_attachInterrupt(void (*p_function)(void));

/** Clears callback function on I2C interrupt in case the callback will no longer be used.
*/
void i2c_detachInterrupt();

#endif /* I2C_H_ */