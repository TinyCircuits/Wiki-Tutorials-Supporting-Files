/*
MotorDriver.cpp - Last modified 24 Feb 2020

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

#include "ServoDriver.h"
#include <inttypes.h>
#include "Arduino.h"
#include "Wire.h"

ServoDriver::ServoDriver(uint8_t addr)
{
  address = T841_ADDRESS+addr;
}

uint8_t ServoDriver::begin(uint16_t PWMperiod)
{
  if(servoResetPin!=-1){
    pinMode(servoResetPin, OUTPUT);
    digitalWrite(servoResetPin, LOW);
    delay(1);
    digitalWrite(servoResetPin, HIGH);
    delay(50);
  }
  writeByte(COMMAND_SET_MODE,MODE_REGISTER_DEC);//write to the T841 registers directly
  if(read(FIRMWARE_REVISION_REG)!=EXPECTED_SERVO_FIRMWARE)
    return 1;
  writeByte(T841_DDRA, _BV(7)|_BV(2)|_BV(1));
  writeByte(T841_DDRB, _BV(2));
  writeByte(T841_TOCPMSA1, _BV(T841_TOCC7S1)|_BV(T841_TOCC6S1));
  writeByte(T841_TOCPMSA0, _BV(T841_TOCC1S0)|_BV(T841_TOCC0S0));
  writeByte(T841_TCCR1A, _BV(T841_COM0A1)|_BV(T841_COM0B1)|_BV(T841_WGM11));
  writeByte(T841_TCCR2A, _BV(T841_COM2A1)|_BV(T841_COM0B1)|_BV(T841_WGM21));
  writeByte(T841_TCCR1B, _BV(T841_WGM13)|_BV(T841_WGM12)|_BV(T841_CS10));
  writeByte(T841_TCCR2B, _BV(T841_WGM23)|_BV(T841_WGM22)|_BV(T841_CS20));
  //Added for low power mode
  writeByte(T841_ADCSRA, 0);
  writeByte(T841_PRR, _BV(T841_PRADC) | _BV(T841_PRSPI) | _BV(T841_PRUSART0) | _BV(T841_PRUSART1));
  
  writeByte(COMMAND_SET_MODE, MODE_COMMAND);//send interpreted commands- see header file
  writeByte(COMMAND_CLOCK_PRESCALER, T841_CLOCK_PRESCALER_1);
  writeByte(COMMAND_PRESCALER_1, T841_TIMER_PRESCALER_8);//This default changed from T841_TIMER_PRESCALER_1 July 11 2018 to suit the servo driver
  writeByte(COMMAND_PRESCALER_2, T841_TIMER_PRESCALER_8);//This default changed from T841_TIMER_PRESCALER_1 July 11 2018 to suit the servo driver
  writeCommand(COMMAND_TIMER_1, PWMperiod);
  writeCommand(COMMAND_TIMER_2, PWMperiod);
  writeByte(COMMAND_SET_FAILSAFE_PRESCALER, T841_TIMER_PRESCALER_8);
  return 0;
}

void ServoDriver::useResetPin(uint8_t pin){
  servoResetPin=pin;
}

void ServoDriver::setServo(uint8_t servo, uint16_t val){
  if(servo>=1 && servo<=4){
    writeCommand(servo, val);
  }
}

void ServoDriver::sleep(){
  powerDown();
}
