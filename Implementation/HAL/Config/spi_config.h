/**	@file		spi_config.h
	@brief		SPI configuration
	@author		Adrian Grosu
	@version	0.1
	@date		09.09.2017
	@details	
*/

#ifndef SPI_CONFIG_H_
#define SPI_CONFIG_H_


/************************************************************************/
/* Defines, enums, structs, types                                       */
/************************************************************************/

/** Using interrupt mode for SPI
*/
#define SPI_INTERRUPT_MODE

/** Set max buffer size 
*/
#define SPI_T_BUFFER_MAX_SIZE 10
#define SPI_R_BUFFER_MAX_SIZE 10

#endif /* SPI_CONFIG_H_ */