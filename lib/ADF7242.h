////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Analog Devices, Inc.
//  May 2015
//  By: Daniel H. Tatum & Juan Chong
//  Written for the TeensyDuino Platform
////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ADF7242.h
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


#ifndef ADF7242_h
#define ADF7242_h

#include "Arduino.h"
#include <SPI.h>

//#define DEBUG // uncomment for DEBUG mode

// SPI Command List for ADF7242
#define SPI_NOP 0xFF // No operation. Use for dummy writes.
#define SPI_PKT_WR 0x10 // Write data to the packet RAM starting from the transmit packet base address pointer, Register txpb, Field tx_pkt_base (0x314[7:0]).
#define SPI_PKT_RD 0x30 // Read data from the packet RAM starting from the receive packet base address pointer, Register rxpb, Field rx_pkt_base (0x315[7:0]).
#define SPI_MEM_WR 0x18 // 0x18 + memory address[10:8] // Write data to MCR or packet RAM sequentially. 
#define SPI_MEM_RD 0x38 // 0x38 + memory address[10:8] // Read data from MCR or packet RAM sequentially. 
#define SPI_MEMR_WR 0x08 // 0x08 + memory address[10:8] // Write data to MCR or packet RAM as a random block. 
#define SPI_MEMR_RD 0x28 // 0x28 + memory address[10:8] // Read data from MCR or packet RAM as a random block. 
#define SPI_PRAM_WR 0x1E // Write data to the program RAM. 
#define RC_SLEEP 0xB1 // Invoke transition of the radio controller into the sleep state.
#define RC_IDLE 0xB2 // Invoke transition of the radio controller into the idle state.
#define RC_PHY_RDY 0xB3 // Invoke transition of the radio controller into the PHY_RDY state.
#define RC_RX 0xB4 // Invoke transition of the radio controller into the RX state.
#define RC_TX 0xB5 // Invoke transition of the radio controller into the TX state.
#define RC_MEAS 0xB6 // Invoke transition of the radio controller into the MEAS state.
#define RC_CCA 0xB7 // Invoke clear channel assessment.
#define RC_PC_RESET 0xC7 // Program counter reset. This should only be used after a firmware download to the program RAM.
#define RC_RESET 0xC8 // Resets the ADF7242 and puts it in the sleep state.

// Register Map from Table 50
#define ext_ctrl 0x100 // External LNA/PA and internal PA control configuration bits
#define fsk_preamble 0x102 // GFSK/FSK preamble length configuration
#define cca1 0x105 // RSSI threshold for CCA
#define cca2 0x106 // CCA mode configuration
#define buffercfg 0x107 // RX and TX Buffer configuration
#define pkt_cfg 0x108 // Firmware download module enable/FCS/CRC control
#define delaycfg0 0x109 // RC_RX command to SFD or SWD search delay
#define delaycfg1 0x10A // RC_TX command to TX state delay
#define delaycfg2 0x10B // MAC delay extension
#define sync_word0 0x10C // Sync Word Bits[7:0] of [23:0]
#define sync_word1 0x10D // Sync Word Bits[15:8] of [23:0]
#define sync_word2 0x10E // Sync Word Bits[23:16] of [23:0]
#define sync_config 0x10F // Sync word configuration
#define fsk_preamble_config 0x111 // GFSK/FSK preamble configuration
#define rc_cfg 0x13E // Packet/SPORT mode configuration
#define ch_freq0 0x300 // Channel frequency settings—low byte
#define ch_freq1 0x301 // Channel frequency settings—middle byte
#define ch_freq2 0x302 // Channel frequency settings—two MSBs
#define tx_fd 0x304 // Transmit frequency deviation register
#define dm_cfg0 0x305 // Receive discriminator bandwidth register
#define tx_m 0x306 // Gaussian and preemphasis filter configuration
#define rrb 0x30C // RSSI readback register
#define lrb 0x30D // Signal quality indicator quality readback register
#define dr0 0x30E // Data rate [bps/100], Bits[15:8] of [15:0]
#define dr1 0x30F // Data rate [bps/100], Bits[7:0] of [15:0]
#define prampg 0x313 // PRAM page
#define txpb 0x314 // Transmit packet storage base address
#define rxpb 0x315 // Receive packet storage base address
#define tmr_cfg0 0x316 // Wake-up timer configuration register—high byte
#define tmr_cfg1 0x317 // Wake-up timer configuration register—low byte
#define tmr_rld0 0x318 // Wake-up timer value register—high byte
#define tmr_rld1 0x319 // Wake-up timer value register—low byte
#define tmr_ctrl 0x31A // Wake-up timer timeout flag configuration register
#define wuc_32khzosc_status 0x31B // 32 kHz oscillator/WUC status
#define pd_aux 0x31E // Battery monitor and external PA bias enable
#define gp_cfg 0x32C // GPIO configuration
#define gp_out 0x32D // GPIO configuration
#define synt 0x335 // Synthesizer lock time
#define rc_cal_cfg 0x33D // RC calibration setting
#define vco_band_ovrw 0x353 // Overwrite value for the VCO frequency band.
#define vco_idac_ovrw 0x354 // Overwrite value for the VCO bias current DAC.
#define vco_ovwr_cfg 0x355 // VCO calibration settings overwrite enable
#define pa_bias 0x36E // PA bias control
#define vco_cal_cfg 0x36F // VCO calibration parameters
#define xto26_trim_cal 0x371 // 26 MHz crystal oscillator configuration
#define vco_band_rb 0x380 // Readback VCO band after calibration
#define vco_idac_rb 0x381 // Readback of the VCO bias current DAC after calibration
#define iirf_cfg 0x389 // BB filter decimation rate
#define dm_cfg1 0x38B // Postdemodulator filter bandwidth
#define rxcal0 0x395 // Receiver baseband filter calibration word, LSB
#define rxcal1 0x396 // Receiver baseband filter calibration word, MSB
#define rxfe_cfg 0x39B // Receive baseband filter bandwidth and LNA selection
#define pa_rr 0x3A7 // PA ramp rate
#define pa_cfg 0x3A8 // PA output stage current control
#define extpa_cfg 0x3A9 // External PA bias DAC configuration
#define extpa_msc 0x3AA // External PA interface circuit configuration
#define adc_rbk 0x3AE // ADC readback
#define agc_cfg1 0x3B2 // AGC configuration parameters
#define agc_max 0x3B4 // AGC configuration parameters
#define agc_cfg2 0x3B6 // AGC configuration parameters
#define agc_cfg3 0x3B7 // AGC configuration parameters
#define agc_cfg4 0x3B8 // AGC configuration parameters
#define agc_cfg5 0x3B9 // AGC configuration parameters
#define agc_cfg6 0x3BA // AGC configuration parameters
#define agc_cfg7 0x3BC // AGC configuration parameters
#define ocl_cfg0 0x3BF // OCL system parameters
#define ocl_cfg1 0x3C4 // OCL system parameters
#define irq1_en0 0x3C7 // Interrupt Mask Set Bits[7:0] of [15:0] for IRQ1
#define irq1_en1 0x3C8 // Interrupt Mask Set Bits[15:8] of [15:0] for IRQ1
#define irq2_en0 0x3C9 // Interrupt Mask Set Bits[7:0] of [15:0] for IRQ2
#define irq2_en1 0x3CA // Interrupt Mask Set Bits[15:8] of [15:0] for IRQ2
#define irq1_src0 0x3CB // Interrupt Source Bits[7:0] of [15:0] for IRQ
#define irq1_src1 0x3CC // Interrupt Source Bits[15:8] of [15:0] for IRQ
#define ocl_bw0 0x3D2 // OCL system parameters
#define ocl_bw1 0x3D3 // OCL system parameters
#define ocl_bw2 0x3D4 // OCL system parameters
#define ocl_bw3 0x3D5 // OCL system parameters
#define ocl_bw4 0x3D6 // OCL system parameters
#define ocl_bws 0x3D7 // OCL system parameters
#define ocl_bw13 0x3E0 // OCL system parameters
#define gp_drv 0x3E3 // GPIO and SPI I/O pads drive strength configuration
#define bm_cfg 0x3E6 // Battery monitor threshold voltage setting
#define tx_fsk_test 0x3F0 // TX GFSK/FSK SPORT test mode configuration
#define preamble_num_validate 0x3F3 // Preamble validation
#define sfd_15_4 0x3F4 // Option to set nonstandard SFD
#define afc_cfg 0x3F7 // AFC mode and polarity configuration
#define afc_ki_kp 0x3F8 // AFC ki and kp
#define afc_range 0x3F9 // AFC range
#define afc_read 0x3FA // AFC frequency error readback

class ADF7242 {
public:
	// Constructor with chip select (CS) pin
	ADF7242(int CS);

	// Destructor
	~ADF7242();

	// Reset state
	void reset();

	// Sleep state
	void sleep();

	// Idle state
	void idle();

	// PHY_RDY state
	void PHY_RDY();

	// Clear channel assessment
	void CCA();

	// Receive state
	void receive();

	// Transmit state
	void transmit();

	// Measure chip temperature state
	unsigned char meas();

	// Sets SPI bit order, clock divider, and data mode
	void configSPI();

	// Read register
	unsigned char regRead(unsigned int regAddr);

	// Write register
	void regWrite(unsigned int regAddr, unsigned char regData);

	// Initialize FSK at data rate
	void initFSK(unsigned char dataRate);

	// TRx frequency in MHz
	void chFreq(long freq);

	// Operating mode: IEEE 802.15.4-2006 packet, SPORT; GFSK/FSK packet, SPORT
	void setMode(unsigned char MCS);

	// Set sync word
	void syncWord(unsigned long word, unsigned char tol);

	// PA configuration
	void cfgPA(unsigned char pwr, bool hp, unsigned char rr);

	// AFC configuration
	void cfgAFC(unsigned char range);

	// Sets Tx/Rx packet buffer pointers
	void cfgPB(unsigned char Tx, unsigned char Rx);

	// Automatically append CRC to packets
	void cfgCRC(bool CRC);

	// FSK preamble configuration
	void cfgPreamble(bool syncWordDetect, bool lockAGC, bool preambleDetect, unsigned char preambleErrors);

	// Close SPI Transaction
	void closeSPI();

	// Dump the entire register map from the memory for debugging
	void dumpRegMap();

	// Dump the ADF7242 interrupt source bits for status of radio
	void dumpISB();

	//Dummy write with no CS call to force the MCU into a different SPI mode
	void dummySPIWrite();

private:
	// Chip select pin
	int _CS;

};

#endif