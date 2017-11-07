/*
  TwoWire.cpp - TWI/I2C library for Wiring & Arduino
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 
  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
*/

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "twi.h"
#include "types.h"
#include "Wire.h"

// Initialize Class Variables //////////////////////////////////////////////////

uint8_t rxBuffer[BUFFER_LENGTH];
uint8_t rxBufferIndex = 0;
uint8_t rxBufferLength = 0;

uint8_t txAddress = 0;
uint8_t txBuffer[BUFFER_LENGTH];
uint8_t txBufferIndex = 0;
uint8_t txBufferLength = 0;

uint8_t transmitting = 0;
void (*user_onRequest)(void);
void (*user_onReceive)(int);

// Constructors ////////////////////////////////////////////////////////////////

// Public Methods //////////////////////////////////////////////////////////////

void Wire_begin(void)
{
  rxBufferIndex = 0;
  rxBufferLength = 0;

  txBufferIndex = 0;
  txBufferLength = 0;

  twi_init();
}

void Wire_end(void)
{
  twi_disable();
}

void Wire_setClock(uint32_t clock)
{
  twi_setFrequency(clock);
}

uint8_t Wire_requestFrom(uint8_t address, uint8_t quantity)
{

	// clamp to buffer length
	if(quantity > BUFFER_LENGTH){
		quantity = BUFFER_LENGTH;
	}
	// perform blocking read into buffer
	uint8_t read = twi_readFrom(address, rxBuffer, quantity, TRUE);
	// set rx buffer iterator vars
	rxBufferIndex = 0;
	rxBufferLength = read;

	return read;
}

void Wire_beginTransmission(uint8_t address)
{
  // indicate that we are transmitting
  transmitting = 1;
  // set address of targeted slave
  txAddress = address;
  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;
}

//
//	Originally, 'endTransmission' was an f(void) function.
//	It has been modified to take one parameter indicating
//	whether or not a STOP should be performed on the bus.
//	Calling endTransmission(FALSE) allows a sketch to 
//	perform a repeated start. 
//
//	WARNING: Nothing in the library keeps track of whether
//	the bus tenure has been properly ended with a STOP. It
//	is very possible to leave the bus in a hung state if
//	no call to endTransmission(TRUE) is made. Some I2C
//	devices will behave oddly if they do not see a STOP.
//

//	This provides backwards compatibility with the original
//	definition, and expected behaviour, of endTransmission
//
uint8_t Wire_endTransmission(void)
{
	// transmit buffer (blocking)
	uint8_t ret = twi_writeTo(txAddress, txBuffer, txBufferLength, 1, FALSE);
	// reset tx buffer iterator vars
	txBufferIndex = 0;
	txBufferLength = 0;
	// indicate that we are done transmitting
	transmitting = 0;
	return ret;
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
u8 Wire_write(uint8_t data)
{
  if(transmitting){
  // in master transmitter mode
    // don't bother if buffer is full
    if(txBufferLength >= BUFFER_LENGTH){
      //setWriteError();
      return 0;
    }
    // put byte in tx buffer
    txBuffer[txBufferIndex] = data;
    ++txBufferIndex;
    // update amount in buffer   
    txBufferLength = txBufferIndex;
  }else{
  // in slave send mode
    // reply to master
    twi_transmit(&data, 1);
  }
  return 1;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int Wire_available(void)
{
  return rxBufferLength - rxBufferIndex;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int Wire_read(void)
{
  int value = -1;
  
  // get each successive byte on each call
  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
    ++rxBufferIndex;
  }

  return value;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int Wire_peek(void)
{
  int value = -1;
  
  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
  }

  return value;
}

void Wire_flush(void)
{
  // XXX: to be implemented.
}

// behind the scenes function that is called when data is received
void Wire_onReceiveService(uint8_t* inBytes, int numBytes)
{
  // don't bother if user hasn't registered a callback
  if(!user_onReceive){
    return;
  }
  // don't bother if rx buffer is in use by a master requestFrom() op
  // i know this drops data, but it allows for slight stupidity
  // meaning, they may not have read all the master requestFrom() data yet
  if(rxBufferIndex < rxBufferLength){
    return;
  }
  // copy twi rx buffer into local read buffer
  // this enables new reads to happen in parallel
  for(uint8_t i = 0; i < numBytes; ++i){
    rxBuffer[i] = inBytes[i];    
  }
  // set rx iterator vars
  rxBufferIndex = 0;
  rxBufferLength = numBytes;
  // alert user program
  user_onReceive(numBytes);
}

// behind the scenes function that is called when data is requested
void Wire_onRequestService(void)
{
  // don't bother if user hasn't registered a callback
  if(!user_onRequest){
    return;
  }
  // reset tx buffer iterator vars
  // !!! this will kill any pending pre-master sendTo() activity
  txBufferIndex = 0;
  txBufferLength = 0;
  // alert user program
  user_onRequest();
}

// sets function called on slave write
void Wire_onReceive( void (*function)(int) )
{
  user_onReceive = function;
}

// sets function called on slave read
void Wire_onRequest( void (*function)(void) )
{
  user_onRequest = function;
}
