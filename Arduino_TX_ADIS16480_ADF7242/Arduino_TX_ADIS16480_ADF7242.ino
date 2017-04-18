////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Analog Devices, Inc.
//  April 17, 2017
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

#include <ADF7242.h>
#include <ADIS16480.h>
#include <SPI.h>

//#define DEBUG // Comment out this line to disable DEBUG mode

// Define Variables
unsigned char roll = 0;
unsigned char pitch = 0;
unsigned char yaw = 0;
unsigned char serialSyncWord = 0xFF; // Used to synchronize serial data received by GUI on PC

ADF7242 Tx(7); // Instantiate ADF7242 Tx(Chip Select)
ADIS16480 IMU(10,8,6); // Instantiate ADIS16480 IMU(Chip Select, Data Ready, HW Reset) 
//10,2,6 when using the development platform

void setup() {
  
  SPI.begin(); //Start SPI
  Serial.begin(115200); //Start USB Serial

  // ADIS16480 IMU configuration
  delay(600);
  IMU.configSPI();          // Begin the SPI transaction
  IMU.dummySPIWrite();      // Dummy write to force SPI Mode change
  IMU.reset(2000);          // Reset ADIS16480 during cold start up
  IMU.regRead(PROD_ID);     // Read the product ID register
  IMU.regWrite(FNCTIO_CTRL, 0x0D); // Enable data ready on DIO2 (0x0D)
  IMU.regWrite(DEC_RATE, 0x51); // Set decimation to 30Hz
  //IMU.tare();               // Tare the ADIS16480 during cold start up
  IMU.closeSPI();           // End the SPI transaction

  // ADF7242 RFIC configuration
  Tx.configSPI();           // Begin the SPI transaction
  Tx.dummySPIWrite();       // Dummy SPI write to force a SPI mode update
  Tx.reset();               // Reset ADF7242 transceiver during cold start up
  Tx.idle();                // Idle ADF7242 transceiver after cold start up

  // Initialize settings for GFSK/FSK
  Tx.initFSK(5);            // Data rate [ 1=50kbps, 2=62.5kbps, 3=100kbps, 4=125kbps, 5=250kbps, 6=500kbps, 7=1Mbps, 8=2Mbps ]
  Tx.setMode(0x04);         // Set operating mode to GFSK/FSK packet mode
  //Tx.initIEEE();
  Tx.chFreq(2450);          // Set operating frequency in MHz
  Tx.syncWord(0x00, 0x00);  // Set sync word // sync word currently hardcoded
  Tx.cfgPA(15, 1, 7);       // Configure power amplifier (power, high power mode, ramp rate)
  Tx.cfgAFC(80);            // Writes AFC configuration for GFSK / FSK
  Tx.cfgCRC(0);             // CRC - Disable automatic CRC = 1, else 0
  Tx.cfgBasicPreamble();    // FSK preamble configuration
  
  // Configure TX packet buffer
  Tx.cfgPB(0x080, 0x000);   // Sets Tx/Rx packet buffer pointers
  Tx.regWrite(0x081, 0x05); // Set packet length LSB
  Tx.regWrite(0x080, 0x00); // Set packet length MSB
  Tx.PHY_RDY();             // System calibration
  Tx.closeSPI();            // End the SPI transaction
  
  // Set interrupt pin on the MCU as an input and attach an interrupt
  attachInterrupt(8, transmitData, RISING); //Use GPIO 2 when using the development platform
}

// Read IMU data, cast it, and transmit it.
void grabSensorData() {
  IMU.configSPI();          // Begin SPI transactions
  IMU.dummySPIWrite();      // Dummy write to force SPI Mode change
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
  IMU.closeSPI(); // End SPI transaction
}

// Transmit IMU data via ADF7242
void sendWirelessSensorData() {
  Tx.configSPI(); // Begin SPI transaction
  Tx.dummySPIWrite(); // Dummy write to force SPI Mode change
  delay(1);
  Tx.regWrite(0x082, roll); // Write roll data to ADF7242 packet buffer
  Tx.regWrite(0x083, pitch); // Write pitch data to ADF7242 packet buffer
  Tx.regWrite(0x084, yaw); // Write yaw data to ADF7242 packet buffer
  Tx.transmit();  // Transmit packet buffer
  Tx.closeSPI();  // End SPI transaction
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
  sendWirelessSensorData();
  sendSerialSensorData();
}

void loop() {
  
  // Program is entirely interrupt driven. Nothing to do here!
  
}

