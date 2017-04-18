////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Analog Devices, Inc.
//  October 2015
//  By: Daniel H. Tatum & Juan Chong
//  Written for the TeensyDuino Platform
////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ADF7242.cpp
////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//  This file is part of Interfacing ADF7242 with Arduino example.
//
//  Interfacing ADF7242 with Arduino example is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Interfacing ADF7242 with Arduino example is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser Public License for more details.
//
//  You should have received a copy of the GNU Lesser Public License
//  along with Interfacing ADF7242 with Arduino example.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "Arduino.h"
#include <SPI.h>
#include "ADF7242.h"

////////////////////////////////////////////////////////////////////////////
// ADF7242(int CS)
////////////////////////////////////////////////////////////////////////////
// Constructor with configurable CS pin and creates _CS private variable
////////////////////////////////////////////////////////////////////////////
// CS - Chip select pin
////////////////////////////////////////////////////////////////////////////
ADF7242::ADF7242(int CS) {
  _CS = CS;
  pinMode(_CS, OUTPUT); // Set CS pin to be an Output
  digitalWrite(_CS, HIGH); // Initialize CS pin to be high
}

// Destructor
////////////////////////////////////////////////////////////////////////////
// ADF7242()
////////////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////////////
// TODO - Write me!!!
////////////////////////////////////////////////////////////////////////////
ADF7242::~ADF7242() {
  sleep(); // Put the ADF7242 to sleep
  SPI.end(); // End SPI communication with ADF7242
}

////////////////////////////////////////////////////////////////////////////
// void reset()
////////////////////////////////////////////////////////////////////////////
// Resets radio controller and puts it to sleep
////////////////////////////////////////////////////////////////////////////
void ADF7242::reset() {
  digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADF7242
  SPI.transfer(RC_RESET); // Resets the ADF7242 and puts it in the sleep state
  digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADF7242
  delay(3); // Minimum delay as per datasheet is t16 = 2ms
}

////////////////////////////////////////////////////////////////////////////
// void sleep()
////////////////////////////////////////////////////////////////////////////
// Brings radio controller into sleep state
////////////////////////////////////////////////////////////////////////////
void ADF7242::sleep() {
  digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADF7242
  SPI.transfer(RC_SLEEP); // Sleep the ADF7242
  digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADF7242
  delay(3); // Minimum delay as per datasheet is t16 = 2ms
}

////////////////////////////////////////////////////////////////////////////
// void idle()
////////////////////////////////////////////////////////////////////////////
// Brings radio controller into idle state
////////////////////////////////////////////////////////////////////////////
void ADF7242::idle() {
  digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADF7242
  SPI.transfer(RC_IDLE); // Sleep the ADF7242
  delay(1); // allow time for the RC to mellow out
  digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADF7242
}

////////////////////////////////////////////////////////////////////////////
// void PHY_RDY()
////////////////////////////////////////////////////////////////////////////
// Brings radio controller into PHY_RDY state
////////////////////////////////////////////////////////////////////////////
void ADF7242::PHY_RDY() {
	regWrite(vco_cal_cfg, 9); // DO NOT SKIP VCO CAL
  digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADF7242
  SPI.transfer(RC_PHY_RDY); // PHY_RDY the ADF7242
  digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADF7242
}

////////////////////////////////////////////////////////////////////////////
// void CCA()
////////////////////////////////////////////////////////////////////////////
// Performs a clear channel assessment
////////////////////////////////////////////////////////////////////////////
void ADF7242::CCA() {
  digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADF7242
  SPI.transfer(RC_CCA); // CCA the ADF7242
  digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADF7242
}

////////////////////////////////////////////////////////////////////////////
// void receive()
////////////////////////////////////////////////////////////////////////////
// Brings radio controller into receive state
////////////////////////////////////////////////////////////////////////////
void ADF7242::receive() {
  digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADF7242
  SPI.transfer(RC_RX); // receive the ADF7242
  digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADF7242
}

////////////////////////////////////////////////////////////////////////////
// void transmit()
////////////////////////////////////////////////////////////////////////////
// Brings radio controller into transmit state
////////////////////////////////////////////////////////////////////////////
void ADF7242::transmit() {
  digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADF7242
  SPI.transfer(RC_TX); // transmit the ADF7242
  digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADF7242
}

////////////////////////////////////////////////////////////////////////////
// unsigned char meas()
////////////////////////////////////////////////////////////////////////////
// Measures chip temperature
////////////////////////////////////////////////////////////////////////////
// return - chip temperature from register adc_rbk, field adc_out
////////////////////////////////////////////////////////////////////////////
unsigned char ADF7242::meas() {
  digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADF7242
  SPI.transfer(RC_MEAS); // PHY_RDY the ADF7242
  digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADF7242
  #ifdef DEBUG
  	Serial.println(regRead(adc_rbk) & 0x3F, HEX); // print contents of adc_rbk[5..0]
  #endif
  return(regRead(adc_rbk) & 0x3F); // return contents of adc_rbk[5..0]
}

////////////////////////////////////////////////////////////////////////////
// void configSPI()
////////////////////////////////////////////////////////////////////////////
// Sets SPI bit order, clock divider, and data mode. This function is useful
// when there are multiple SPI devices using different settings.
////////////////////////////////////////////////////////////////////////////
void ADF7242::configSPI() {
  SPISettings TXSettings(4000000, MSBFIRST, SPI_MODE0);
  SPI.beginTransaction(TXSettings);
}

unsigned char ADF7242::statusRead() {
  digitalWrite(_CS, LOW);
  SPI.transfer(SPI_NOP);
  unsigned char _status = SPI.transfer(SPI_NOP);
  digitalWrite(_CS, HIGH);
  return(_status);
}

////////////////////////////////////////////////////////////////////////////
// unsigned char regRead(unsigned int regAddr)
////////////////////////////////////////////////////////////////////////////
// Reads 1 byte of data to the specified register over SPI
////////////////////////////////////////////////////////////////////////////
// regAddr - address of register
// return - byte of data
////////////////////////////////////////////////////////////////////////////
// TODO - this function can be flaky and doesn't work inside the DEBUG
//        portion of regWrite
////////////////////////////////////////////////////////////////////////////
unsigned char ADF7242::regRead(unsigned int regAddr) {
  digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADF7242
  SPI.transfer(SPI_MEMR_RD | ( regAddr >> 8 )); // SPI_MEMR_WR + address bits [10:8]
  SPI.transfer(0xFF & regAddr); // Address bits [7:0]
  SPI.transfer(SPI_NOP);
  unsigned char _dataRead = SPI.transfer(SPI_NOP); // Data byte
  digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADF7242
  return(_dataRead);
}

////////////////////////////////////////////////////////////////////////////
// void regWrite(unsigned int regAddr, unsigned char regData)
////////////////////////////////////////////////////////////////////////////
// Writes 1 byte of data to the specified register over SPI
////////////////////////////////////////////////////////////////////////////
// regAddr - address of register
// regAddr - byte of data
////////////////////////////////////////////////////////////////////////////
void ADF7242::regWrite(unsigned int regAddr, unsigned char regData) {
  digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADF7242
  SPI.transfer(SPI_MEMR_WR | ( regAddr >> 8 )); // SPI_MEMR_WR + address bits [10:8]
  SPI.transfer(0xFF & regAddr); // Address bits [7:0]
  SPI.transfer(regData); // Data byte
  digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADF7242
  //digitalWrite(11, LOW); // Software fix for stray capacitance on MOSI line
  //digitalWrite(12, LOW); // Software fix for stray capacitance on MISO line
  delayMicroseconds(25);
  #ifdef DEBUG
    // Read data just written and verify. Errors will appear in serial terminal
    digitalWrite(_CS, LOW); // send CS low to enable SPI transfer to/from ADF7242
    SPI.transfer(SPI_MEMR_RD | ( regAddr >> 8 )); // SPI_MEMR_WR + address bits [10:8]
    SPI.transfer(0xFF & regAddr); // Address bits [7:0]
    SPI.transfer(SPI_NOP);
    unsigned char _dataRead = SPI.transfer(SPI_NOP); // Data byte
    digitalWrite(_CS, HIGH); // send CS high to disable SPI transfer to/from ADF7
    if(_dataRead != regData) {
      Serial.println("!!!!!Reg Write Error!!!!!");
      Serial.print("Register Address: 0x");
      Serial.println(regAddr, HEX);
      Serial.print("Register Data Written: 0x");
      Serial.println(regData, HEX);
      Serial.print("Register Data Read: 0x");
      Serial.println(_dataRead, HEX);
      Serial.println("*************************");
    } // end if(_dataRead != regData)
  #endif
}

////////////////////////////////////////////////////////////////////////////
// void initFSK(unsigned char dataRate)
////////////////////////////////////////////////////////////////////////////
// Load settings common to all GFSK/FSK configurations from table 37
// Load data rate-specific GFSK/FSK settings from table 38
////////////////////////////////////////////////////////////////////////////
// dataRate - case 1: 50kbps FSK
//            case 2: 62.5kbps FSK
//            case 3: 100kbps FSK
//            case 4: 125kbps FSK
//            case 5: 250kbps GFSK
//            case 6: 500kbps GFSK
//            case 7: 1Mbps GFSK
//            case 8: 2Mbps GFSK
////////////////////////////////////////////////////////////////////////////
// TODO - investigate iirf_cfg, agc_cfg5
////////////////////////////////////////////////////////////////////////////
void ADF7242::initFSK(unsigned char dataRate) {
  #ifdef DEBUG
    Serial.println("Settings common to all GFSK/FSK configurations loaded!");
  #endif
  // Table 37. Settings common to all GFSK/FSK configurations
  regWrite(synt,0x28);
  regWrite(agc_cfg1,0x34); // from table 37
  regWrite(agc_max,0x80); // from table 37
  regWrite(agc_cfg2,0x37);
  regWrite(agc_cfg3,0x2A);
  regWrite(agc_cfg4,0x1D);
  regWrite(agc_cfg6,0x24);
  regWrite(agc_cfg7,0x7B);
  regWrite(ocl_cfg0,0x00);
  regWrite(ocl_cfg1,0x07);
  regWrite(irq1_en0, 0x00);
  regWrite(irq1_en1, 0x08); // Enables interrupt to be triggered when valid packet is received
  regWrite(irq2_en0, 0x00);
  regWrite(irq2_en1, 0x00);
  regWrite(irq1_src0, 0xFF);
  regWrite(irq1_src1, 0xFF);
  regWrite(ocl_bw0,0x1A);
  regWrite(ocl_bw1,0x19);
  regWrite(ocl_bw2,0x1E);
  regWrite(ocl_bw3,0x1E);
  regWrite(ocl_bw4,0x1E);
  regWrite(ocl_bws,0x00);
  regWrite(ocl_bw13,0xF0);
  regWrite(preamble_num_validate,0x03);
  #ifdef DEBUG
    Serial.print("Data rate-specific settings loaded: ");
  #endif
  // Table 38. Data rate-specific GFSK/FSK settings 
  switch(dataRate) {
    case 1: // 50kbps FSK
      #ifdef DEBUG
        Serial.println("50kbps FSK");
      #endif
      regWrite(fsk_preamble,0x04);
      regWrite(tx_fd,0x03);
      regWrite(dm_cfg0,0x37);
      regWrite(tx_m,0x00);
      regWrite(dr0,0x01);
      regWrite(dr1,0xF4);
      regWrite(iirf_cfg,0x17); // from table 38
      regWrite(dm_cfg1,0x08);
      regWrite(rxfe_cfg,0x16);
      break;
    case 2: // 62.5kbps FSK
      #ifdef DEBUG
        Serial.println("62.5kbps FSK");
      #endif
      regWrite(fsk_preamble,0x04);
      regWrite(tx_fd,0x06);
      regWrite(dm_cfg0,0x37);
      regWrite(tx_m,0x00);
      regWrite(dr0,0x02);
      regWrite(dr1,0x71);
      regWrite(iirf_cfg,0x17); // from table 38
      regWrite(dm_cfg1,0x08);
      regWrite(rxfe_cfg,0x16);
      break;
    case 3: // 100kbps FSK
      #ifdef DEBUG
        Serial.println("100kbps FSK");
      #endif
      regWrite(fsk_preamble,0x05);
      regWrite(tx_fd,0x03);
      regWrite(dm_cfg0,0x6B);
      regWrite(tx_m,0x00);
      regWrite(dr0,0x03);
      regWrite(dr1,0xE8);
      regWrite(iirf_cfg,0x17);
      regWrite(dm_cfg1,0x0D);
      regWrite(rxfe_cfg,0x16);
      break;
    case 4: // 125kbps FSK
      #ifdef DEBUG
        Serial.println("125kbps FSK");
      #endif
      regWrite(fsk_preamble,0x05);
      regWrite(tx_fd,0x06);
      regWrite(dm_cfg0,0x37);
      regWrite(tx_m,0x00);
      regWrite(dr0,0x04);
      regWrite(dr1,0xE2);
      regWrite(iirf_cfg,0x17);
      regWrite(dm_cfg1,0x11);
      regWrite(rxfe_cfg,0x16);
      break;
    case 5: // 250kbps GFSK
      #ifdef DEBUG
        Serial.println("250kbps GFSK");
      #endif
      regWrite(fsk_preamble,0x05);
      regWrite(tx_fd,0x0D);
      regWrite(dm_cfg0,0x19);
      regWrite(tx_m,0x02);
      regWrite(dr0,0x09);
      regWrite(dr1,0xC4);
      regWrite(iirf_cfg,0x12);
      regWrite(dm_cfg1,0x20);
      regWrite(rxfe_cfg,0x16);
      break;
    case 6: // 500kbps GFSK
      #ifdef DEBUG
        Serial.println("500kbps GFSK");
      #endif
      regWrite(fsk_preamble,0x05);
      regWrite(tx_fd,0x19);
      regWrite(dm_cfg0,0x0D);
      regWrite(tx_m,0x03);
      regWrite(dr0,0x13);
      regWrite(dr1,0x88);
      regWrite(iirf_cfg,0x0A);
      regWrite(dm_cfg1,0x3D);
      regWrite(rxfe_cfg,0x16);
      break;
    case 7: // 1Mbps GFSK
      #ifdef DEBUG
        Serial.println("1Mbps GFSK");
      #endif
      regWrite(fsk_preamble,0x07);
      regWrite(tx_fd,0x19);
      regWrite(dm_cfg0,0x0D);
      regWrite(tx_m,0x03);
      regWrite(dr0,0x27);
      regWrite(dr1,0x10);
      regWrite(iirf_cfg,0x05);
      regWrite(dm_cfg1,0x6E);
      regWrite(rxfe_cfg,0x16);
      break;
    case 8: // 2Mbps GFSK
      #ifdef DEBUG
        Serial.println("2Mbps GFSK");
      #endif
      regWrite(fsk_preamble,0x09);
      regWrite(tx_fd,0x32);
      regWrite(dm_cfg0,0x06);
      regWrite(tx_m,0x03);
      regWrite(dr0,0x4E);
      regWrite(dr1,0x20);
      regWrite(iirf_cfg,0x05);
      regWrite(dm_cfg1,0xAA);
      regWrite(rxfe_cfg,0x1D);
      break;
  }
}

////////////////////////////////////////////////////////////////////////////
// void chFreq(long freq)
////////////////////////////////////////////////////////////////////////////
// Transceiver frequency in MHz
////////////////////////////////////////////////////////////////////////////
// freq - frequency from 2400 MHz - 2500 MHz
////////////////////////////////////////////////////////////////////////////
// TODO - reduce input argument to type int
////////////////////////////////////////////////////////////////////////////
void ADF7242::chFreq(long freq) {
  if (freq >= 2400L && freq <= 2500L)
  {
    regWrite(ch_freq0, (freq * 100L) & 0xFC);
    regWrite(ch_freq1, ((freq * 100L) >> 8) & 0xFF);
    regWrite(ch_freq2, ((freq * 100L) >> 16) & 0xFF);
  } else {
    #ifdef DEBUG
      Serial.println("ERROR: Invalid frequency input!");
    #endif    
  }
	#ifdef DEBUG
		Serial.print("Operating frequency read from ch_freq2..0: ");
		Serial.print( ( (long(regRead(ch_freq2)) << 16) | (long(regRead(ch_freq1)) << 8) | long(regRead(ch_freq0)) )/100L );
    Serial.println("MHz");
	#endif
}

////////////////////////////////////////////////////////////////////////////
// void setMode(unsigned char mode)
////////////////////////////////////////////////////////////////////////////
// Operating mode: IEEE 802.15.4-2006 packet, SPORT; GFSK/FSK packet, SPORT
////////////////////////////////////////////////////////////////////////////
// mode - 0x00: IEEE 802.15.4-2006 packet
// mode - 0x02: IEEE 802.15.4-2006 SPORT
// mode - 0x03: IEEE GFSK/FSK SPORT
// mode - 0x04: IEEE GFSK/FSK packet
////////////////////////////////////////////////////////////////////////////
void ADF7242::setMode(unsigned char mode) {
  if(mode == 0x00 || mode == 0x02 || mode == 0x03 || mode == 0x04) {
    regWrite(rc_cfg, mode);
  } else {
    #ifdef DEBUG
      Serial.println("ERROR: Invalid mode input!");
    #endif
  }
  #ifdef DEBUG
    Serial.print("Operating mode read from rc_cfg: ");
    Serial.println(regRead(rc_cfg),HEX);
  #endif
}

////////////////////////////////////////////////////////////////////////////
// void syncWord(unsigned long word)
////////////////////////////////////////////////////////////////////////////
// Writes sync word configuration
////////////////////////////////////////////////////////////////////////////
// word - sync word up to 24 bits
// tol - sync word error tolerance from 0..3 errors
////////////////////////////////////////////////////////////////////////////
// TODO - put sync word as input argument
// TODO - set length, error tolerance in sync_config
////////////////////////////////////////////////////////////////////////////
void ADF7242::syncWord(unsigned long word, unsigned char tol) {
  regWrite(sync_word0, 0x31); // hardcoded sync word
  regWrite(sync_word1, 0x7F); // hardcoded sync word
  regWrite(sync_word2, 0xAA); // hardcoded sync word
  regWrite(sync_config, 0x10);  // hardcoded sync word tolerance (JC-Was 0x10, but should be 3 words)
  #ifdef DEBUG
    Serial.print("Sync word length read from sync_config, field sync_len: ");
    Serial.println(regRead(sync_config) & 0x1F);
    Serial.print("Sync word read from sync_word2: 0x");
    Serial.println(regRead(sync_word2),HEX);
    Serial.print("Sync word read from sync_word1: 0x");
    Serial.println(regRead(sync_word1),HEX);
    Serial.print("Sync word read from sync_word0: 0x");
    Serial.println(regRead(sync_word0),HEX);
  #endif
}

////////////////////////////////////////////////////////////////////////////
// void cfgPA(unsigned char pwr, bool hp, unsigned char rr)
////////////////////////////////////////////////////////////////////////////
// Writes PA configuration
////////////////////////////////////////////////////////////////////////////
// pwr - PA power level from 3..15
// hp - High power mode = 1 else low power
// rr - PA ramp rate
////////////////////////////////////////////////////////////////////////////
// TODO - regWrite(ext_ctrl, ); // placeholder for PA rampdown
// TODO - Fix datasheet p. 99 table 104 bit 6 = 1 on reset
////////////////////////////////////////////////////////////////////////////
void ADF7242::cfgPA(unsigned char pwr, bool hp, unsigned char rr) {
  // PA power level
  if (pwr <= 15) {
    regWrite(extpa_msc, (pwr << 4) | 0x01); //regWrite(pa_rr, ); // (2^pa_rr)*2.4ns per PA power step
  } else {
    #ifdef DEBUG
      Serial.println("ERROR: Invalid output power input!");
    #endif
  }
  // PA bias for high power
  if (hp == 1) {
    regWrite(pa_cfg, 21 | 0x40); // 21 is max for +4.8dBm output power, bit 6 should be 1
    regWrite(pa_bias, (63 << 1) | 0x01); // 63 is max for +4.8dBm output power
  } else {
    regWrite(pa_cfg, 13 | 0x40); // Default output power on reset, bit 6 should be 1
    regWrite(pa_bias, (55 << 1) | 0x01); // Default output power on reset
  }
  // PA ramp rate
  if (rr <= 7) {
    regWrite(pa_rr, rr); // (2^pa_rr)*2.4ns per PA power step
  } else {
    #ifdef DEBUG
      Serial.println("ERROR: Invalid ramp rate input!");
    #endif
  }
  #ifdef DEBUG
    Serial.print("Register extpa_msc: 0x");
    Serial.println(regRead(extpa_msc), HEX);
    Serial.print("Register pa_cfg: 0x");
    Serial.println(regRead(pa_cfg), HEX);
    Serial.print("Register pa_bias: 0x");
    Serial.println(regRead(pa_bias), HEX);
    Serial.print("Register pa_rr: 0x");
    Serial.println(regRead(pa_rr), HEX);
  #endif
}

////////////////////////////////////////////////////////////////////////////
// void cfgAFC(unsigned char range)
////////////////////////////////////////////////////////////////////////////
// Writes AFC configuration for GFSK / FSK
////////////////////////////////////////////////////////////////////////////
// range - AFC pull-in range in kHz
//         Should be set to half of the receive baseband filter
////////////////////////////////////////////////////////////////////////////
void ADF7242::cfgAFC(unsigned char range) {
  regWrite(afc_cfg, 0x07); // Sets AFC polarity to 1 and locks AFC on preamble detection
  regWrite(afc_ki_kp, 0x99); // Sets the AFC PI controller proportional and integral gain
  regWrite(afc_range, range); // Limits the AFC pull-in range. Should be set to half of the receive baseband filter BW
  #ifdef DEBUG
    Serial.print("Register afc_cfg: 0x");
    Serial.println(regRead(afc_cfg), HEX);
    Serial.print("Register afc_ki_kp: 0x");
    Serial.println(regRead(afc_ki_kp), HEX);
    Serial.print("Register afc_range: ");
    Serial.print(regRead(afc_range));
    Serial.println(" kHz");
    Serial.print("Register afc_read: 0x");
    Serial.println(regRead(afc_read), HEX);
  #endif
}

////////////////////////////////////////////////////////////////////////////
// void cfgPB(unsigned char Tx, unsigned char Rx)
////////////////////////////////////////////////////////////////////////////
// Sets Tx/Rx packet buffer pointers
////////////////////////////////////////////////////////////////////////////
// Tx - transmit packet buffer pointer
// Rx - receive packet buffer pointer
////////////////////////////////////////////////////////////////////////////
void ADF7242::cfgPB(unsigned char Tx, unsigned char Rx) {
  regWrite(txpb, Tx); // Sets transmit packet buffer pointer
  regWrite(rxpb, Rx); // Sets receive packet buffer pointer
  #ifdef DEBUG
    Serial.print("Register txpb: 0x");
    Serial.println(regRead(txpb), HEX);
    Serial.print("Register rxpb: 0x");
    Serial.println(regRead(rxpb), HEX);
  #endif
}

////////////////////////////////////////////////////////////////////////////
// void cfgCRC(bool CRC)
////////////////////////////////////////////////////////////////////////////
// Disable automatic CRC append to packets
////////////////////////////////////////////////////////////////////////////
// CRC - Disable automatic CRC = 1, else 0
////////////////////////////////////////////////////////////////////////////
void ADF7242::cfgCRC(bool CRC) {
  if(CRC == 1 || CRC == 0) {
    regWrite(pkt_cfg, CRC | (2 << 1)); // see notes on configuration of this register in datasheet
  }
  #ifdef DEBUG
    Serial.print("Register pkt_cfg: 0x");
    Serial.println(regRead(pkt_cfg), HEX);
  #endif
}

////////////////////////////////////////////////////////////////////////////
// void cfgPreamble(bool syncWordDetect, bool lockAGC, bool preambleDetect, unsigned char preambleErrors)
////////////////////////////////////////////////////////////////////////////
// FSK preamble configuration
////////////////////////////////////////////////////////////////////////////
// syncWordDetect - Bypass SFD detection (GFSK/FSK SPORT mode only)
//                  0: perform sync word detection
//                  1: skip sync word detection
// lockAGC - Lock AGC after preamble (GFSK/FSK packet/SPORT modes only)
//           0: disable AGC lock
//           1: enable AGC lock
// preambleDetect - Bypass preamble detection and qualification; only search for SWD 
//                  0: enable preamble detection + qualification
//                  1: disable preamble detection + qualification
// preambleErrors - Number of bit-pairs in error allowed in 12 bit-pairs (0, 1, 2, 3, 11, or 0xFF disabled)
////////////////////////////////////////////////////////////////////////////
void ADF7242::cfgPreamble(bool syncWordDetect, bool lockAGC, bool preambleDetect, unsigned char preambleErrors) {
  if( (syncWordDetect == 1 || syncWordDetect == 0)
    && (lockAGC == 1 || lockAGC == 0)
    && (preambleDetect == 1 || preambleDetect ==0)
    && (preambleErrors == 0 || preambleErrors == 1 || preambleErrors == 2 || preambleErrors == 3
      || preambleErrors == 11 || preambleErrors == 0xFF) ) {
    switch(preambleErrors) {
      case 0:
        regWrite(fsk_preamble_config, (syncWordDetect << 6) | (lockAGC << 5) | (preambleDetect << 4) | 0x0C); // see notes on configuration of this register in datasheet
        break;
      case 1:
        regWrite(fsk_preamble_config, (syncWordDetect << 6) | (lockAGC << 5) | (preambleDetect << 4) | 0x0B); // see notes on configuration of this register in datasheet
        break;
      case 2:
        regWrite(fsk_preamble_config, (syncWordDetect << 6) | (lockAGC << 5) | (preambleDetect << 4) | 0x0A); // see notes on configuration of this register in datasheet
        break;
      case 3:
        regWrite(fsk_preamble_config, (syncWordDetect << 6) | (lockAGC << 5) | (preambleDetect << 4) | 0x09); // see notes on configuration of this register in datasheet
        break;
      case 11:
        regWrite(fsk_preamble_config, (syncWordDetect << 6) | (lockAGC << 5) | (preambleDetect << 4) | 0x01); // see notes on configuration of this register in datasheet
        break;
      case 0xFF:
        regWrite(fsk_preamble_config, (syncWordDetect << 6) | (lockAGC << 5) | (preambleDetect << 4) | 0x00); // see notes on configuration of this register in datasheet
        break;
    }
  }
  
  #ifdef DEBUG
    Serial.print("Register fsk_preamble_config: 0x");
    Serial.println(regRead(fsk_preamble_config), HEX);
  #endif
}

void ADF7242::cfgBasicPreamble() {
  regWrite(fsk_preamble_config, 0x2C); //Enable AGC lock after preamble lock and don't allow any errors
}

////////////////////////////////////////////////////////////////////////////
// void dumpRegMap()
////////////////////////////////////////////////////////////////////////////
// Dump the ADF7242 register map
////////////////////////////////////////////////////////////////////////////
void ADF7242::dumpRegMap() {
  int regMap [] = {0x100, 0x102, 0x105, 0x106, 0x107, 0x108, 0x109, 0x10A, 0x10B, 0x10C, 0x10D, 0x10E, 0x10F,
    0x111, 0x13E, 0x300, 0x301, 0x302, 0x304, 0x305, 0x306, 0x30C, 0x30D, 0x30E, 0x30F, 0x313, 0x314, 0x315, 
    0x316, 0x317, 0x318, 0x319, 0x31A, 0x31B, 0x31E, 0x32C, 0x32D, 0x335, 0x33D, 0x353, 0x354, 0x355, 0x36E, 
    0x36F, 0x371, 0x380, 0x381, 0x389, 0x38B, 0x395, 0x396, 0x39B, 0x3A7, 0x3A8, 0x3A9, 0x3AA, 0x3AE, 0x3B2, 
    0x3B4, 0x3B6, 0x3B7, 0x3B8, 0x3B9, 0x3BA, 0x3BC, 0x3BF, 0x3C4, 0x3C7, 0x3C8, 0x3C9, 0x3CA, 0x3CB, 0x3CC, 
    0x3D2, 0x3D3, 0x3D4, 0x3D5, 0x3D6, 0x3D7, 0x3E0, 0x3E3, 0x3E6, 0x3F0, 0x3F3, 0x3F4, 0x3F7, 0x3F8, 0x3F9, 
    0x3FA};
  for (int i = 0; i < 89; ++i)
  {
    Serial.print("Register 0x");
    Serial.print(regMap[i], HEX);
    Serial.print(" contains value 0x");
    Serial.println(regRead(regMap[i]), HEX);
  }
}

////////////////////////////////////////////////////////////////////////////
// void dumpISB()
////////////////////////////////////////////////////////////////////////////
// Dump the ADF7242 interrupt source bits for status of radio
////////////////////////////////////////////////////////////////////////////
void ADF7242::dumpISB() {
  Serial.print("irq1_src0[3] rc_ready: ");
  Serial.println(regRead(irq1_src0) & 0x08, HEX);
  Serial.print("irq1_src1[4] tx_pkt_sent: ");
  Serial.println(regRead(irq1_src1) & 0x10, HEX);
  Serial.print("irq1_src1[3] rx_pkt_rcvd: ");
  Serial.println(regRead(irq1_src1) & 0x08, HEX);
  Serial.print("irq1_src1[2] tx_sfd: ");
  Serial.println(regRead(irq1_src1) & 0x04, HEX);
  Serial.print("irq1_src1[1] rx_sfd: ");
  Serial.println(regRead(irq1_src1) & 0x02, HEX);
  Serial.print("irq1_src1[0] cca_complete: ");
  Serial.println(regRead(irq1_src1) & 0x01, HEX);
}

//////////////////////////////////////////////////////////////////////////////
// closeSPI()
//////////////////////////////////////////////////////////////////////////////
// Ends the current SPI transaction, allowing different settings to be loaded
//////////////////////////////////////////////////////////////////////////////
void ADF7242::closeSPI(){
  SPI.endTransaction();
}

//////////////////////////////////////////////////////////////////////////////
// dummySPIWrite()
//////////////////////////////////////////////////////////////////////////////
// A dummy write which does not trigger CS. Used when the SPI mode is changed
//////////////////////////////////////////////////////////////////////////////
void ADF7242::dummySPIWrite(){
  SPI.transfer(SPI_NOP);
}