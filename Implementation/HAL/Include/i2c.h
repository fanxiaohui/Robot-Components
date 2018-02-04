/**	@file		i2c.h
	@brief		I2C features
	@details	Supports only master mode without interrupts.
				Basic flow:
				1. Initialize a @link i2c_struct_t @endlink.
				2. Pass it to @link i2c_init @endlink.
				3. Call @link i2c_start @endlink.
				4. Use the following sequences to communicate:
					a. Transmission
						- Call @link i2c_sendStart @endlink with the write address of the slave device
						- Call @link i2c_write @endlink with the register to write to
						- Call @link i2c_write @endlink with the data to write to the register
						- Call @link i2c_sendStop @endlink
					a. Reception
						- Call @link i2c_sendStart @endlink with the write address of the slave device
						- Call @link i2c_write @endlink with the register to read from
						- Call @link i2c_sendRepStart @endlink with the read address of the slave device
						- Call @link i2c_read @endlink to read the register
						- Call @link i2c_sendStop @endlink
				- To stop the I2C (for power saving for instance) call @link i2c_stop @endlink.
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

/** Defines the data direction (reading from I2C device)
*/
#define I2C_READ    1

/** Defines the data direction (writing to I2C device)
*/
#define I2C_WRITE   0

/** No error on the I2C bus
*/
#define I2C_NO_ERROR			0x00

/**	Bus locked by another device
*/
#define I2C_BUS_LOCKED			0xFF

/** Lost arbitration to another master
*/
#define I2C_LOST_ARBITRATION	TW_MT_ARB_LOST

/**	Start condition sent
*/
#define I2C_START				TW_START
/**	Repeated start condition sent
*/
#define I2C_REPEATED_START		TW_REP_START

/**	Received ACK of sent address in master transmitter mode
*/
#define I2C_MT_ADDR_ACK			TW_MT_SLA_ACK
/**	Did not receive ACK of sent address in master transmitter mode
*/
#define I2C_MT_ADDR_NACK		TW_MT_SLA_NACK
/**	Received ACK of sent data in master transmitter mode
*/
#define I2C_MT_DATA_ACK			TW_MT_DATA_ACK
/**	Did not receive ACK of sent data in master transmitter mode
*/
#define I2C_MT_DATA_NACK		TW_MT_DATA_NACK

/**	Received ACK of sent address in master receiver mode
*/
#define I2C_MR_ADDR_ACK			TW_MR_SLA_ACK
/**	Did not receive ACK of sent address in master receiver mode
*/
#define I2C_MR_ADDR_NACK		TW_MR_SLA_NACK
/**	Sent ACK for received data in master receiver mode
*/
#define I2C_MR_DATA_ACK			TW_MR_DATA_ACK
/**	Didn't send ACK for received data in master receiver mode
*/
#define I2C_MR_DATA_NACK		TW_MR_DATA_NACK

/** I2C configuration structure
*/
typedef struct i2c_struct_t
{
#ifdef I2C_USING_MASTER
	/**	SCL frequency to be used */
	u32 frequency;
#endif

#ifdef I2C_USING_SLAVE
	/**	Slave address to be used */
	u8 u8_address;

	/**	Slave address mask. Bits set here are ignored on the slave address match. e.g. if slave address mask is 0x02 and slave address 0x26, both 0x26 and 0x27 will match. */
	u8 addressMask;

	/**	General call enable */
	bool generalCallEnable;
#endif
}i2c_struct_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Initializes I2C peripheral.
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

/** Issues a start condition and sends address and transfer direction
	@pre Must be called after the I2C is initialized (with @link i2c_init @endlink).
	@param[in]	u8_address: address and transfer direction of I2C device
	@return		Succeeded in accessing device
 */
u8 i2c_sendStart(u8 u8_address);

/** Issues a repeated start condition and sends address and transfer direction. Functionally identical to @link sendStart @endlink.
	@pre Must be called after the I2C is initialized (with @link i2c_init @endlink).
	@param[in]	u8_address: address and transfer direction of I2C device
	@return		Succeeded in accessing device
 */
u8 i2c_sendRepStart(u8 u8_address);

/** Stops I2C peripheral.
	@pre Must be called after the I2C is initialized (with @link i2c_init @endlink).
*/
void i2c_sendStop(void);

/** Transmits data to a slave.
	@pre		Must be called after a start/repeated start condition was sent (with @link i2c_sendStart @endlink).
	@post		Call @link i2c_sendStop @endlink after transfer is completed to release the bus.
	@param[in]	u8_data: byte to send
	@return		error code
*/
u8 i2c_write(u8 u8_data);

/** Receives data from a slave while sending an Ack (more data to be received).
	@pre		Must be called after a start/repeated start condition was sent (with @link i2c_sendStart @endlink).
	@post		Call @link i2c_sendStop @endlink after transfer is completed to release the bus.
	@return		byte received
*/
u8 i2c_readAck(void);

/** Receives data from a slave while not sending an Ack (no more data to be received).
	@pre		Must be called after a start/repeated start condition was sent (with @link i2c_sendStart @endlink).
	@post		Call @link i2c_sendStop @endlink after transfer is completed to release the bus.
	@return		byte received
*/
u8 i2c_readNak(void);

/** Enables I2C interrupts
	@pre Must be called after an interrupt is attached(with @link i2c_attachInterrupt @endlink).
*/
void i2c_enableInterrupts();

/** Disables I2C interrupts
*/
void i2c_disableInterrupts();

/** Sets a callback function to be called on I2C interrupt. Can replace previous callback (no need to call @link i2c_detachInterrupt @endlink first).
	@param[in]	p_function: pointer to function to call on interrupt
*/
void i2c_attachInterrupt(void (*p_function)(void));

/** Clears callback function on I2C interrupt in case the callback will no longer be used.
*/
void i2c_detachInterrupt();

#endif
