////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Analog Devices, Inc.
//  October 2015
//  By: Daniel H. Tatum & Juan Chong
//  Written for the TeensyDuino Platform
////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Arduino_RX_ADF7242.ino
////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//  This program received packets transmitted from the Arduino_TX_ADIS16480_ADF7242.ino program
//  and relays them to a USB virtual COM port on the PC.
//
//  Arduino_RX_ADF7242.ino is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Arduino_RX_ADF7242.ino is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser Public License for more details.
//
//  You should have received a copy of the GNU Lesser Public License
//  along with Arduino_RX_ADF7242.ino.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <ADF7242.h>
#include <SPI.h>

//#define DEBUG // Comment out this line to disable DEBUG mode

// Variables
unsigned char roll = 0;
unsigned char pitch = 0;
unsigned char yaw = 0;
unsigned char serialSyncWord = 0xFF; // Used to synchronize serial data received by GUI on PC

ADF7242 Rx(10); // Instantiate ADF7242 Rx(Chip Select)

void setup() {

  // For serial communication to the PC via USB
  Serial.begin(9600); // Baud rate was set arbitrarily
  SPI.begin();        // Begin SPI

  // Reminder in terminal that you're in DEBUG mode
  #ifdef DEBUG
    Serial.println("**********DEBUG MODE**********");
  #endif

  // ADF7242 RFIC configuration
  Rx.configSPI();           //Begin SPI Transaction
  Rx.reset();               // Reset ADF7242 transceiver during cold start up
  Rx.idle();                // Idle ADF7242 transceiver after cold start up

  // Initialize settings for GFSK/FSK
  Rx.initFSK(5);            // Data rate [ 1=50kbps, 2=62.5kbps, 3=100kbps, 4=125kbps, 5=250kbps, 6=500kbps, 7=1Mbps, 8=2Mbps ]
  Rx.setMode(0x04);         // Set operating mode to GFSK/FSK packet mode
  //Rx.initIEEE();
  Rx.chFreq(2450);          // Set operating frequency in MHz
  Rx.syncWord(0x00, 0x00);  // Set sync word // sync word currently hardcoded
  Rx.cfgPA(3, 0, 7);        // Configure power amplifier (power, high power mode, ramp rate)
  Rx.cfgAFC(80);            // Writes AFC configuration for GFSK / FSK
  Rx.cfgPB(0x080, 0x000);   // Sets Tx/Rx packet buffer pointers
  Rx.cfgCRC(0);             // CRC - Disable automatic CRC = 1, else 0
  Rx.cfgBasicPreamble();    // FSK preamble configuration
  Rx.PHY_RDY();             // System calibration
  Rx.receive();             // Set transceiver to receive mode
  
  // Clear receive buffer to all 0x00
  for(int i = 0x000; i < 0x005; ++i) {
    Rx.regWrite(i, 0x00);
  }
  
}

void loop() {
  
  #ifndef DEBUG // If NOT in DEBUG mode
  
    Rx.receive();
    delay(2);
    // Only output data to the serial port if the CRC matches and data is valid
    if((Rx.statusRead() & 0x40) == 0x40) {
      roll = Rx.regRead(0x002);
      pitch = Rx.regRead(0x003);
      yaw = Rx.regRead(0x004);
      Serial.write(roll); // Write roll data to serial connection
      Serial.write((pitch * -1)); // Write pitch data to serial connection
      Serial.write(yaw); // Write yaw data to serial connection
      Serial.write(0xFF); // Write synchronization word to serial connection
    }
    
  #endif

  // Write IMU data and status to serial port
  #ifdef DEBUG // If IN DEBUG mode
    Rx.receive();
    Serial.print(Rx.statusRead());
    Rx.dumpISB();
    for(int i = 0x000; i < 0x005; ++i) {
      int recPac = Rx.regRead(i);
      Serial.print("Packet buffer contencts for address 0x");
      Serial.print(i, HEX);
      Serial.print(" : 0x");
      Serial.println(recPac, HEX);
    }
  #endif
  
  delay(5);
  
}
