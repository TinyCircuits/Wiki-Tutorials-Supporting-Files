/*
ATtiny841Lib.cpp - Last modified 24 Feb 2020

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Written by Ben Rose for TinyCircuits.

The latest version of this library can be found at https://tinycircuits.com/
*/

#include "MotorDriver.h"
#include <inttypes.h>
#include "Arduino.h"
#include "Wire.h"


void ATtiny841Lib::writeByte(uint8_t b1){
  Wire.beginTransmission(address);
  Wire.write(b1);
  Wire.endTransmission();
}
  
void ATtiny841Lib::writeByte(uint8_t b1,uint8_t b2){
  Wire.beginTransmission(address);
  Wire.write(b1);
  Wire.write(b2);
  Wire.endTransmission();
}

void ATtiny841Lib::writeByte(uint8_t b1,uint8_t b2,uint8_t b3){
  Wire.beginTransmission(address);
  Wire.write(b1);
  Wire.write(b2);
  Wire.write(b3);
  Wire.endTransmission();
}  

void ATtiny841Lib::writeCommand(uint8_t cmd, uint16_t val){
  int MSB=val>>8;
  Wire.beginTransmission(address);
  Wire.write(cmd);
  Wire.write(val);
  Wire.write(MSB);
  Wire.endTransmission();
  
}

void ATtiny841Lib::writeCommand(uint8_t cmd, uint16_t val1, uint16_t val2){
  int MSB=val1>>8;
  Wire.beginTransmission(address);
  Wire.write(cmd);
  Wire.write(val1);
  Wire.write(MSB);
  MSB=val2>>8;
  Wire.write(val2);
  Wire.write(MSB);
  Wire.endTransmission();
  
}

void ATtiny841Lib::writeCommand(uint8_t cmd, uint16_t val1, uint16_t val2, uint16_t val3, uint16_t val4){
  int MSB=val1>>8;
  Wire.beginTransmission(address);
  Wire.write(cmd);
  Wire.write(val1);
  Wire.write(MSB);
  MSB=val2>>8;
  Wire.write(val2);
  Wire.write(MSB);
  MSB=val3>>8;
  Wire.write(val3);
  Wire.write(MSB);
  MSB=val4>>8;
  Wire.write(val4);
  Wire.write(MSB);
  Wire.endTransmission();
}


uint8_t ATtiny841Lib::read(uint8_t reg){
  writeByte(reg);
  Wire.requestFrom(address,(uint8_t)1);
  return Wire.read();
}

void ATtiny841Lib::setFailsafe(uint16_t ms){
  if(ms>0x3FFF)ms=0x3FFF;
  writeCommand(COMMAND_SET_FAILSAFE_TIMEOUT, ms*4);//using defualt settings- really ~1.024ms
}

void ATtiny841Lib::setTimerPrescaler(uint8_t prescaler){
  if(prescaler>=T841_TIMER_PRESCALER_0 && prescaler<=T841_TIMER_PRESCALER_1024){
    writeByte(COMMAND_PRESCALER_1, prescaler);
    writeByte(COMMAND_PRESCALER_2, prescaler);
  }
}

void ATtiny841Lib::powerDown(){
  writeByte(COMMAND_SET_SLEEP_MODE, T841_SLEEP_MODE_PWR_DOWN);
  writeByte(COMMAND_SLEEP);
}
