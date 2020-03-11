/*
CapTouchWireling.h - Last modified 24 Feb 2020

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

#ifndef CapTouchWireling_h
#define CapTouchWireling_h

#include "ATtiny841Lib.h"


class CapTouchWireling: public ATtiny841Lib {
  public:
    CapTouchWireling(uint8_t addr=0);
    uint8_t begin();
    uint16_t capTouchRead(uint8_t pin);
    int16_t getPosition(int range=100);
    int16_t getMaxReading();
    int16_t getMagnitude();
    uint32_t duration();
    bool update();
    bool isTouched();
    
    static const uint8_t numSensors = 6;
    const uint8_t T841_ADDRESS = 0x31;
    const uint8_t EXPECTED_CAPTOUCHWIRELING_FIRMWARE = 0x1A;
    const uint8_t capTouchPins[numSensors] = { 0, 1, 2, 3, 5, 7};
    int16_t capTouchCal[numSensors];
    int16_t capTouchCurrent[numSensors];
    uint8_t overCalCount[numSensors];
    int16_t currentMaxReading= 0;
    uint8_t primary = 0;
    bool isTouch = 0;
    uint8_t initialTouchPos = 0;
    uint8_t finalTouchPos = 0;
    uint8_t currentTouchedPositions = 0;
    uint8_t allTouchedPositions = 0;
    uint32_t touchTimer = 0;
};
#endif