/*
  An RTC demo for the TinyCircuits SAMD21 boards: TinyZero, TinyScreen+, RobotZero, and WirelingZero.
*/

#include <RTCZero.h>

#if defined (ARDUINO_ARCH_AVR)
#define SerialMonitorInterface Serial
#include <SoftwareSerial.h>
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#include "SoftwareSerialZero.h"
#endif

RTCZero rtc; // create an rtc object

/* Change these values to set the current initial time */
const byte hours = 14;
const byte minutes = 45;
const byte seconds = 0;

/* Change these values to set the current initial date */
const byte day = 3;
const byte month = 8;
const byte year = 20;

void setup() {
  rtc.begin(); // initialize RTC

  // Set the time
  rtc.setHours(hours);
  rtc.setMinutes(minutes);
  rtc.setSeconds(seconds);

  // Set the date
  rtc.setDay(day);
  rtc.setMonth(month);
  rtc.setYear(year);
  
  // you can use also
  //rtc.setTime(hours, minutes, seconds);
  //rtc.setDate(day, month, year);
}

void loop() {
  print_RTC(); // print RTC data
  delay(1000); // wait 1 second
}

void print2digits(int number) {
  if (number < 10) {
    SerialMonitorInterface.print("0"); // print a 0 before if the number is < than 10
  }
  SerialMonitorInterface.print(number);
}

void print_time() {
  SerialMonitorInterface.print("Time: ");
  
  // Print time...
  print2digits(rtc.getHours());
  SerialMonitorInterface.print(":");
  print2digits(rtc.getMinutes());
  SerialMonitorInterface.print(":");
  print2digits(rtc.getSeconds());
}

void print_date() {
  SerialMonitorInterface.print("\tDate: ");
  
  // Print date...
  print2digits(rtc.getDay());
  SerialMonitorInterface.print("/");
  print2digits(rtc.getMonth());
  SerialMonitorInterface.print("/");
  print2digits(rtc.getYear());
  SerialMonitorInterface.print(" ");

  SerialMonitorInterface.println();
}

void print_RTC() {
  print_time();
  print_date();
  SerialMonitorInterface.println("- - - - - - - - - - - - - - - -");
}
