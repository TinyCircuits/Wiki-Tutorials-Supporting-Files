/*
TinyArcade.h - Last modified 6 September 2016

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

#ifndef TINYARCADE_H
#define TINYARCADE_H

#define TinyArcadePin1 45
#define TinyArcadePin2 44
#define TinyArcadePinX 42
#define TinyArcadePinY 1
#define TinyArcadePinUp 42
#define TinyArcadePinDown 19
#define TinyArcadePinLeft 25
#define TinyArcadePinRight 15

const uint8_t TAJoystickUp  = 1 << 0;
const uint8_t TAJoystickDown = 1 << 1;
const uint8_t TAJoystickLeft  = 1 << 2;
const uint8_t TAJoystickRight = 1 << 3;

const uint8_t TAButton1  = 1 << 0;
const uint8_t TAButton2 = 1 << 1;

uint8_t arcadeVersion = 1;

void arcadeInit() {
  pinMode(4, INPUT_PULLUP);
  if (!digitalRead(4)) {
    arcadeVersion = 2;
  }
  analogWrite(A0, analogRead(A0));//Set audio output to avoid picking up noise, even though audio isn't used
  pinMode(TinyArcadePin2, INPUT_PULLUP);
  pinMode(TinyArcadePin1, INPUT_PULLUP);
  if (arcadeVersion == 2) {
    pinMode(TinyArcadePinUp, INPUT_PULLUP);
    pinMode(TinyArcadePinDown, INPUT_PULLUP);
    pinMode(TinyArcadePinLeft, INPUT_PULLUP);
    pinMode(TinyArcadePinRight, INPUT_PULLUP);
  }
}

uint8_t checkButton(uint8_t btn) {
  uint8_t rVal = 0;
  if ((btn & TAButton1) && !digitalRead(TinyArcadePin1))
    rVal |= TAButton1;
  if ((btn & TAButton2) && !digitalRead(TinyArcadePin2))
    rVal |= TAButton2;
  return rVal;
}

uint8_t checkJoystick(uint8_t joystickDir) {
  if (arcadeVersion == 1) {
    int analogVal = analogRead(TinyArcadePinY);
    uint8_t rVal = 0;
    if ((joystickDir & TAJoystickUp) && analogVal < 512 - 250)
      rVal |= TAJoystickUp;
    else if ((joystickDir & TAJoystickDown) && analogVal > 512 + 250)
      rVal |= TAJoystickDown;
    analogVal = analogRead(TinyArcadePinX);
    if ((joystickDir & TAJoystickLeft) && analogVal > 512 + 250)
      rVal |= TAJoystickLeft;
    else if ((joystickDir & TAJoystickRight) && analogVal < 512 - 250)
      rVal |= TAJoystickRight;
    return rVal;
  } else if (arcadeVersion == 2) {
    uint8_t rVal = 0;
    if ((joystickDir & TAJoystickUp) && !digitalRead(TinyArcadePinUp))
      rVal |= TAJoystickUp;
    if ((joystickDir & TAJoystickDown) && !digitalRead(TinyArcadePinDown))
      rVal |= TAJoystickDown;
    if ((joystickDir & TAJoystickLeft) && !digitalRead(TinyArcadePinLeft))
      rVal |= TAJoystickLeft;
    if ((joystickDir & TAJoystickRight) && !digitalRead(TinyArcadePinRight))
      rVal |= TAJoystickRight;
    return rVal;
  }
}
#endif