/**	@file		spi.h
	@brief		SPI features
	@details
*/

#ifndef SPI_H_
#define SPI_H_

/************************************************************************/
/* Project specific includes                                            */
/************************************************************************/

#include "types.h"

/** @example spi_config.h
	This is an example of how to configure the SPI driver
*/
#include "spi_config.h"

/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/** Specifies if the SPI module is set in Master or Slave mode.
*/
typedef enum spi_master_enum_t
{
	SPI_SLAVE = 0,
	SPI_MASTER = 1
}spi_master_enum_t;

/** Specifies if the SCK line is LOW or HIGH when idle.
*/
typedef enum spi_sckIdleState_enum_t
{
	LOW = 0,
	HIGH = 1
}spi_sckIdleState_enum_t;

/** Specifies if the data is sampled on the LEADING or the TRAILING edge
*/
typedef enum spi_sckSampleEdge_enum_t
{
	LEADING = 0,
	TRAILING = 1
}spi_sckSampleEdge_enum_t;

/** Specifies the division factor for the SPI clock. Values can be halved by enabling the double speed mode (MASTER only).
*/
typedef enum spi_clkDivision_enum_t
{
	SPI_PRESCALER_4 = 0b00,
	SPI_PRESCALER_16 = 0b01,
	SPI_PRESCALER_64 = 0b10,
	SPI_PRESCALER_128 = 0b11
}spi_clkDivision_enum_t;

/** Specifies in what order the data is transmitted.
*/
typedef enum spi_dataOrder_enum_t
{
	MSB_FIRST = 0,
	LSB_FIRST = 1
}spi_dataOrder_enum_t;

typedef enum spi_readWrite_enum_t
{
	READ,
	WRITE,
	READ_WRITE
}spi_readWrite_enum_t;

/** SPI configuration structure
*/
typedef struct spi_struct_t
{
/** Master or Slave operation mode */
	spi_master_enum_t mode;

/** HIGH or LOW SCK line when idle */
	spi_sckIdleState_enum_t sckIdleState;

/** LEADING or TRAILING edge used for sampling data*/
	spi_sckSampleEdge_enum_t sckSampleEdge;

/** Division factor from the I/O clock for the SCK line*/
	spi_clkDivision_enum_t divisionFactor;

/** SPI double speed mode. Halves the division factor if used*/
	bool doubleSpeed;

/** MSB or LSB first data transmission*/
	spi_dataOrder_enum_t dataOrder;

/** Set the SPI module as Read only, Write only or both */
	spi_readWrite_enum_t readWrite;
}spi_struct_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/** Initialized SPI peripheral.
	@remark Must be called before any other SPI function is used.
	@param[in] s_spi: SPI peripheral to use
*/
void spi_init(spi_struct_t s_spi);

/** Powers up the SPI.
	@pre Must be called after the SPI is initialized (with @link spi_init @endlink).
*/

void spi_start();

/** Shuts down the SPI.
	@pre Must be called after the SPI is initialized (with @link spi_init @endlink).
*/
void spi_stop();

/** Enables SPI interrupts when a serial transfer is complete
	@pre Must be called after an interrupt is attached(with @link spi_attachInterrupt @endlink).
*/
void spi_enableInterrupts();

/** Disables SPI interrupts
*/
void spi_disableInterrupts();

/** Sets a callback function to be called on SPI interrupt. Can replace previous callback (no need to call @link spi_detachInterrupt @endlink first).
	@param[in]	p_function: pointer to function to call on interrupt
*/
void spi_attachInterrupt(void (*p_function)(void));

/** Clears callback function on SPI interrupt in case the callback will no longer be used.
*/
void spi_detachInterrupt();

/** Starts a data transfer
	@param[in] data: data to transfer (8 bits).
*/
u8 spi_transferData(u8 data);

/** Reads data received from the SPI
	@return data received (8 bits).
*/
u8 spi_readData();

/** Writes data for the SPI to send
	@param[in] u8_data: data to be sent (8 bits).
*/
void spi_writeData(u8 u8_data);

#endif /* SPI_H_ */
