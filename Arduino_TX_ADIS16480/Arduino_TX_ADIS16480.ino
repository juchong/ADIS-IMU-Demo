////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Analog Devices, Inc.
//  April 27, 2018
//  By: Daniel H. Tatum & Juan J. Chong
//  Written for the TeensyDuino Platform
////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Arduino_TX_ADIS16480_ADF7242.ino
////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//  This program utilizes both the ADIS16480 and ADF7242 libraries included with this package in order
//  to interface with the aforementioned devices and transmit pitch, roll, and yaw data to a PC.
//
//  Arduino_TX_ADIS16480_ADF7242.ino is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Arduino_TX_ADIS16480_ADF7242.ino is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser Public License for more details.
//
//  You should have received a copy of the GNU Lesser Public License
//  along with Arduino_TX_ADIS16480_ADF7242.ino.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <ADIS16480.h>
#include <SPI.h>

//#define DEBUG // Comment out this line to disable DEBUG mode

// Define Variables
unsigned char roll = 0;
unsigned char pitch = 0;
unsigned char yaw = 0;
unsigned char serialSyncWord = 0xFF; // Used to synchronize serial data received by GUI on PC
unsigned char temp = 0;

ADIS16480 IMU(10,8,6); // Instantiate ADIS16480 IMU(Chip Select, Data Ready, HW Reset) 
//10,2,6 when using the development platform

void setup() {
  
  SPI.begin(); //Start SPI
  Serial.begin(115200); //Start USB Serial

  // ADIS16480 IMU configuration
  delay(600);
  IMU.configSPI();          // Begin the SPI transaction
  //IMU.dummySPIWrite();      // Dummy write to force SPI Mode change
  IMU.reset(2000);          // Reset ADIS16480 during cold start up
  
  // DEBUG CODE //
  /*
  temp = IMU.regRead(PROD_ID);     // Read the product ID register
  Serial.println(temp);
  IMU.regWrite(FNCTIO_CTRL, 0x0D); // Enable data ready on DIO2 (0x0D)
  temp = IMU.regRead(FNCTIO_CTRL);
  Serial.println(temp);
  IMU.regWrite(DEC_RATE, 0x51); // Set decimation to 30Hz
  temp = IMU.regRead(DEC_RATE);
  Serial.println(temp);
  delay(2000);
  */

  // Set interrupt pin on the MCU as an input and attach an interrupt
  attachInterrupt(8, transmitData, RISING); //Use GPIO 2 when using the development platform
}

// Read IMU data, cast it, and transmit it.
void grabSensorData() {
  roll = (char)(IMU.regRead(ROLL_C23_OUT) >> 8);  // Read roll register and cast to char
  pitch = (char)(IMU.regRead(PITCH_C31_OUT) >> 8);  // Read pitch register and cast to char
  yaw = (char)(IMU.regRead(YAW_C32_OUT) >> 8);  // Read yaw register and cast to char
  // 0xFF is a reserved word used for data synchronization
  if(roll == 0xFF) { // 0xFF represents 360 degrees
    roll = 0; // This makes sense since 0 and 360 degrees are the same place
  }
  if(pitch == 0xFF) { // 0xFF represents 360 degrees
    pitch = 0; // This makes sense since 0 and 360 degrees are the same place
  }
  if(yaw == 0xFF) { // 0xFF represents 360 degrees
    yaw = 0; // This makes sense since 0 and 360 degrees are the same place
  }
}

// Transmit IMU data via USB Serial port.
void sendSerialSensorData() {
  Serial.write(roll); // Write roll data to serial connection
  Serial.write(pitch); // Write pitch data to serial connection
  Serial.write(yaw); // Write yaw data to serial connection
  Serial.write(serialSyncWord); // Write synchronization word to serial connection
}

// Interrupt routine will grab data from the IMU and transmit it via the ADF7242 and SPI
void transmitData() {
  grabSensorData();
  sendSerialSensorData();
}

void loop() {
  
  // Program is entirely interrupt driven. Nothing to do here!
  
}

