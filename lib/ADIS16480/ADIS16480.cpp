////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Analog Devices, Inc.
//  May 2015
//  By: Daniel H. Tatum & Juan Chong
//  Written for the TeensyDuino Platform
////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ADIS16480.cpp
////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//  This file is part of Interfacing ADIS16480 with Arduino example.
//
//  Interfacing ADIS16480 with Arduino example is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Interfacing ADIS16480 with Arduino example is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser Public License for more details.
//
//  You should have received a copy of the GNU Lesser Public License
//  along with Interfacing ADIS16480 with Arduino example.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ADIS16480.h"

////////////////////////////////////////////////////////////////////////////
// ADIS16480(int CS)
////////////////////////////////////////////////////////////////////////////
// Constructor with configurable CS pin and creates _CS private variable
////////////////////////////////////////////////////////////////////////////
// CS - Chip select pin
// DR - DR output pin for data ready
// RST - Hardware reset pin
////////////////////////////////////////////////////////////////////////////
ADIS16480::ADIS16480(int CS, int DR, int RST) {
  _CS = CS;
  _DR = DR;
  _RST = RST;
  pinMode(_CS, OUTPUT); // Set CS pin to be an output
  pinMode(_DR, INPUT); // Set DR pin to be an input
  pinMode(_RST, OUTPUT); // Set RST pin to be an output
  digitalWrite(_CS, HIGH); // Initialize CS pin to be high
  digitalWrite(_RST, HIGH); // Initialize RST pin to be high
}

////////////////////////////////////////////////////////////////////////////
//                           Destructor
////////////////////////////////////////////////////////////////////////////
ADIS16480::~ADIS16480() {
  // Put device to sleep
  //sleep();
  // Close SPI bus
  SPI.end();
}

////////////////////////////////////////////////////////////////////////////
// reset()
////////////////////////////////////////////////////////////////////////////
// Performs hardware reset by sending _RST pin low for 2 seconds
////////////////////////////////////////////////////////////////////////////
int ADIS16480::reset(uint16_t ms) {
  digitalWrite(_RST, LOW);
  delayMicroseconds(500);
  digitalWrite(_RST, HIGH);
  delay(ms);
  return(1);
}

////////////////////////////////////////////////////////////////////////////
// tare()
////////////////////////////////////////////////////////////////////////////
// Tares IMU
////////////////////////////////////////////////////////////////////////////
int ADIS16480::tare() {
  regWrite(GLOB_CMD, 0x100);
  delay(10); 
  return (1);
}

////////////////////////////////////////////////////////////////////////////
// void configSPI()
////////////////////////////////////////////////////////////////////////////
// Sets SPI bit order, clock divider, and data mode. This function is useful
// when there are multiple SPI devices using different settings.
////////////////////////////////////////////////////////////////////////////
int ADIS16480::configSPI() {
  SPISettings IMUSettings(1000000, MSBFIRST, SPI_MODE3);
  SPI.beginTransaction(IMUSettings);
  return(1);
}

////////////////////////////////////////////////////////////////////////////////////////////
// Reads two bytes (one word) in two sequential registers over SPI
////////////////////////////////////////////////////////////////////////////////////////////
// regAddr - address of register to be read
// return - (int) signed 16 bit 2's complement number
////////////////////////////////////////////////////////////////////////////////////////////
uint16_t ADIS16480::regRead(uint16_t regAddr) {
  // Separate page ID from register address
  uint8_t page = ((regAddr >> 8) & 0xFF);
  uint8_t address = (regAddr & 0xFF);

  // Check whether the sensor is currently on the requested page
  if (currentPage != page) {
    // Write desired page to PAGE_ID register
    digitalWrite(_CS, LOW); // Set CS low to enable device
    SPI.transfer(0x80); // Write high byte from low word to SPI bus
    SPI.transfer(page); // Write low byte from low word to SPI bus
    digitalWrite(_CS, HIGH); // Set CS high to disable device
    // Write new current page to tracking variable
    currentPage = page; 
    delayMicroseconds(_stall); // Stall time delay
  }

  // Write desired register address
  digitalWrite(_CS, LOW); // Set CS low to enable device
  SPI.transfer(address); // Write address over SPI bus
  SPI.transfer(0x00); // Write 0x00 to the SPI bus fill the 16 bit transaction requirement
  digitalWrite(_CS, HIGH); // Set CS high to disable device

  delayMicroseconds(_stall); // Stall time delay

  // Read data from requested register
  digitalWrite(_CS, LOW); // Set CS low to enable device
  uint16_t _dataOut = (SPI.transfer(0x00) << 8) | (SPI.transfer(0x00) & 0xFF); // Concatenate upper and lower bytes
  digitalWrite(_CS, HIGH); // Set CS high to disable device

  delayMicroseconds(_stall); // Stall time delay

  //uint16_t _dataOut = (_msbData << 8) | (_lsbData); // Concatenate upper and lower bytes
  // Shift MSB data left by 8 bits, mask LSB data with 0xFF, and OR both bits.

  return(_dataOut);
}


////////////////////////////////////////////////////////////////////////////
// Writes one byte of data to the specified register over SPI.
// Returns 1 when complete.
////////////////////////////////////////////////////////////////////////////
// regAddr - address of register to be written
// regData - data to be written to the register
////////////////////////////////////////////////////////////////////////////
int ADIS16480::regWrite(uint16_t regAddr, int16_t regData) {
  // Separate page ID from register address
  uint8_t page = ((regAddr >> 8) & 0xFF);
  uint8_t address = (regAddr & 0xFF);

  // Check whether the sensor is currently on the requested page
  if (currentPage != page) {
    // Write desired page to PAGE_ID register
    digitalWrite(_CS, LOW); // Set CS low to enable device
    SPI.transfer(0x80); // Write high byte from low word to SPI bus
    SPI.transfer(page); // Write low byte from low word to SPI bus
    digitalWrite(_CS, HIGH); // Set CS high to disable device
    // Write new current page to tracking variable
    currentPage = page; 
    delayMicroseconds(_stall); // Stall time delay
  }

  // Sanity-check address and register data
  uint16_t addr = (((address & 0x7F) | 0x80) << 8); // Toggle sign bit, and check that the address is 8 bits
  uint16_t lowWord = (addr | (regData & 0xFF)); // OR Register address (A) with data(D) (AADD)
  uint16_t highWord = ((addr | 0x100) | ((regData >> 8) & 0xFF)); // OR Register address with data and increment address

  // Write highWord to SPI bus
  digitalWrite(_CS, LOW); // Set CS low to enable device
  SPI.transfer(lowWord >> 8); // Write high byte from low word to SPI bus
  SPI.transfer(lowWord & 0xFF); // Write low byte from low word to SPI bus
  digitalWrite(_CS, HIGH); // Set CS high to disable device

  delayMicroseconds(_stall); // Stall time delay

  // Write lowWord to SPI bus
  digitalWrite(_CS, LOW); // Set CS low to enable device
  SPI.transfer(highWord >> 8); // Write high byte from high word to SPI bus
  SPI.transfer(highWord & 0xFF); // Write low byte from high word to SPI bus
  digitalWrite(_CS, HIGH); // Set CS high to disable device

  delayMicroseconds(_stall); // Stall time delay

  return(1);
}

//////////////////////////////////////////////////////////////////////////////
// closeSPI()
//////////////////////////////////////////////////////////////////////////////
// Ends the current SPI transaction, allowing different settings to be loaded
//////////////////////////////////////////////////////////////////////////////
int ADIS16480::closeSPI(){
  SPI.endTransaction();
  return(1);
}

//////////////////////////////////////////////////////////////////////////////
// dummySPIWrite()
//////////////////////////////////////////////////////////////////////////////
// A dummy write which does not trigger CS. Used when the SPI mode is changed
//////////////////////////////////////////////////////////////////////////////
void ADIS16480::dummySPIWrite(){
  SPI.transfer(0x00); // Write dummy data to force SPI phase/polarity change
}