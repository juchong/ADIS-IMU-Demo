////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Analog Devices, Inc.
//  October 2014
//  By: Daniel H. Tatum
////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ADIS16480.h
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

#ifndef ADIS16480_h
#define ADIS16480_h
#include "Arduino.h"
#include <SPI.h>

//#define DEBUG // uncomment for DEBUG mode

#define SPI_NOP 0x00 // No operation. Use for dummy writes.

// User Register Memory Map from Table 9
#define PAGE_ID 0x0000 // 0x00, R/W, No, Page identifier, N/A
// Name, {PAGE_ID, Address} // Default, R/W, Flash, Register Description, Format
#define SEQ_CNT 0x0006 // N/A, R, No, Sequence counter, Table 68
#define SYS_E_FLAG 0x0008 // 0x0000, R, No, Output - system error flags, Table 59
#define DIAG_STS 0x000A // 0x0000, R, No, Output - self-test error flags, Table 60
#define ALM_STS 0x000C // 0x0000, R, No, Output - alarm error flags, Table 61
#define TEMP_OUT 0x000E // N/A, R, No, Output - temperature, Table 57
#define X_GYRO_LOW 0x0010 // N/A, R, No, Output - x-axis gyroscope low word, Table 14
#define X_GYRO_OUT 0x0012 // N/A, R, No, Output - x-axis gyroscope high word, Table 10
#define Y_GYRO_LOW 0x0014 // N/A, R, No, Output - y-axis gyroscope low word, Table 15
#define Y_GYRO_OUT 0x0016 // N/A, R, No, Output - y-axis gyroscope high word, Table 11
#define Z_GYRO_LOW 0x0018 // N/A, R, No, Output - z-axis gyroscope low word, Table 16
#define Z_GYRO_OUT 0x001A // N/A, R, No, Output - z-axis gyroscope high word, Table 12
#define X_ACCL_LOW 0x001C // N/A, R, No, Output - x-axis accelerometer low word, Table 21
#define X_ACCL_OUT 0x001E // N/A, R, No, Output - x-axis accelerometer high word, Table 17
#define Y_ACCL_LOW 0x0020 // N/A, R, No, Output - y-axis accelerometer low word, Table 22
#define Y_ACCL_OUT 0x0022 // N/A, R, No, Output - y-axis accelerometer high word, Table 18
#define Z_ACCL_LOW 0x0024 // N/A, R, No, Output - z-axis accelerometer low word, Table 23
#define Z_ACCL_OUT 0x0026 // N/A, R, No, Output - z-axis accelerometer high word, Table 19
#define X_MAGN_OUT 0x0028 // N/A, R, No, Output - x-axis magnetometer high word, Table 38
#define Y_MAGN_OUT 0x002A // N/A, R, No, Output - y-axis magnetometer high word, Table 39
#define Z_MAGN_OUT 0x002C // N/A, R, No, Output - z-axis magnetometer high word, Table 40
#define BAROM_LOW 0x002E // N/A, R, No, Output - barometer low word, Table 56
#define BAROM_OUT 0x0030 // N/A, R, No, Output - barometer high word, Table 54
#define X_DELTANG_LOW 0x0040 // N/A, R, No, Output - x-axis delta angle low word, Table 28
#define X_DELTANG_OUT 0x0042 // N/A, R, No, Output - x-axis delta angle high word, Table 24
#define Y_DELTANG_LOW 0x0044 // N/A, R, No, Output - y-axis delta angle low word, Table 29
#define Y_DELTANG_OUT 0x0046 // N/A, R, No, Output - y-axis delta angle high word, Table 25
#define Z_DELTANG_LOW 0x0048 // N/A, R, No, Output - z-axis delta angle low word, Table 30
#define Z_DELTANG_OUT 0x004A // N/A, R, No, Output - z-axis delta angle high word, Table 26
#define X_DELTVEL_LOW 0x004C // N/A, R, No, Output - x-axis delta velocity low word, Table 35
#define X_DELTVEL_OUT 0x004E // N/A, R, No, Output - x-axis delta velocity high word, Table 31
#define Y_DELTVEL_LOW 0x0050 // N/A, R, No, Output - y-axis delta velocity low word, Table 36
#define Y_DELTVEL_OUT 0x0052 // N/A, R, No, Output - y-axis delta velocity high word, Table 32
#define Z_DELTVEL_LOW 0x0054 // N/A, R, No, Output - z-axis delta velocity low word, Table 37
#define Z_DELTVEL_OUT 0x0056 // N/A, R, No, Output - z-axis delta velocity high word, Table 33
#define Q0_C11_OUT 0x0060 // N/A, R/W, Yes, Quaternion q0 or rotation matrix C11, Table 42
#define Q1_C12_OUT 0x0062 // N/A, R/W, Yes, Quaternion q1 or rotation matrix C12, Table 43
#define Q2_C13_OUT 0x0064 // N/A, R/W, Yes, Quaternion q2 or rotation matrix C13, Table 44
#define Q3_C21_OUT 0x0066 // N/A, R/W, Yes, Quaternion q3 or rotation matrix C21, Table 45
#define C22_OUT 0x0068 // N/A, R/W, Yes, Rotation matrix C22, Table 46
#define ROLL_C23_OUT 0x006A // N/A, R/W, Yes, Euler angle / roll axis / or rotation matrix C23, Table 47
#define PITCH_C31_OUT 0x006C // N/A, R/W, Yes, Euler angle / pitch axis / or rotation matrix C31, Table 48
#define YAW_C32_OUT 0x006E // N/A, R/W, Yes, Euler angle / yaw axis / or rotation matrix, C32, Table 49
#define C33_OUT 0x0070 // N/A, R/W, Yes, Rotation matrix C33, Table 50
#define TIME_MS_OUT 0x0078 // N/A, R, Yes, Factory configuration time: minutes/seconds, Table 156
#define TIME_DH_OUT 0x007A // N/A, R, Yes, Factory configuration date/time: day/hour, Table 157
#define TIME_YM_OUT 0x007C // N/A, R, Yes, Factory configuration date: year/month, Table 158
#define PROD_ID 0x007E // 0x4060, R, Yes, Output, product identification (16,480), Table 65
#define X_GYRO_SCALE 0x0204 // 0x0000, R/W, Yes, Calibration scale - x-axis gyroscope, Table 103
#define Y_GYRO_SCALE 0x0206 // 0x0000, R/W, Yes, Calibration scale - y-axis gyroscope, Table 104
#define Z_GYRO_SCALE 0x0208 // 0x0000, R/W, Yes, Calibration scale - z-axis gyroscope, Table 105
#define X_ACCL_SCALE 0x020A // 0x0000, R/W, Yes, Calibration scale - x-axis accelerometer, Table 113
#define Y_ACCL_SCALE 0x020C // 0x0000, R/W, Yes, Calibration scale - y-axis accelerometer, Table 114
#define Z_ACCL_SCALE 0x020E // 0x0000, R/W, Yes, Calibration scale - z-axis accelerometer, Table 115
#define XG_BIAS_LOW 0x0210 // 0x0000, R/W, Yes, Calibration offset - gyroscope x-axis low word, Table 100
#define XG_BIAS_HIGH 0x0212 // 0x0000, R/W, Yes, Calibration offset - gyroscope x-axis high word, Table 97
#define YG_BIAS_LOW 0x0214 // 0x0000, R/W, Yes, Calibration offset - gyroscope y-axis low word, Table 101
#define YG_BIAS_HIGH 0x0216 // 0x0000, R/W, Yes, Calibration offset - gyroscope y-axis high word, Table 98
#define ZG_BIAS_LOW 0x0218 // 0x0000, R/W, Yes, Calibration offset - gyroscope z-axis low word, Table 102
#define ZG_BIAS_HIGH 0x021A // 0x0000, R/W, Yes, Calibration offset - gyroscope z-axis high word, Table 99
#define XA_BIAS_LOW 0x021C // 0x0000, R/W, Yes, Calibration offset - accelerometer x-axis low word, Table 110
#define XA_BIAS_HIGH 0x021E // 0x0000, R/W, Yes, Calibration offset - accelerometer x-axis high word, Table 107
#define YA_BIAS_LOW 0x0220 // 0x0000, R/W, Yes, Calibration offset - accelerometer y-axis low word, Table 111
#define YA_BIAS_HIGH 0x0222 // 0x0000, R/W, Yes, Calibration offset - accelerometer y-axis high word, Table 108
#define ZA_BIAS_LOW 0x0224 // 0x0000, R/W, Yes, Calibration offset - accelerometer z-axis low word, Table 112
#define ZA_BIAS_HIGH 0x0226 // 0x0000, R/W, Yes, Calibration offset - accelerometer z-axis high word, Table 109
#define HARD_IRON_X 0x0228 // 0x0000, R/W, Yes, Calibration / hard iron - magnetometer x-axis, Table 116
#define HARD_IRON_Y 0x022A // 0x0000, R/W, Yes, Calibration / hard iron - magnetometer y-axis, Table 117
#define HARD_IRON_Z 0x022C // 0x0000, R/W, Yes, Calibration / hard iron - magnetometer z-axis, Table 118
#define SOFT_IRON_S11 0x022E // 0x0000, R/W, Yes, Calibration / soft iron - magnetometer S11, Table 120
#define SOFT_IRON_S12 0x0230 // 0x0000, R/W, Yes, Calibration / soft iron - magnetometer S12, Table 121
#define SOFT_IRON_S13 0x0232 // 0x0000, R/W, Yes, Calibration / soft iron - magnetometer S13, Table 122
#define SOFT_IRON_S21 0x0234 // 0x0000, R/W, Yes, Calibration / soft iron - magnetometer S21, Table 123
#define SOFT_IRON_S22 0x0236 // 0x0000, R/W, Yes, Calibration / soft iron - magnetometer S22, Table 124
#define SOFT_IRON_S23 0x0238 // 0x0000, R/W, Yes, Calibration / soft iron - magnetometer S23, Table 125
#define SOFT_IRON_S31 0x023A // 0x0000, R/W, Yes, Calibration / soft iron - magnetometer S31, Table 126
#define SOFT_IRON_S32 0x023C // 0x0000, R/W, Yes, Calibration / soft iron - magnetometer S32, Table 127
#define SOFT_IRON_S33 0x023E // 0x0000, R/W, Yes, Calibration / soft iron - magnetometer S33, Table 128
#define BR_BIAS_LOW 0x0240 // 0x0000, R/W, Yes, Calibration offset -, barometer low word, Table 131
#define BR_BIAS_HIGH 0x0242 // 0x0000, R/W, Yes, Calibration offset -, barometer high word, Table 130
#define REFMTX_R11 0x0262 // 0x7FFF, R/W, Yes, Reference transformation matrix R11, Table 84
#define REFMTX_R12 0x0264 // 0x0000, R/W, Yes, Reference transformation matrix R12, Table 85
#define REFMTX_R13 0x0266 // 0x0000, R/W, Yes, Reference transformation matrix R13, Table 86
#define REFMTX_R21 0x0268 // 0x0000, R/W, Yes, Reference transformation matrix R21, Table 87
#define REFMTX_R22 0x026A // 0x7FFF, R/W, Yes, Reference transformation matrix R22, Table 88
#define REFMTX_R23 0x026C // 0x0000, R/W, Yes, Reference transformation matrix R23, Table 89
#define REFMTX_R31 0x026E // 0x0000, R/W, Yes, Reference transformation matrix R31, Table 90
#define REFMTX_R32 0x0270 // 0x0000, R/W, Yes, Reference transformation matrix R32, Table 91
#define REFMTX_R33 0x0272 // 0x7FFF, R/W, Yes, Reference transformation matrix R33, Table 92
#define USER_SCR_1 0x0274 // 0x0000, R/W, Yes, User Scratch Register 1, Table 152
#define USER_SCR_2 0x0276 // 0x0000, R/W, Yes, User Scratch Register 2, Table 153
#define USER_SCR_3 0x0278 // 0x0000, R/W, Yes, User Scratch Register 3, Table 154
#define USER_SCR_4 0x027A // 0x0000, R/W, Yes, User Scratch Register 4, Table 155
#define FLSHCNT_LOW 0x027C // N/A, R, Yes, Diagnostic - flash memory count low word, Table 147
#define FLSHCNT_HIGH 0x027E // N/A, R, Yes, Diagnostic - flash memory count high word, Table 148
#define GLOB_CMD 0x0302 // N/A, W, No, Control - global commands, Table 146
#define FNCTIO_CTRL 0x0306 // 0x000D, R/W, Yes, Control - I/O pins functional definitions, Table 149
#define GPIO_CTRL 0x0308 // 0x00X01, R/W, Yes, Control - I/O pins general purpose, Table 150
#define CONFIG 0x030A // 0x00C0, R/W, Yes, Control - clock and miscellaneous correction, Table 106
#define DEC_RATE 0x030C // 0x0000, R/W, Yes, Control - output sample rate decimation, Table 67
#define SLP_CNT 0x0310 // N/A, R/W, No, Control - power-down/sleep mode, Table 151
#define FILTR_BNK_0 0x0316 // 0x0000, R/W, Yes, Filter selection, Table 69
#define FILTR_BNK_1 0x0318 // 0x0000, R/W, Yes, Filter selection, Table 70
#define ALM_CNFG_0 0x0320 // 0x0000, R/W, Yes, Alarm configuration, Table 142
#define ALM_CNFG_1 0x0322 // 0x0000, R/W, Yes, Alarm configuration, Table 143
#define ALM_CNFG_2 0x0324 // 0x0000, R/W, Yes, Alarm configuration, Table 144
#define XG_ALM_MAGN 0x0328 // 0x0000, R/W, Yes, Alarm - x-axis gyroscope threshold setting, Table 132
#define YG_ALM_MAGN 0x032A // 0x0000, R/W, Yes, Alarm - y-axis gyroscope threshold setting, Table 133
#define ZG_ALM_MAGN 0x032C // 0x0000, R/W, Yes, Alarm - z-axis gyroscope threshold setting, Table 134
#define XA_ALM_MAGN 0x032E // 0x0000, R/W, Yes, Alarm - x-axis accelerometer threshold, Table 135
#define YA_ALM_MAGN 0x0330 // 0x0000, R/W, Yes, Alarm - y-axis accelerometer threshold, Table 136
#define ZA_ALM_MAGN 0x0332 // 0x0000, R/W, Yes, Alarm - z-axis accelerometer threshold, Table 137
#define XM_ALM_MAGN 0x0334 // 0x0000, R/W, Yes, Alarm - x-axis magnetometer threshold, Table 138
#define YM_ALM_MAGN 0x0336 // 0x0000, R/W, Yes, Alarm - y-axis magnetometer threshold, Table 139
#define ZM_ALM_MAGN 0x0338 // 0x0000, R/W, Yes, Alarm - z-axis magnetometer threshold, Table 140
#define BR_ALM_MAGN 0x033A // 0x0000, R/W, Yes, Alarm - barometer threshold setting, Table 141
#define EKF_CNFG 0x0350 // 0x0200, R/W, Yes, Extended Kalman filter configuration, Table 94
#define DECLN_ANGL 0x0354 // 0x0000, R/W, Yes, Declination angle, Table 93
#define ACC_DISTB_THR 0x0356 // 0x0020, R/W, Yes, Accelerometer disturbance threshold, Table 95
#define MAG_DISTB_THR 0x0358 // 0x0030, R/W, Yes, Magnetometer disturbance threshold, Table 96
#define QCVR_NOIS_LWR 0x0360 // 0xC5AC, R/W, Yes, Process covariance - gyroscope noise lower word, Table 77
#define QCVR_NOIS_UPR 0x0362 // 0x3727, R/W, Yes, Process covariance - gyroscope noise upper word, Table 76
#define QCVR_RRW_LWR 0x0364 // 0xE6FF, R/W, Yes, Process covariance - gyroscope RRW lower word, Table 79
#define QCVR_RRW_UPR 0x0366 // 0x2E5B, R/W, Yes, Process covariance - gyroscope RRW upper word, Table 78
#define RCVR_ACC_LWR 0x036C // 0x705F, R/W, Yes, Measurement covariance - accelerometer upper, Table 81
#define RCVR_ACC_UPR 0x036E // 0x3189, R/W, Yes, Measurement covariance - accelerometer lower, Table 80
#define RCVR_MAG_LWR 0x0370 // 0xCC77, R/W, Yes, Measurement covariance - magnetometer upper, Table 83
#define RCVR_MAG_UPR 0x0372 // 0x32AB, R/W, Yes, Measurement covariance - magnetometer lower, Table 82
#define FIRM_REV 0x0378 // N/A, R, Yes, Firmware revision, Table 62
#define FIRM_DM 0x037A // N/A, R, Yes, Firmware programming date: day/month, Table 63
#define FIRM_Y 0x037C // N/A, R, Yes, Firmware programming date: year, Table 64
#define SERIAL_NUM 0x0420 // N/A, R, Yes, Serial number, Table 66
#define FIR_COEF_A_LOW 0x0502 // to 0x7E N/A, R/W, Yes, FIR Filter Bank A Coefficients 0 through 59, Table 71
#define FIR_COEF_A_HIGH 0x0602 // to 0x7E N/A, R/W, Yes, FIR Filter Bank A Coefficients 60 through 119, Table 71
#define FIR_COEF_B_LOW 0x0702 // to 0x7E N/A, R/W, Yes, FIR Filter Bank B Coefficients 0 through 59, Table 72
#define FIR_COEF_B_HIGH 0x0802 // to 0x7E N/A, R/W, Yes, FIR Filter Bank B Coefficients 60 through 119, Table 72
#define FIR_COEF_C_LOW 0x0902 // to 0x7E N/A, R/W, Yes, FIR Filter Bank C Coefficients 0 through 59, Table 73
#define FIR_COEF_C_HIGH 0x0A02 // to 0x7E N/A, R/W, Yes, FIR Filter Bank C Coefficients 60 through 119, Table 73
#define FIR_COEF_D_LOW 0x0B02 // to 0x7E N/A, R/W, Yes, FIR Filter Bank D Coefficients 0 through 59, Table 74
#define FIR_COEF_D_HIGH 0x0C02 // to 0x7E N/A, R/W, Yes, FIR Filter Bank D Coefficients 60 through 119, Table 74

// ADIS16480 class definition
class ADIS16480{

public:
  // Constructor with configurable CS, data ready, and HW reset pins
  ADIS16480(int CS, int DR, int RST);
  // Destructor
  ~ADIS16480();

  // Performs hardware reset by sending pin 7 low for 2 seconds
  int reset(uint16_t ms);

  // Tares IMU
  int tare();

  // Sets SPI bit order, clock divider, and data mode
  int configSPI();

  // Write data to sensor
  int regWrite(uint16_t regAddr, int16_t regData);

  // Read single register from sensor
  uint16_t regRead(uint16_t regAddr);

  // Close SPI Transaction
  int closeSPI();

  //Dummy write with no CS call to force the MCU into a different SPI mode
  void dummySPIWrite();

private:
  // Chip select pin
  int _CS;

  // IRQ output pin for data ready
  int _DR;

  // Hardware reset pin
  int _RST;

  // SPI stall time
  int _stall = 10;

  // Current page
  int currentPage = 0x00;

};

#endif
