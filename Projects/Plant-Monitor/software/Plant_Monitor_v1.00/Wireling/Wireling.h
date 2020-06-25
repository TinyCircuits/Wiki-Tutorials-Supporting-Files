/*
 * Wireling.h - Last modified 11 February 2016
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

#ifndef Wireling_h
#define Wireling_h

#if defined(ARDUINO_ARCH_SAMD)
  #include "variant.h"
#endif

#if defined(_VARIANT_ROBOTZERO_)
  #define WirelingPowerPin      25
  #define WirelingPowerStateOn  LOW
  #define WirelingPowerStateOff HIGH
  #define WirelingPowerPin1      4
  #define WirelingPowerStateOn1  HIGH
  #define WirelingPowerStateOff1 LOW
#elif defined(_VARIANT_WIRELINGZERO_)
  #define WirelingPowerPin      9
  #define WirelingPowerStateOn  LOW
  #define WirelingPowerStateOff HIGH
#else
  #define WirelingPowerPin      4
  #define WirelingPowerStateOn  HIGH
  #define WirelingPowerStateOff LOW
#endif

#define MUX_BASE_ADDR 0x70
#define MUX_DISABLE 0x00
#define MUX_ENABLE 0x04
#define MUX_PORT_MASK 0x03

class wirelingLib
{
  public:
    void begin();
    void end();
    void selectPort(int);
    void enablePower();
    void disablePower();

  private:
    int32_t lastPort;
};

extern wirelingLib Wireling;

#endif //Wireling_h
