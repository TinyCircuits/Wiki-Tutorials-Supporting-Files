/*
CapTouchWireling.cpp - Last modified 24 Feb 2020

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

#include "CapTouchWireling.h"
#include <inttypes.h>
#include "Arduino.h"
#include "Wire.h"


CapTouchWireling::CapTouchWireling(uint8_t addr)
{
  address = T841_ADDRESS+addr;
}

uint8_t CapTouchWireling::begin(void)
{
  writeByte(COMMAND_SET_MODE,MODE_REGISTER_DEC);//write to the T841 registers directly
  if(read(FIRMWARE_REVISION_REG)!=EXPECTED_CAPTOUCHWIRELING_FIRMWARE)
    return 1;
  
  writeByte(T841_ADCSRA, _BV(T841_ADEN) | 4 | 1);
  writeByte(T841_DIDR0, 0xAF);
  writeByte(T841_PRR,/* _BV(T841_PRADC) | */_BV(T841_PRSPI) | _BV(T841_PRUSART0) | _BV(T841_PRUSART1));
  
  writeByte(COMMAND_SET_MODE, MODE_COMMAND);//send interpreted commands- see header file
  writeByte(COMMAND_CLOCK_PRESCALER, T841_CLOCK_PRESCALER_1);
  
  uint8_t pin;
  for (pin = 0; pin < numSensors; pin++) {
    capTouchCal[pin] = capTouchRead(pin);
    overCalCount[pin] = 0;
  }
  
  return 0;
}

uint16_t CapTouchWireling::capTouchRead(uint8_t pin) {

  writeByte(COMMAND_CAP_TOUCH, capTouchPins[pin], 5);
  delay(1);
  writeByte(COMMAND_SET_MODE, MODE_REGISTER_DEC);
  int value = read(RETURN_VAL_REG_0);
  value += (int)read(RETURN_VAL_REG_1)<<8;
  writeByte(COMMAND_SET_MODE, MODE_COMMAND);
  return value;
}

int16_t CapTouchWireling::getPosition(int range) {
  float position = primary;
  if (primary < numSensors-1)
    position += (pow((float)capTouchCurrent[primary + 1] / (float)getMaxReading(),0.45)) / 2.0;
  if (primary > 0)
    position -= (pow((float)capTouchCurrent[primary - 1] / (float)getMaxReading(),0.45)) / 2.0;
  
  position = position * (float)range / float(numSensors-1);
  return position;
}

int16_t CapTouchWireling::getMaxReading() {
  return currentMaxReading;
}

int16_t CapTouchWireling::getMagnitude() {
  uint8_t j;
  long magnitude = 0;
  for (j = 0; j < 6; j++) {
    magnitude += pow(capTouchCurrent[j], 2);
  }
  return sqrt(magnitude);
}

bool CapTouchWireling::update() {
  uint8_t j;
  currentMaxReading=0;
  for (j = 0; j < 6; j++) {
    int val = capTouchRead(j);
    if (val < capTouchCal[j]) {
      capTouchCal[j] = val;
    } else if (val > capTouchCal[j] + 1) {
      overCalCount[j]++;
      if (overCalCount[j] > 3) {
        overCalCount[j] = 0;
        capTouchCal[j] += 1;
      } else {
        //overCalCount[j]=0;
      }
    }
    capTouchCurrent[j] = val  - capTouchCal[j];  
    if (capTouchCurrent[j] > getMaxReading()) {
      currentMaxReading = capTouchCurrent[j];
      primary = j;
    }
  }

  bool completedTouch = false;

  if (currentMaxReading > 20) {
    if (!isTouch) {
      isTouch = true;
      touchTimer = millis();
      initialTouchPos = primary;
      finalTouchPos = primary;
      currentTouchedPositions = 0;
      allTouchedPositions = 0;
    } else {
      finalTouchPos = primary;
      currentTouchedPositions = 0;
      for (j = 0; j < 6; j++) {
        if (capTouchCurrent[j] > 12)
          currentTouchedPositions |= (1 << j);
        allTouchedPositions |= (1 << j);
      }
    }
  } else {
    if (isTouch) {
      isTouch = false;
      touchTimer = millis() - touchTimer;
      completedTouch = true;
    }
  }

  return completedTouch;
}

bool CapTouchWireling::isTouched() {
  return isTouch;
}

uint32_t CapTouchWireling::duration() {
  if (isTouch) {
    return millis() - touchTimer;
  }
  return touchTimer;
}