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

#include "twi.h"
#include "Wire.h"

// Initialize Class Variables //////////////////////////////////////////////////

u8 Wire_rxBuffer[BUFFER_LENGTH];
u8 Wire_rxBufferIndex = 0;
u8 Wire_rxBufferLength = 0;

u8 Wire_txAddress = 0;
u8 Wire_txBuffer[BUFFER_LENGTH];
u8 Wire_txBufferIndex = 0;
u8 Wire_txBufferLength = 0;

u8 Wire_transmitting = 0;
void (*Wire_user_onRequest)(void);
void (*Wire_user_onReceive)(int);

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

void Wire_setClock(u32 clock)
{
  twi_setFrequency(clock);
}

u8 Wire_requestFrom(u8 address, u8 quantity)
{
	// clamp to buffer length
	if(quantity > BUFFER_LENGTH){
		quantity = BUFFER_LENGTH;
	}
	// perform blocking read into buffer
	u8 read = twi_readFrom(address, rxBuffer, quantity, TRUE);
	// set rx buffer iterator vars
	rxBufferIndex = 0;
	rxBufferLength = read;

	return read;
}

void Wire_beginTransmission(u8 address)
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
//	Calling endTransmission(false) allows a sketch to 
//	perform a repeated start. 
//
//	WARNING: Nothing in the library keeps track of whether
//	the bus tenure has been properly ended with a STOP. It
//	is very possible to leave the bus in a hung state if
//	no call to endTransmission(true) is made. Some I2C
//	devices will behave oddly if they do not see a STOP.
//
u8 Wire_endTransmission()
{
  // transmit buffer (blocking)
  u8 ret = twi_writeTo(txAddress, txBuffer, txBufferLength, 1, TRUE);
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
u8 Wire_write(u8 data)
{
  if(transmitting){
  // in master transmitter mode
    // don't bother if buffer is full
    if(txBufferLength >= BUFFER_LENGTH){
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
u8 Wire_read(void)
{
  u8 value = -1;
  
  // get each successive byte on each call
  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
    ++rxBufferIndex;
  }

  return value;
}
