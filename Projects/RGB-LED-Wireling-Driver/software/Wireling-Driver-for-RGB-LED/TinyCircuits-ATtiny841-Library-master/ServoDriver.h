/*
ServoDriver.h - Last modified 24 Feb 2020

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

#ifndef ServoDriver_h
#define ServoDriver_h

#include "ATtiny841Lib.h"

#define ServoDriverDefaultResetPin 9
#define ROBOTZERO_SERVO_ADDR R1_R2_R3_R4_REMOVED

class ServoDriver: public ATtiny841Lib{
  public:
    ServoDriver(uint8_t);
    uint8_t begin(uint16_t);
    void useResetPin(uint8_t pin=ServoDriverDefaultResetPin);
    void setServo(uint8_t, uint16_t);
    void sleep();
    const uint8_t T841_ADDRESS = 0x62;
    const uint8_t EXPECTED_SERVO_FIRMWARE = 0x2A;
    int8_t servoResetPin=-1;
};

#endif