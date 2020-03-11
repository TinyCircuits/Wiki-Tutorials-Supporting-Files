/*
ATtiny841Lib.h - Last modified 24 Feb 2020

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

#ifndef ATtiny841Lib_h
#define ATtiny841Lib_h

#include <inttypes.h>
#include "T841Defs.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

#define COMMAND_SET_MODE 0x00  //write mode- command, register access
#define COMMAND_SERVO_1 0x01 //write 16 bit pwm value 1
#define COMMAND_SERVO_2 0x02 //write 16 bit pwm value 2
#define COMMAND_SERVO_3 0x03 //write 16 bit pwm value 3
#define COMMAND_SERVO_4 0x04 //write 16 bit pwm value 4
#define COMMAND_MOTOR_1 0x05 //write first two 16 bit pwm values
#define COMMAND_MOTOR_2 0x06 //write second two 16 bit pwm values
#define COMMAND_ALL_PWM 0x07 //write four 16 bit pwm values
#define COMMAND_TIMER_1 0x08 //write 16 bit timer 1 top value
#define COMMAND_TIMER_2 0x09 //write 16 bit timer 2 top value
#define COMMAND_PRESCALER_1 0x0A //write timer 1 prescaler
#define COMMAND_PRESCALER_2 0x0B //write timer 2 prescaler
#define COMMAND_CLOCK_PRESCALER 0x0C //write system clock prescaler
#define COMMAND_SET_SLEEP_MODE 0x0D //set sleep mode
#define COMMAND_SLEEP 0x0E //go to sleep after I2C communication is done
#define COMMAND_SET_FAILSAFE_VALUES 0x0F //set failsafe PWM values - default is 0
#define COMMAND_SET_FAILSAFE_PRESCALER 0x10 //set failsafe timeout
#define COMMAND_SET_FAILSAFE_TIMEOUT 0x11 //set failsafe timeout
#define COMMAND_ALL_PWM_8 0x12 //write four 8 bit pwm values
#define COMMAND_CAP_TOUCH 0x13 //read cap touch value on pin

#define T841_CLOCK_PRESCALER_1 0x00
#define T841_CLOCK_PRESCALER_2 0x01
#define T841_CLOCK_PRESCALER_4 0x02
#define T841_CLOCK_PRESCALER_8 0x03
#define T841_CLOCK_PRESCALER_16 0x04
#define T841_CLOCK_PRESCALER_32 0x05
#define T841_CLOCK_PRESCALER_64 0x06
#define T841_CLOCK_PRESCALER_128 0x07
#define T841_CLOCK_PRESCALER_256 0x08

#define T841_TIMER_PRESCALER_0 0x00
#define T841_TIMER_PRESCALER_1 0x01
#define T841_TIMER_PRESCALER_8 0x02
#define T841_TIMER_PRESCALER_64 0x03
#define T841_TIMER_PRESCALER_256 0x04
#define T841_TIMER_PRESCALER_1024 0x05

#define T841_SLEEP_MODE_IDLE 0
#define T841_SLEEP_MODE_ADC _BV(T841_SM0)
#define T841_SLEEP_MODE_PWR_DOWN _BV(T841_SM1)

#define MODE_REGISTER_INC 0xAA
#define MODE_REGISTER_DEC 0xAB
#define MODE_COMMAND 0xAC

#define RETURN_VAL_REG_0 0x1A
#define RETURN_VAL_REG_1 0x1B
#define RETURN_VAL_REG_2 0x1C
#define RETURN_VAL_REG_3 0x1D

#define FIRMWARE_REVISION_REG 0x19

#define NO_R_REMOVED        0
#define R1_REMOVED          1
#define R2_REMOVED          2
#define R1_R2_REMOVED       3
#define R3_REMOVED          4
#define R1_R3_REMOVED       5
#define R2_R3_REMOVED       6
#define R1_R2_R3_REMOVED    7
#define R4_REMOVED          8
#define R1_R4_REMOVED       9
#define R2_R4_REMOVED       10
#define R1_R2_R4_REMOVED    11
#define R3_R4_REMOVED       12
#define R1_R3_R4_REMOVED    13
#define R2_R3_R4_REMOVED    14
#define R1_R2_R3_R4_REMOVED 15

class ATtiny841Lib {
public:
//ATtiny841Lib(uint8_t);
void writeByte(uint8_t);
void writeByte(uint8_t, uint8_t);
void writeByte(uint8_t, uint8_t, uint8_t);
void writeCommand(uint8_t, uint16_t);
void writeCommand(uint8_t, uint16_t, uint16_t);
void writeCommand(uint8_t, uint16_t, uint16_t, uint16_t, uint16_t);
uint8_t read(uint8_t);
void setFailsafe(uint16_t ms);
void setTimerPrescaler(uint8_t);
void powerDown();
uint8_t address;
};
#endif