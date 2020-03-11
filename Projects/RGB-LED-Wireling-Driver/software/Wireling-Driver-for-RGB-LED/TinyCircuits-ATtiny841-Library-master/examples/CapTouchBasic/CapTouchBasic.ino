//-------------------------------------------------------------------------------
//  TinyCircuits Capacitive Touch Slider Basic Example
//  Last Updated 24 Feb 2020
//
//  This example code shows basic usage of the AST1003 Capacitive Touch Slider
//  Wireling. The board has an ATtiny841 microcontroller loaded with firmware that
//  implements capacitive touch sensing on six copper pads on the PCB, and
//  communicates the results over I2C. The CapTouchWireling library interprets
//  these results and provides a 0 to 100 position value when a finger is
//  detected on the touchpad. In this example, this value is used to print a
//  graphic output to the Serial Monitor.
//
//  Written by Ben Rose, TinyCircuits http://tinycircuits.com
//
//-------------------------------------------------------------------------------


#include <Wire.h>
#include <Wireling.h>
#include <ATtiny841Lib.h>
#include <CapTouchWireling.h>

CapTouchWireling capTouch;


#if defined (ARDUINO_ARCH_AVR)
#define SerialMonitorInterface Serial
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#endif

int capTouchPort = 0;

void setup() {
  SerialMonitorInterface.begin(9600);
  Wire.begin();
  Wireling.begin();
  Wireling.selectPort(capTouchPort);

  while (!SerialMonitorInterface && millis() < 5000); //This will block until the Serial Monitor is opened on TinyScreen+/TinyZero platform!
  if (capTouch.begin()) {
    SerialMonitorInterface.println("Capacitive touch Wireling not detected!");
    //while (1);
  }
}



void loop() {
  //function capTouch.update() must be called to update touch values, but the
  //return value(true or false) assigned to completedTouch does
  //not need to be used. capTouch.duration() is optional as well.

  bool completedTouch = capTouch.update();
  if (completedTouch) {
    long touchMillis = capTouch.duration();
    float touchSeconds = touchMillis / 1000.0;
    SerialMonitorInterface.print("Completed Touch: ");
    SerialMonitorInterface.print(touchSeconds);
    SerialMonitorInterface.println("s");
  }

  if (capTouch.isTouched()) {
    int pos = capTouch.getPosition();  //getPosition() returns a position value from 0 to 100 across the sensor
    int mag = capTouch.getMagnitude(); //getMagnitude() returns a relative 'pressure' value of about 50 to 500
    for (int i = 0; i < 100; i++) {
      if (abs(i - pos) <= (mag / 100)) {
        SerialMonitorInterface.print('|');
      } else {
        SerialMonitorInterface.print(' ');
      }
    }
    SerialMonitorInterface.print(pos);
    SerialMonitorInterface.print('\t');
    SerialMonitorInterface.print(mag);
    SerialMonitorInterface.print('\t');
    SerialMonitorInterface.println(capTouch.duration());
  }


  //Alternate usage: read the raw output from each sensor
  /*
    SerialMonitorInterface.print(capTouch.capTouchRead(0));SerialMonitorInterface.print('\t');
    SerialMonitorInterface.print(capTouch.capTouchRead(1));SerialMonitorInterface.print('\t');
    SerialMonitorInterface.print(capTouch.capTouchRead(2));SerialMonitorInterface.print('\t');
    SerialMonitorInterface.print(capTouch.capTouchRead(3));SerialMonitorInterface.print('\t');
    SerialMonitorInterface.print(capTouch.capTouchRead(4));SerialMonitorInterface.print('\t');
    SerialMonitorInterface.print(capTouch.capTouchRead(5));
    SerialMonitorInterface.println();
  */
}