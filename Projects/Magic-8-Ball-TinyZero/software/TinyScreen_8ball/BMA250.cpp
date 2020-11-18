#include "BMA250.h"
#include <inttypes.h>
#include "Arduino.h"
#include <Wire.h>

BMA250::BMA250()
{
}

int BMA250::begin(uint8_t range, uint8_t bw)
{
  //Detect address
  I2Caddress = BMA250_I2CADDR;
  Wire.beginTransmission(I2Caddress);
  if (Wire.endTransmission()) {
    I2Caddress++;
    Wire.beginTransmission(I2Caddress);
    if (Wire.endTransmission()) {
      I2Caddress = 0;
      return -1;
    }
  }
  //Setup the range measurement setting
  Wire.beginTransmission(I2Caddress);
  Wire.write(0x0F);
  Wire.write(range);
  Wire.endTransmission();
  //Setup the bandwidth
  Wire.beginTransmission(I2Caddress);
  Wire.write(0x10);
  Wire.write(bw);
  Wire.endTransmission();
  return 0;
}


void BMA250::read()
{
  //Set register index
  Wire.beginTransmission(I2Caddress);
  Wire.write(0x02);
  Wire.endTransmission();
  //Request seven data bytes
  Wire.requestFrom(I2Caddress, 7);
  //Receive acceleration measurements as 16 bit integers
  X = (int16_t)Wire.read();
  X |= (int16_t)Wire.read() << 8;
  Y = (int16_t)Wire.read();
  Y |= (int16_t)Wire.read() << 8;
  Z = (int16_t)Wire.read();
  Z |= (int16_t)Wire.read() << 8;
  //Only use the 10 significant bits
  X >>= 6; Y >>= 6; Z >>= 6;
  //Receive temperature measurement
  rawTemp = Wire.read();
  tempC = rawTemp/2 + 23;
}
