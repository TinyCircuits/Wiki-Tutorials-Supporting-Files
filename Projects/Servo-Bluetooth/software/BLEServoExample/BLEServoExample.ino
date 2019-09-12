//-------------------------------------------------------------------------------
//  TinyCircuits ST BLE and Servo TinyShield Example
//  Last Updated 24 Dec 2018
//
//  This example combines the UART Passthrough and Servo TinyShield example to
//  control a servo through the Nordic nRF UART app by sending microsecond pulse
//  values from 800 to 2200, such as '1500', through the terminal.
//
//  Written by Ben Rose, TinyCircuits http://tinycircuits.com
//-------------------------------------------------------------------------------


#include <SPI.h>
#include <STBLE.h>


#include <Wire.h>
#include <ServoDriver.h>

ServoDriver servo(NO_R_REMOVED);//this value affects the I2C address, which can be changed by
//removing resistors R1-R3. Then the corresponding R1_REMOVED,
//R2_REMOVED, R1_R2_REMOVED, R1_R4_REMOVED and so on can be set.
//Default is NO_R_REMOVED


//Debug output adds extra flash and memory requirements!
#ifndef BLE_DEBUG
#define BLE_DEBUG true
#endif

#if defined (ARDUINO_ARCH_AVR)
#define SerialMonitorInterface Serial
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#endif


uint8_t ble_rx_buffer[21];
uint8_t ble_rx_buffer_len = 0;
uint8_t ble_connection_state = false;
#define PIPE_UART_OVER_BTLE_UART_TX_TX 0

void setup() {
  SerialMonitorInterface.begin(9600);
  Wire.begin();
  while (!SerialMonitorInterface); //This will block until the Serial Monitor is opened on TinyScreen+/TinyZero platform!
  //pinMode(9, OUTPUT);//Pin 9 is the reset pin for the servo controller TinyShield, and the BLE TinyShield
  //digitalWrite(9, LOW);
  //delay(10);
  //digitalWrite(9, HIGH);
  //delay(100);
  BLEsetup();//This will toggle the shared reset line
  delay(15); // Not every ycomputer needs this delay between the BLEsetup() and servo.begin()

  if (servo.begin(20000)) {    //Set the period to 20000us or 20ms, correct for driving most servos
    SerialMonitorInterface.println("Motor driver not detected!");
    while (1);
  }
  //The failsafe turns off the PWM output if a command is not sent in a certain amount of time.
  //Failsafe is set in milliseconds- comment or set to 0 to disable
  servo.setFailsafe(1000);
}


void loop() {
  aci_loop();//Process any ACI commands or events from the NRF8001- main BLE handler, must run often. Keep main loop short.
  if (ble_rx_buffer_len) {//Check if data is available
    SerialMonitorInterface.print(ble_rx_buffer_len);
    SerialMonitorInterface.print(" : ");
    SerialMonitorInterface.println((char*)ble_rx_buffer);
    if (ble_rx_buffer_len == 3 || ble_rx_buffer_len == 4) {
      int servoValue = strtol((char*)ble_rx_buffer, NULL, 10);
      if (servoValue >= 800 & servoValue <= 2200) {
        servo.setServo(1, servoValue);
        servo.setServo(2, servoValue);
        servo.setServo(3, servoValue);
        servo.setServo(4, servoValue);
      }
    }
    ble_rx_buffer_len = 0;//clear afer reading
  }
  if (SerialMonitorInterface.available()) {//Check if serial input is available to send
    delay(10);//should catch input
    uint8_t sendBuffer[21];
    uint8_t sendLength = 0;
    while (SerialMonitorInterface.available() && sendLength < 19) {
      sendBuffer[sendLength] = SerialMonitorInterface.read();
      sendLength++;
    }
    if (SerialMonitorInterface.available()) {
      SerialMonitorInterface.print(F("Input truncated, dropped: "));
      if (SerialMonitorInterface.available()) {
        SerialMonitorInterface.write(SerialMonitorInterface.read());
      }
    }
    sendBuffer[sendLength] = '\0'; //Terminate string
    sendLength++;
    if (!lib_aci_send_data(PIPE_UART_OVER_BTLE_UART_TX_TX, (uint8_t*)sendBuffer, sendLength))
    {
      SerialMonitorInterface.println(F("TX dropped!"));
    }
  }
}
