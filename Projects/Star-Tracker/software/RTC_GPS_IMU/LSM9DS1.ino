////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTIMULib
//
//  Edited 22 February 2016 by Ben Rose for TinyCircuits, www.tinycircuits.com
//
//  Copyright (c) 2014-2015, richards-tech, LLC
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of
//  this software and associated documentation files (the "Software"), to deal in
//  the Software without restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
//  Software, and to permit persons to whom the Software is furnished to do so,
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#ifndef _RTIMULSM9DS1_H
#define  _RTIMULSM9DS1_H


//  LSM9DS1

//  I2C Slave Addresses

#define LSM9DS1_ADDRESS0  0x6a
#define LSM9DS1_ADDRESS1  0x6b
#define LSM9DS1_ID        0x68

#define LSM9DS1_MAG_ADDRESS0        0x1c
#define LSM9DS1_MAG_ADDRESS1        0x1e
#define LSM9DS1_MAG_ID              0x3d

//  LSM9DS1 Register map

#define LSM9DS1_ACT_THS             0x04
#define LSM9DS1_ACT_DUR             0x05
#define LSM9DS1_INT_GEN_CFG_XL      0x06
#define LSM9DS1_INT_GEN_THS_X_XL    0x07
#define LSM9DS1_INT_GEN_THS_Y_XL    0x08
#define LSM9DS1_INT_GEN_THS_Z_XL    0x09
#define LSM9DS1_INT_GEN_DUR_XL      0x0A
#define LSM9DS1_REFERENCE_G         0x0B
#define LSM9DS1_INT1_CTRL           0x0C
#define LSM9DS1_INT2_CTRL           0x0D
#define LSM9DS1_WHO_AM_I            0x0F
#define LSM9DS1_CTRL1               0x10
#define LSM9DS1_CTRL2               0x11
#define LSM9DS1_CTRL3               0x12
#define LSM9DS1_ORIENT_CFG_G        0x13
#define LSM9DS1_INT_GEN_SRC_G       0x14
#define LSM9DS1_OUT_TEMP_L          0x15
#define LSM9DS1_OUT_TEMP_H          0x16
#define LSM9DS1_STATUS              0x17
#define LSM9DS1_OUT_X_L_G           0x18
#define LSM9DS1_OUT_X_H_G           0x19
#define LSM9DS1_OUT_Y_L_G           0x1A
#define LSM9DS1_OUT_Y_H_G           0x1B
#define LSM9DS1_OUT_Z_L_G           0x1C
#define LSM9DS1_OUT_Z_H_G           0x1D
#define LSM9DS1_CTRL4               0x1E
#define LSM9DS1_CTRL5               0x1F
#define LSM9DS1_CTRL6               0x20
#define LSM9DS1_CTRL7               0x21
#define LSM9DS1_CTRL8               0x22
#define LSM9DS1_CTRL9               0x23
#define LSM9DS1_CTRL10              0x24
#define LSM9DS1_INT_GEN_SRC_XL      0x26
#define LSM9DS1_STATUS2             0x27
#define LSM9DS1_OUT_X_L_XL          0x28
#define LSM9DS1_OUT_X_H_XL          0x29
#define LSM9DS1_OUT_Y_L_XL          0x2A
#define LSM9DS1_OUT_Y_H_XL          0x2B
#define LSM9DS1_OUT_Z_L_XL          0x2C
#define LSM9DS1_OUT_Z_H_XL          0x2D
#define LSM9DS1_FIFO_CTRL           0x2E
#define LSM9DS1_FIFO_SRC            0x2F
#define LSM9DS1_INT_GEN_CFG_G       0x30
#define LSM9DS1_INT_GEN_THS_XH_G    0x31
#define LSM9DS1_INT_GEN_THS_XL_G    0x32
#define LSM9DS1_INT_GEN_THS_YH_G    0x33
#define LSM9DS1_INT_GEN_THS_YL_G    0x34
#define LSM9DS1_INT_GEN_THS_ZH_G    0x35
#define LSM9DS1_INT_GEN_THS_ZL_G    0x36
#define LSM9DS1_INT_GEN_DUR_G       0x37

//  Gyro sample rate defines

#define LSM9DS1_GYRO_SAMPLERATE_14_9    0
#define LSM9DS1_GYRO_SAMPLERATE_59_5    1
#define LSM9DS1_GYRO_SAMPLERATE_119     2
#define LSM9DS1_GYRO_SAMPLERATE_238     3
#define LSM9DS1_GYRO_SAMPLERATE_476     4
#define LSM9DS1_GYRO_SAMPLERATE_952     5

//  Gyro banwidth defines

#define LSM9DS1_GYRO_BANDWIDTH_0    0
#define LSM9DS1_GYRO_BANDWIDTH_1    1
#define LSM9DS1_GYRO_BANDWIDTH_2    2
#define LSM9DS1_GYRO_BANDWIDTH_3    3

//  Gyro FSR defines

#define LSM9DS1_GYRO_FSR_250        0
#define LSM9DS1_GYRO_FSR_500        1
#define LSM9DS1_GYRO_FSR_2000       3

//  Gyro high pass filter defines

#define LSM9DS1_GYRO_HPF_0          0
#define LSM9DS1_GYRO_HPF_1          1
#define LSM9DS1_GYRO_HPF_2          2
#define LSM9DS1_GYRO_HPF_3          3
#define LSM9DS1_GYRO_HPF_4          4
#define LSM9DS1_GYRO_HPF_5          5
#define LSM9DS1_GYRO_HPF_6          6
#define LSM9DS1_GYRO_HPF_7          7
#define LSM9DS1_GYRO_HPF_8          8
#define LSM9DS1_GYRO_HPF_9          9

//  Mag Register Map

#define LSM9DS1_MAG_OFFSET_X_L      0x05
#define LSM9DS1_MAG_OFFSET_X_H      0x06
#define LSM9DS1_MAG_OFFSET_Y_L      0x07
#define LSM9DS1_MAG_OFFSET_Y_H      0x08
#define LSM9DS1_MAG_OFFSET_Z_L      0x09
#define LSM9DS1_MAG_OFFSET_Z_H      0x0A
#define LSM9DS1_MAG_WHO_AM_I        0x0F
#define LSM9DS1_MAG_CTRL1           0x20
#define LSM9DS1_MAG_CTRL2           0x21
#define LSM9DS1_MAG_CTRL3           0x22
#define LSM9DS1_MAG_CTRL4           0x23
#define LSM9DS1_MAG_CTRL5           0x24
#define LSM9DS1_MAG_STATUS          0x27
#define LSM9DS1_MAG_OUT_X_L         0x28
#define LSM9DS1_MAG_OUT_X_H         0x29
#define LSM9DS1_MAG_OUT_Y_L         0x2A
#define LSM9DS1_MAG_OUT_Y_H         0x2B
#define LSM9DS1_MAG_OUT_Z_L         0x2C
#define LSM9DS1_MAG_OUT_Z_H         0x2D
#define LSM9DS1_MAG_INT_CFG         0x30
#define LSM9DS1_MAG_INT_SRC         0x31
#define LSM9DS1_MAG_INT_THS_L       0x32
#define LSM9DS1_MAG_INT_THS_H       0x33

//  Accel sample rate defines

#define LSM9DS1_ACCEL_SAMPLERATE_14_9    1
#define LSM9DS1_ACCEL_SAMPLERATE_59_5    2
#define LSM9DS1_ACCEL_SAMPLERATE_119     3
#define LSM9DS1_ACCEL_SAMPLERATE_238     4
#define LSM9DS1_ACCEL_SAMPLERATE_476     5
#define LSM9DS1_ACCEL_SAMPLERATE_952     6

//  Accel FSR

#define LSM9DS1_ACCEL_FSR_2     0
#define LSM9DS1_ACCEL_FSR_16    1
#define LSM9DS1_ACCEL_FSR_4     2
#define LSM9DS1_ACCEL_FSR_8     3

//  Accel filter bandwidth

#define LSM9DS1_ACCEL_LPF_408   0
#define LSM9DS1_ACCEL_LPF_211   1
#define LSM9DS1_ACCEL_LPF_105   2
#define LSM9DS1_ACCEL_LPF_50    3

//  Compass sample rate defines

#define LSM9DS1_COMPASS_SAMPLERATE_0_625    0
#define LSM9DS1_COMPASS_SAMPLERATE_1_25     1
#define LSM9DS1_COMPASS_SAMPLERATE_2_5      2
#define LSM9DS1_COMPASS_SAMPLERATE_5        3
#define LSM9DS1_COMPASS_SAMPLERATE_10       4
#define LSM9DS1_COMPASS_SAMPLERATE_20       5
#define LSM9DS1_COMPASS_SAMPLERATE_40       6
#define LSM9DS1_COMPASS_SAMPLERATE_80       7

//  Compass FSR

#define LSM9DS1_COMPASS_FSR_4   0
#define LSM9DS1_COMPASS_FSR_8   1
#define LSM9DS1_COMPASS_FSR_12  2
#define LSM9DS1_COMPASS_FSR_16  3

#define RTFLOAT float
#define  RTMATH_PI                   3.1415926535
#define RTMATH_DEGREE_TO_RAD        (RTMATH_PI / 180.0)
#define RTMATH_RAD_TO_DEGREE        (180.0 / RTMATH_PI)

int m_LSM9DS1GyroSampleRate;                            // the gyro sample rate
int m_LSM9DS1GyroBW;                                    // the gyro bandwidth code
int m_LSM9DS1GyroHpf;                                   // the gyro high pass filter cutoff code
int m_LSM9DS1GyroFsr;                                   // the gyro full scale range

int m_LSM9DS1AccelSampleRate;                           // the accel sample rate
int m_LSM9DS1AccelFsr;                                  // the accel full scale range
int m_LSM9DS1AccelLpf;                                  // the accel low pass filter

int m_LSM9DS1CompassSampleRate;                         // the compass sample rate
int m_LSM9DS1CompassFsr;                                // the compass full scale range
int m_sampleRate;
RTFLOAT m_gyroScale;
RTFLOAT m_accelScale;
RTFLOAT m_compassScale;

unsigned char m_accelGyroSlaveAddr;                     // I2C address of accel andgyro
unsigned char m_magSlaveAddr;                           // I2C address of mag


bool I2CWriteByte(uint8_t addr, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(val);
  if (Wire.endTransmission())
    return false;
  else
    return true;
}

bool I2CReadByte(uint8_t addr, uint8_t reg, uint8_t *val) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  if (Wire.endTransmission())
    return false;

  while (Wire.available())
    Wire.read();

  Wire.requestFrom((int)addr, (int)1);

  if (Wire.available() != 1)
    return false;

  *val = Wire.read();
  return true;
}

bool I2CReadBytes(uint8_t addr, uint8_t reg, uint8_t count, uint8_t *vals) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  if (Wire.endTransmission())
    return false;

  while (Wire.available())
    Wire.read();

  Wire.requestFrom((int)addr, (int)count);

  if (Wire.available() != count)
    return false;

  for (uint8_t i = 0; i < count; i++) {
    *vals = Wire.read();
    vals++;
  }
  return true;
}

bool setGyroSampleRate()
{
  unsigned char ctrl1;

  switch (m_LSM9DS1GyroSampleRate) {
    case LSM9DS1_GYRO_SAMPLERATE_14_9:
      ctrl1 = 0x20;
      m_sampleRate = 15;
      break;

    case LSM9DS1_GYRO_SAMPLERATE_59_5:
      ctrl1 = 0x40;
      m_sampleRate = 60;
      break;

    case LSM9DS1_GYRO_SAMPLERATE_119:
      ctrl1 = 0x60;
      m_sampleRate = 119;
      break;

    case LSM9DS1_GYRO_SAMPLERATE_238:
      ctrl1 = 0x80;
      m_sampleRate = 238;
      break;

    case LSM9DS1_GYRO_SAMPLERATE_476:
      ctrl1 = 0xa0;
      m_sampleRate = 476;
      break;

    case LSM9DS1_GYRO_SAMPLERATE_952:
      ctrl1 = 0xc0;
      m_sampleRate = 952;
      break;

    default:
      //HAL_ERROR1("Illegal LSM9DS1 gyro sample rate code %d\n", m_LSM9DS1GyroSampleRate);
      return false;
  }

  //m_sampleInterval = (uint64_t)1000000 / m_sampleRate;

  switch (m_LSM9DS1GyroBW) {
    case LSM9DS1_GYRO_BANDWIDTH_0:
      ctrl1 |= 0x00;
      break;

    case LSM9DS1_GYRO_BANDWIDTH_1:
      ctrl1 |= 0x01;
      break;

    case LSM9DS1_GYRO_BANDWIDTH_2:
      ctrl1 |= 0x02;
      break;

    case LSM9DS1_GYRO_BANDWIDTH_3:
      ctrl1 |= 0x03;
      break;
  }

  switch (m_LSM9DS1GyroFsr) {
    case LSM9DS1_GYRO_FSR_250:
      ctrl1 |= 0x00;
      m_gyroScale = (RTFLOAT)0.00875 * RTMATH_DEGREE_TO_RAD;
      break;

    case LSM9DS1_GYRO_FSR_500:
      ctrl1 |= 0x08;
      m_gyroScale = (RTFLOAT)0.0175 * RTMATH_DEGREE_TO_RAD;
      break;

    case LSM9DS1_GYRO_FSR_2000:
      ctrl1 |= 0x18;
      m_gyroScale = (RTFLOAT)0.07 * RTMATH_DEGREE_TO_RAD;
      break;

    default:
      //HAL_ERROR1("Illegal LSM9DS1 gyro FSR code %d\n", m_LSM9DS1GyroFsr);
      return false;
  }
  return I2CWriteByte(m_accelGyroSlaveAddr, LSM9DS1_CTRL1, ctrl1);
}

bool setGyroCTRL3()
{
  unsigned char ctrl3;

  if ((m_LSM9DS1GyroHpf < LSM9DS1_GYRO_HPF_0) || (m_LSM9DS1GyroHpf > LSM9DS1_GYRO_HPF_9)) {
    //HAL_ERROR1("Illegal LSM9DS1 gyro high pass filter code %d\n", m_LSM9DS1GyroHpf);
    return false;
  }
  ctrl3 = m_LSM9DS1GyroHpf;

  //  Turn on hpf
  ctrl3 |= 0x40;

  return I2CWriteByte(m_accelGyroSlaveAddr,  LSM9DS1_CTRL3, ctrl3);
}

bool setAccelCTRL6()
{
  unsigned char ctrl6;

  if ((m_LSM9DS1AccelSampleRate < 0) || (m_LSM9DS1AccelSampleRate > 6)) {
    //HAL_ERROR1("Illegal LSM9DS1 accel sample rate code %d\n", m_LSM9DS1AccelSampleRate);
    return false;
  }

  ctrl6 = (m_LSM9DS1AccelSampleRate << 5);

  if ((m_LSM9DS1AccelLpf < 0) || (m_LSM9DS1AccelLpf > 3)) {
    //HAL_ERROR1("Illegal LSM9DS1 accel low pass fiter code %d\n", m_LSM9DS1AccelLpf);
    return false;
  }

  switch (m_LSM9DS1AccelFsr) {
    case LSM9DS1_ACCEL_FSR_2:
      m_accelScale = (RTFLOAT)0.000061;
      break;

    case LSM9DS1_ACCEL_FSR_4:
      m_accelScale = (RTFLOAT)0.000122;
      break;

    case LSM9DS1_ACCEL_FSR_8:
      m_accelScale = (RTFLOAT)0.000244;
      break;

    case LSM9DS1_ACCEL_FSR_16:
      m_accelScale = (RTFLOAT)0.000732;
      break;

    default:
      //HAL_ERROR1("Illegal LSM9DS1 accel FSR code %d\n", m_LSM9DS1AccelFsr);
      return false;
  }

  ctrl6 |= (m_LSM9DS1AccelLpf) | (m_LSM9DS1AccelFsr << 3);

  return I2CWriteByte(m_accelGyroSlaveAddr,  LSM9DS1_CTRL6, ctrl6);
}

bool setAccelCTRL7()
{
  unsigned char ctrl7;

  ctrl7 = 0x00;
  //Bug: Bad things happen.
  //ctrl7 = 0x05;

  return I2CWriteByte(m_accelGyroSlaveAddr,  LSM9DS1_CTRL7, ctrl7);
}


bool setCompassCTRL1()
{
  unsigned char ctrl1;

  if ((m_LSM9DS1CompassSampleRate < 0) || (m_LSM9DS1CompassSampleRate > 5)) {
    //HAL_ERROR1("Illegal LSM9DS1 compass sample rate code %d\n", m_LSM9DS1CompassSampleRate);
    return false;
  }

  ctrl1 = (m_LSM9DS1CompassSampleRate << 2) | (0x80); //temp compensation?

  return I2CWriteByte(m_magSlaveAddr, LSM9DS1_MAG_CTRL1, ctrl1);
}

bool setCompassCTRL2()
{
  unsigned char ctrl2;

  //  convert FSR to uT

  switch (m_LSM9DS1CompassFsr) {
    case LSM9DS1_COMPASS_FSR_4:
      ctrl2 = 0;
      m_compassScale = (RTFLOAT)0.014;
      break;

    case LSM9DS1_COMPASS_FSR_8:
      ctrl2 = 0x20;
      m_compassScale = (RTFLOAT)0.029;
      break;

    case LSM9DS1_COMPASS_FSR_12:
      ctrl2 = 0x40;
      m_compassScale = (RTFLOAT)0.043;
      break;

    case LSM9DS1_COMPASS_FSR_16:
      ctrl2 = 0x60;
      m_compassScale = (RTFLOAT)0.058;
      break;

    default:
      //HAL_ERROR1("Illegal LSM9DS1 compass FSR code %d\n", m_LSM9DS1CompassFsr);
      return false;
  }

  return I2CWriteByte(m_magSlaveAddr, LSM9DS1_MAG_CTRL2, ctrl2);
}

bool setCompassCTRL3()
{
  return I2CWriteByte(m_magSlaveAddr,  LSM9DS1_MAG_CTRL3, 0x00);
}


bool setIMUPowerDown(bool pd) {
  if (pd) {
    I2CWriteByte(m_magSlaveAddr,  LSM9DS1_MAG_CTRL3, 0x03);
    I2CWriteByte(m_accelGyroSlaveAddr, LSM9DS1_CTRL1, 0x00);
  } else {
    setGyroSampleRate();
    setCompassCTRL3();
  }
}

int IMUInit()
{
  m_LSM9DS1GyroSampleRate = LSM9DS1_GYRO_SAMPLERATE_119;
  m_LSM9DS1GyroBW = LSM9DS1_GYRO_BANDWIDTH_1;
  m_LSM9DS1GyroHpf = LSM9DS1_GYRO_HPF_4;
  m_LSM9DS1GyroFsr = LSM9DS1_GYRO_FSR_500;

  m_LSM9DS1AccelSampleRate = LSM9DS1_ACCEL_SAMPLERATE_119;
  m_LSM9DS1AccelFsr = LSM9DS1_ACCEL_FSR_4;
  m_LSM9DS1AccelLpf = LSM9DS1_ACCEL_LPF_50;

  m_LSM9DS1CompassSampleRate = LSM9DS1_COMPASS_SAMPLERATE_20;
  m_LSM9DS1CompassFsr = LSM9DS1_COMPASS_FSR_4;


  unsigned char result;

  //  configure IMU

  m_accelGyroSlaveAddr = 0;

  // work out accel/gyro address

  I2CReadByte(LSM9DS1_ADDRESS0, LSM9DS1_WHO_AM_I, &result);
  if (result == LSM9DS1_ID)
    m_accelGyroSlaveAddr = LSM9DS1_ADDRESS0;
  result = 0;
  I2CReadByte(LSM9DS1_ADDRESS1, LSM9DS1_WHO_AM_I, &result);
  if (result == LSM9DS1_ID)
    m_accelGyroSlaveAddr = LSM9DS1_ADDRESS1;

  if (!m_accelGyroSlaveAddr)
    return -1;

  // work out mag address

  m_magSlaveAddr = 0;

  I2CReadByte(LSM9DS1_MAG_ADDRESS0, LSM9DS1_MAG_WHO_AM_I, &result);
  if (result == LSM9DS1_MAG_ID)
    m_magSlaveAddr = LSM9DS1_MAG_ADDRESS0;
  result = 0;
  I2CReadByte(LSM9DS1_MAG_ADDRESS1, LSM9DS1_MAG_WHO_AM_I, &result);
  if (result == LSM9DS1_MAG_ID)
    m_magSlaveAddr = LSM9DS1_MAG_ADDRESS1;

  if (!m_magSlaveAddr)
    return -2;

  //setCalibrationData();

  //  Set up the gyro/accel

  if (!I2CWriteByte(m_accelGyroSlaveAddr, LSM9DS1_CTRL8, 0x80 | 0x04))
    return -3;

  delay(100);

  if (!setGyroSampleRate())
    return -4;

  if (!setGyroCTRL3())
    return -5;

  //  Set up the mag

  if (!setAccelCTRL6())
    return -6;

  if (!setAccelCTRL7())
    return -7;

  if (!setCompassCTRL1())
    return -8;

  if (!setCompassCTRL2())
    return -9;

  if (!setCompassCTRL3())
    return -10;

  //gyroBiasInit();

  return 0;
}


extern int aX, aY, aZ, gX, gY, gZ, mX, mY, mZ, tempF;

bool IMURead()
{
  unsigned char status;
  unsigned char gyroData[6];
  unsigned char accelData[6];
  unsigned char compassData[6];
  unsigned char tempData[6];

  if (!I2CReadByte(m_accelGyroSlaveAddr, LSM9DS1_STATUS, &status))
    return false;

  if ((status & 0x3) == 0)
    return false;

  if (!I2CReadBytes(m_accelGyroSlaveAddr, LSM9DS1_OUT_X_L_G, 6, gyroData))
    return false;

  if (!I2CReadBytes(m_accelGyroSlaveAddr, LSM9DS1_OUT_X_L_XL, 6, accelData))
    return false;

  if (!I2CReadBytes(m_magSlaveAddr, LSM9DS1_MAG_OUT_X_L, 6, compassData))
    return false;

  if (!I2CReadBytes(m_accelGyroSlaveAddr, LSM9DS1_OUT_TEMP_L, 2, tempData))
    return false;

  aX =  (int16_t)(((uint16_t)accelData[1] << 8) | (uint16_t)accelData[0]);
  aY =  (int16_t)(((uint16_t)accelData[3] << 8) | (uint16_t)accelData[2]);
  aZ =  (int16_t)(((uint16_t)accelData[5] << 8) | (uint16_t)accelData[4]);
  //X = X>>6;
  //Y = Y>>6;
  //Z = Z>>6;

  gX =  (int16_t)(((uint16_t)gyroData[1] << 8) | (uint16_t)gyroData[0]);
  gY =  (int16_t)(((uint16_t)gyroData[3] << 8) | (uint16_t)gyroData[2]);
  gZ =  (int16_t)(((uint16_t)gyroData[5] << 8) | (uint16_t)gyroData[4]);

  mX =  (int16_t)(((uint16_t)compassData[1] << 8) | (uint16_t)compassData[0]);
  mY =  (int16_t)(((uint16_t)compassData[3] << 8) | (uint16_t)compassData[2]);
  mZ =  (int16_t)(((uint16_t)compassData[5] << 8) | (uint16_t)compassData[4]);
  //mX = mX>>6;
  // mY = mY>>6;
  //mZ = mZ>>6;


  int temp = (int16_t)(((uint16_t)tempData[1] << 8) | (uint16_t)tempData[0]);


  float t = 25.0 + (float)temp / 16.0;
  t = t * 1.8 + 32.0;

  int tempOffset = 5;
  
  tempF = t - tempOffset;


  return true;
}


#endif // _RTIMULSM9DS1_H
