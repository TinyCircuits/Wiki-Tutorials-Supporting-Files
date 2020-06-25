//-------------------------------------------------------------------------------
//  TinyCircuits Wireling Basic Example
//  Last Updated 4 December 2019
//
//  This example shows how to scan for I2C devices on each port as well as print
//  out the state of the analog pin on each port.
//
//  Written by Ben Rose for TinyCircuits, https://TinyCircuits.com
//-------------------------------------------------------------------------------

#include <Wire.h>
#include <Wireling.h>

#if defined (ARDUINO_ARCH_AVR)
#define SerialMonitorInterface Serial
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#endif

void setup() {
  Wire.begin();
  Wireling.begin();
  SerialMonitorInterface.begin(9600);
}

void loop() {
  for (byte port = 0; port < 4; port++) {
    Wireling.selectPort(port);
    SerialMonitorInterface.print("Port ");
    SerialMonitorInterface.print(port);
    SerialMonitorInterface.print(": ");
    int nDevices = 0;
    for (byte I2Caddress = 1; I2Caddress < 127; I2Caddress++ )
    {
      if (I2Caddress != 0x68 && I2Caddress != 0x70) { // Ignore the multiplexer and RTC address
#if defined(_VARIANT_ROBOTZERO_)
        // On RobotZero, ignore the built in IMU, motor current setting digital potentiometer, and servo driver MCU
        if (I2Caddress != 0x1C && I2Caddress != 0x2F && I2Caddress != 0x6A) {
#endif
          Wire.beginTransmission(I2Caddress);
          if (!Wire.endTransmission()) { // This means that a device has acknowledged the transmission
            SerialMonitorInterface.print(" 0x");
            if (I2Caddress < 16) {
              SerialMonitorInterface.print("0");
            }
            SerialMonitorInterface.print(I2Caddress, HEX);
            nDevices++;
          }
#if defined(_VARIANT_ROBOTZERO_)
        }
#endif
      }
    }
    if (nDevices == 0) {
      SerialMonitorInterface.print("None\t");
    } else {
      SerialMonitorInterface.print("\t");
    }
    int portAnalogPin = A0 + port;
    if (digitalRead(portAnalogPin)) {
      SerialMonitorInterface.print("High");
    } else {
      SerialMonitorInterface.print("Low");
    } 
    SerialMonitorInterface.print("\t");
  }
  SerialMonitorInterface.println();

}
