/*************************************************************************
 * BMA250 TinyZero Accelerometer Internet of Things Project: 
 * This program uses the TinyZero with an accelerometer populated on it to 
 * connect to WiFi for a Cayenne Internet of Things application
 * 
 * Hardware by: TinyCircuits
 * BMA250 Library by: TinyCircuits
 * Code by: Laverena Wienclaw for TinyCircuits
 ************************************************************************/

#include <Wire.h>         // For I2C communication with sensor
#include <WiFi101.h>      // For connecting to WiFi
#include "BMA250.h"       // For interfacing with the accel. sensor
#include <CayenneMQTTWiFi101.h> // Cayenne library to connect to transmit data 

/*********************** EDIT THIS SECTION TO MATCH YOUR INFO *************************/
// WiFi network information
char ssid[] = "TinyCircuits";        //  your network SSID (name)
char wifiPassword[] = "__________";  // your network password

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "________________________________________";
char password[] = "________________________________________";
char clientID[] = "________________________________________";
/*************************************************************************************/

// Accelerometer sensor variables for the sensor and its values
BMA250 accel_sensor;
int x, y, z;
double temp;

void setup() {
  SerialUSB.begin(9600);
  Wire.begin();
  WiFi.setPins(8, 2, A3, -1); // VERY IMPORTANT FOR TINYCIRCUITS WIFI SHIELD

  while (!SerialUSB); // ***Will halt program until the Serial Monitor is opened***

  // Connect to WiFi and Cayenne
  connectWifi();
  Cayenne.begin(username, password, clientID);

  // Set up the BMA250 acccelerometer sensor
  SerialUSB.print("Initializing BMA..."); SerialUSB.println();
  accel_sensor.begin(BMA250_range_2g, BMA250_update_time_64ms); 
}

void loop() {
  // Take sensor reading and print to Serial Monitor
  accel_sensor.read(); //This function gets new data from the acccelerometer

  // Get the acceleration values from the sensor and store them into global variables
  // (Makes reading the rest of the program easier)
  x = accel_sensor.X;
  y = accel_sensor.Y;
  z = accel_sensor.Z;
  temp = ((accel_sensor.rawTemp * 0.5) + 24.0);

  // Call function to display sensor readings on Serial Monitor
  showSerial();

  // Report sensor reading to Cayenne
  Cayenne.loop();
  delay(2000); // delay before looping again to take new reading
}

// Prints the sensor values to the Serial Monitor (found under 'Tools')
void showSerial() {
  SerialUSB.print("X = ");
  SerialUSB.print(x);
  
  SerialUSB.print("  Y = ");
  SerialUSB.print(y);
  
  SerialUSB.print("  Z = ");
  SerialUSB.print(z);
  
  SerialUSB.print("  Temperature(C) = ");
  SerialUSB.println(temp);
}

// This function goes through the steps to connect to WiFi using the WiFi101 library
void connectWifi(void){
  // Connect to Wifi network:
  SerialUSB.print("Connecting Wifi: ");
  SerialUSB.println(ssid);
  WiFi.begin(ssid, wifiPassword);

  // Loop to tell user that WiFi connection was not achieved
  while (WiFi.status() != WL_CONNECTED)
    SerialUSB.print("Connection failed: Move closer to router, or double check network info.");

  // Success message and put WiFi in low power mode to save energy
  WiFi.maxLowPowerMode();
  SerialUSB.println("");
  SerialUSB.println("WiFi connected");
}

// Default function for sending sensor data at intervals to Cayenne.
CAYENNE_OUT_DEFAULT()
{
  // Write data to Cayenne here:
  Cayenne.virtualWrite(1, x);
  Cayenne.virtualWrite(2, y);
  Cayenne.virtualWrite(3, z);
  Cayenne.virtualWrite(4, temp);
}
