/*
 * Wireling.cpp - Last modified 11 February 2016
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Written by Ben Rose for TinyCircuits.
 * 
 * The latest version of this library can be found at https://TinyCircuits.com/
 * OR directly at the GitHub repo: 
 */



#include <Arduino.h>
#include <wiring_private.h>

#include "Wireling.h"
#include <Wire.h>

void wirelingLib::begin(void) {
  lastPort=-1;
  Wire.begin();
  pinMode(WirelingPowerPin, OUTPUT);
#if defined(WirelingPowerPin1)
  pinMode(WirelingPowerPin1, OUTPUT);
#endif
  enablePower();
  selectPort(0);
}

void wirelingLib::selectPort(int newPort) {

#if defined(_VARIANT_WIRELINGZERO_)
  if(newPort < 0 || newPort > 3)return;
  Wire.changeInterface(newPort);
#else
  if(newPort < 0 || newPort > 31)return;
  if(lastPort != -1){
    Wire.beginTransmission((uint8_t)(MUX_BASE_ADDR | (lastPort>>2)));
    Wire.write(MUX_DISABLE);
    Wire.endTransmission();
    delay(1);//needed?
  }
  Wire.beginTransmission(MUX_BASE_ADDR | (newPort>>2));
  Wire.write(MUX_ENABLE | (newPort & MUX_PORT_MASK));
  Wire.endTransmission();
  delay(1);//needed?
#endif
  lastPort = newPort;
}

void wirelingLib::enablePower() {
  digitalWrite(WirelingPowerPin, WirelingPowerStateOn);
#if defined(WirelingPowerPin1)
  digitalWrite(WirelingPowerPin1, WirelingPowerStateOn1);
#endif
}

void wirelingLib::disablePower() {
  digitalWrite(WirelingPowerPin, WirelingPowerStateOff);
#if defined(WirelingPowerPin1)
  digitalWrite(WirelingPowerPin1, WirelingPowerStateOff1);
#endif
}

void wirelingLib::end() {
  
}

wirelingLib Wireling;

