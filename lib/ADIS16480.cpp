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
void ADIS16480::reset() {
  digitalWrite(_RST, LOW);
  digitalWrite(_RST, HIGH);
  delay(2000);
}

////////////////////////////////////////////////////////////////////////////
// tare()
////////////////////////////////////////////////////////////////////////////
// Tares IMU
////////////////////////////////////////////////////////////////////////////
void ADIS16480::tare() {
  configSPI();
  // Set page
  digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADIS16480
  SPI.transfer(0x80); // Memory write
  SPI.transfer((unsigned char)(GLOB_CMD << 8)); // Change page
  digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADIS16480

  // Write lower byte
  digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADIS16480
  SPI.transfer(0x80 | (unsigned char)GLOB_CMD); // write to GLOB_CMD lower byte
  SPI.transfer(0x00);
  digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADIS16480

  // Write upper byte
  digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADIS16480
  SPI.transfer(0x80 | ((unsigned char)GLOB_CMD) + 1); // write to GLOB_CMD upper byte
  SPI.transfer(0x01);
  digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADIS16480
}

////////////////////////////////////////////////////////////////////////////
// void configSPI()
////////////////////////////////////////////////////////////////////////////
// Sets SPI bit order, clock divider, and data mode. This function is useful
// when there are multiple SPI devices using different settings.
////////////////////////////////////////////////////////////////////////////
void ADIS16480::configSPI() {
  SPISettings IMUSettings(1000000, MSBFIRST, SPI_MODE3);
  SPI.beginTransaction(IMUSettings);
}

////////////////////////////////////////////////////////////////////////////
// unsigned int pageRead()
////////////////////////////////////////////////////////////////////////////
// Reads current page
////////////////////////////////////////////////////////////////////////////
// return - current page
////////////////////////////////////////////////////////////////////////////
unsigned char ADIS16480::pageRead() {
  configSPI();
  // Set page
  digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADIS16480
  SPI.transfer(SPI_NOP); // SPI_NOP = 0x00
  SPI.transfer(SPI_NOP); // SPI_NOP = 0x00
  digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADIS16480

  // Read Data
  digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADIS16480
  SPI.transfer(SPI_NOP); // SPI_NOP = 0x00
  unsigned char _dataRead = SPI.transfer(SPI_NOP); // read page
  digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADIS16480

  #ifdef DEBUG
    Serial.print("Current page: 0x0");
    Serial.println(_dataRead, HEX);
  #endif

  return(_dataRead);
}

////////////////////////////////////////////////////////////////////////////
// unsigned int regRead(unsigned int regAddr)
////////////////////////////////////////////////////////////////////////////
// Reads 1 16 bit integer of data to the specified register over SPI
////////////////////////////////////////////////////////////////////////////
// regAddr - memory page is bits [15..8], register address is bits[7..0]
// return - integer of data
////////////////////////////////////////////////////////////////////////////
unsigned int ADIS16480::regRead(unsigned int regAddr) {
  //configSPI();
  // Set page
  digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADIS16480
  SPI.transfer(0x80); // Memory write
  SPI.transfer((unsigned char)(regAddr << 8)); // Change page
  digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADIS16480

  // Set register
  digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADIS16480
  SPI.transfer((unsigned char)regAddr); // 
  SPI.transfer(SPI_NOP);
  digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADIS16480

  // Read Data
  digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADIS16480
  unsigned int _dataRead = (SPI.transfer(SPI_NOP) << 8) | SPI.transfer(SPI_NOP);
  digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADIS16480

  #ifdef DEBUG
    Serial.print("Register 0x");
    Serial.print((unsigned char)regAddr, HEX);
    Serial.print(" on page 0x");
    Serial.print((unsigned char)(regAddr << 8), HEX);
    Serial.print(" reads: 0x");
    Serial.println(_dataRead);
  #endif

  return(_dataRead);
}

//////////////////////////////////////////////////////////////////////////////
// closeSPI()
//////////////////////////////////////////////////////////////////////////////
// Ends the current SPI transaction, allowing different settings to be loaded
//////////////////////////////////////////////////////////////////////////////
void ADIS16480::closeSPI(){
  SPI.endTransaction();
}

//////////////////////////////////////////////////////////////////////////////
// dummySPIWrite()
//////////////////////////////////////////////////////////////////////////////
// A dummy write which does not trigger CS. Used when the SPI mode is changed
//////////////////////////////////////////////////////////////////////////////
void ADIS16480::dummySPIWrite(){
  SPI.transfer(SPI_NOP);
}