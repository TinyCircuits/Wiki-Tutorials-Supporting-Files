//-------------------------------------------------------------------------------
//  TinyCircuits GPS Tracker Tutorial Program
//  Last updated 1 July 2020 (2.00)
//  
//  Using the GPS TinyShield, the Flash Memory TinyShield, and the TinyDuino/TinyZero,
//  this program turns the stack into a miniature GPS tracker and data logger.
//  The code detects which sentence is being read and formats the string accordingly.
//  In order to reduce the number of writes, we write one NMEA sentence per 10 seconds, 
//  which can be modified.
//
//  With the Telit SE868 V2 module with Glonass support, some messages come through
//  as GN** sentences instead of GP**. These are changed back to GP** before logging
//  so that they don't cause problems with programs like Google Earth.
//  Some GPS modules have been shipped with 4800 baud instead of 9600 (in 2017)- try 
//  this if you see bad data.
//
//  Written by Ben Rose & Lilith Freed for TinyCircuits, http://TinyCircuits.com
//  Updated July 1, 2020 to add SAMD21 support by Laveréna Wienclaw
//
//-------------------------------------------------------------------------------

//This may need to be set to 4800 baud
const int GPSBaud = 9600;

#include <SPIFlash.h>

#if defined (ARDUINO_ARCH_AVR)
#define SerialMonitorInterface Serial
#include <SoftwareSerial.h>
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#include "SoftwareSerialZero.h"
#endif

// The chip/slave select pin is pin 5 for the Flash Memory TinyShield
const uint8_t flashCS = 5; 
unsigned long address = 0;

// The SPIFlash object for the chip. Passed the chip select pin in the constructor.
SPIFlash flash(flashCS); 

// The Arduino pins used by the GPS module
const uint8_t GPS_ONOFFPin = A3;
const uint8_t GPS_SYSONPin = A2;
const uint8_t GPS_RXPin = A1;
const uint8_t GPS_TXPin = A0;
const uint8_t chipSelect = 10;

// The GPS connection is attached with a software serial port
SoftwareSerial Gps_Serial(GPS_RXPin, GPS_TXPin);

// Set which sentences should be enabled on the GPS module
// GPGGA - 
char nmea[] = {'1'/*GPGGA*/, '0'/*GNGLL*/, '0'/*GNGSA*/, '0'/*GPGSV/GLGSV*/, '1'/*GNRMC*/, '0'/*GNVTG*/, '0'/*not supported*/, '0'/*GNGNS*/};

void setup()
{
  Gps_Serial.begin(GPSBaud);
  SerialMonitorInterface.begin(115200);
  if(!SerialMonitorInterface) { delay(5000);}; // Pauses 5 seconds if Serial Monitor is not opened

  SerialMonitorInterface.println("Initializing Flash Memory...");
  SerialMonitorInterface.println();
  pinMode(flashCS, OUTPUT); // Ensure chip select pin is an output.
  flash.begin(); // Boots the flash memory

  SerialMonitorInterface.println("Determining write/read start point...");
  uint8_t flashBuffer[256];
  uint8_t foundStart = false;
  while (!foundStart) {
    flash.readByteArray(address, flashBuffer, 256);
    int i;
    for (i = 0; !foundStart && i < 256; i++) {
      if (flashBuffer[i] == 0xFF) // checks for the first logical 1
        foundStart = true;
    }
    address += i;
  }
  address--;
  SerialMonitorInterface.println("Done.");
  SerialMonitorInterface.println();

  unsigned long timer = millis();
  SerialMonitorInterface.println("Send 'y' to start read mode. Write mode will begin in 10 seconds...");
  SerialMonitorInterface.println();
  while(millis() < timer + 10000) {
    if(SerialMonitorInterface.available()) {
      if(SerialMonitorInterface.read() == 'y') {
        readFlash(address);
      }
    }
  }

  SerialMonitorInterface.println("Now initiating write mode.");
  SerialMonitorInterface.println();

  // Init the GPS Module to wake mode
  pinMode(GPS_SYSONPin, INPUT);
  digitalWrite(GPS_ONOFFPin, LOW);
  pinMode(GPS_ONOFFPin, OUTPUT);
  delay(100);
  SerialMonitorInterface.print("Attempting to wake GPS module.. ");
  while (digitalRead( GPS_SYSONPin ) == LOW )
  {
    // Need to wake the module
    digitalWrite( GPS_ONOFFPin, HIGH );
    delay(5);
    digitalWrite( GPS_ONOFFPin, LOW );
    delay(100);
  }
  SerialMonitorInterface.println("done.");
  delay(100);

  char command[] = "$PSRF103,00,00,00,01*xx\r\n";
  for (int i = 0; i < 8; i++) {
    command[10] = i + '0';
    command[16] = nmea[i];
    int c = 1;
    byte checksum = command[c++];
    while (command[c] != '*')
      checksum ^= command[c++];
    command[c + 1] = (checksum >> 4) + (((checksum >> 4) < 10) ? '0' : ('A' - 10));
    command[c + 2] = (checksum & 0xF) + (((checksum & 0xF) < 10) ? '0' : ('A' - 10));
    Gps_Serial.print(command);
    delay(20);
  }

  SerialMonitorInterface.println();

}

void loop() {
  unsigned long startTime = millis();
  while (Gps_Serial.read() != '$') {
    //do other stuff here
  }
  while (Gps_Serial.available() < 5);
  Gps_Serial.read(); 
  Gps_Serial.read(); //skip two characters
  char c = Gps_Serial.read();
  //determine senetence type
  if (c == 'R' || c == 'G') {
    c = Gps_Serial.read();
    if (c == 'M') {
      logNMEA(1);
    } else if (c == 'G') {
      logNMEA(2);
    }
  }

  // Waits 10 seconds before reading next NMEA string
  while (millis() - startTime < 10000) {
    Gps_Serial.read(); // clears GPS serial buffer
  }
}

void logNMEA(uint8_t type) {
  uint8_t buffer[82];
  // Initializes buffer to null terminators to ensure proper writing to flash memory
  for(int i = 0; i < 82; ++i) {
    buffer[i] = '\0';
  }

    // Writes NMEA string to buffer
  buffer[0] = '$';
  int counter = 1;
  char c = 0;
  while (!Gps_Serial.available());
  c = Gps_Serial.read();
  while (c != '*') {
    buffer[counter++] = c;
    while (!Gps_Serial.available());
    c = Gps_Serial.read();
  }
  buffer[counter++] = c;
  while (!Gps_Serial.available());
  c = Gps_Serial.read();
  buffer[counter++] = c;
  while (!Gps_Serial.available());
  c = Gps_Serial.read();
  buffer[counter++] = c;
  buffer[counter++] = '\r';
  buffer[counter++] = '\n';

  buffer[2] = 'P'; // Changes GNRMC to GPRMC

  c = 1;
  byte checksum = buffer[c++];
  while (buffer[c] != '*')
    checksum ^= buffer[c++];
  buffer[c + 1] = (checksum >> 4) + (((checksum >> 4) < 10) ? '0' : ('A' - 10));
  buffer[c + 2] = (checksum & 0xF) + (((checksum & 0xF) < 10) ? '0' : ('A' - 10));

  // Writes buffer array to flash memory. Write length is variable to maximize memory.
  flash.writeCharArray(address, (char *)buffer, strlen((char *)buffer));
  address += strlen((char *)buffer); // Sets address ahead for length of the nmea string
}

void readFlash(unsigned long address) {
  char command;
  do {
    // Clear Serial write buffer to prepare for read
    while(SerialMonitorInterface.available()) {
      SerialMonitorInterface.read();
    }
    // Menu
    SerialMonitorInterface.println("Read Mode | Please select a command (1, 2, 3):");
    SerialMonitorInterface.println("1. Read to serial monitor.");
    SerialMonitorInterface.println("2. Erase all data.");
    SerialMonitorInterface.println("3. Exit read mode.");
    while(!SerialMonitorInterface.available());
    command = SerialMonitorInterface.read();
    // Command select
    switch(command) {
      case '1':
        // Handles empty flash
        if(address == 0) {
          SerialMonitorInterface.println("No available data.");
          SerialMonitorInterface.println();
        } else {
          // Reads all available data to the Serial monitor
          for(unsigned long i = 0; i < address; ++i) {
            SerialMonitorInterface.print((char)flash.readChar(i));
          }
          SerialMonitorInterface.println();
        }
        break;
      case '2':
        // Erases data up until the first available byte.
        eraseData(address);
        address = 0; // resets the first available byte address to 0
        SerialMonitorInterface.println("All data erased.");
        SerialMonitorInterface.println();
        break;
      case '3':
        // Exits read mode.
        return;
      default:
        // Passes by invalid input.
        SerialMonitorInterface.println("That is not a recognized command.");
        SerialMonitorInterface.println();
        break;  
    }
  } while(command != 3);
}

// Erases the flash memory in 4KB sectors.
// Minimizes excess erase "writes".
void eraseData(unsigned long address) {
  unsigned long index = 0;
  while(index < address) {
    flash.eraseSector(index);
    index += 4096;
  }
}
