//-------------------------------------------------------------------------------
//  TinyCircuits Servo Controller Basic Example
//  Last Updated 24 Feb 2020
//  
//  This example code shows basic usage of the ASD2303 servo Controller TinyShield.
//  The library intance is created with the hardware address of the servo controller
//  which can be changed by removed resistors R1-R3, allowing for up to 16 TinyShields.
//
//  The board uses the ATtiny841 microcontroller and has direct register access-
//  so just about any of the internal peripherals could be put to use.
//
//  Written by Ben Rose, TinyCircuits http://tinycircuits.com
//
//-------------------------------------------------------------------------------


#include <Wire.h>
#include <ServoDriver.h>

ServoDriver servo(NO_R_REMOVED);//this value affects the I2C address, which can be changed by
                                //removing resistors R1-R3. Then the corresponding R1_REMOVED,
                                //R2_REMOVED, R1_R2_REMOVED, R1_R4_REMOVED and so on can be set.
                                //Default is NO_R_REMOVED


#if defined (ARDUINO_ARCH_AVR)
#define SerialMonitorInterface Serial
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#endif

void setup(){
  SerialMonitorInterface.begin(9600);
  Wire.begin();
  
  //This will block until the Serial Monitor is opened on TinyScreen+/TinyZero platform!
  while(!SerialMonitorInterface && millis() < 5000)
  
  servo.useResetPin();         //This tells the library to use the reset line attached to Pin 9
  if(servo.begin(20000)){      //Set the period to 20000us or 20ms, correct for driving most servos
    SerialMonitorInterface.println("Motor driver not detected!");
    while(1);
  }
  //The failsafe turns off the PWM output if a command is not sent in a certain amount of time.
  //Failsafe is set in milliseconds- comment or set to 0 to disable
  servo.setFailsafe(1000);
}

void loop()
{
  for (int pos = 1000; pos <= 2000; pos += 30) {    // goes from 0 degrees to 180 degrees

    servo.setServo(1, pos);   // tell servo to go to position in variable 'pos'
    servo.setServo(2, pos);
    servo.setServo(3, pos);
    servo.setServo(4, pos);
    
    delay(20);                                  // wait 20ms for the servo to reach the position
  }
  for (int pos = 2000; pos >= 1000; pos -= 30) {    // goes from 180 degrees to 0 degrees
    servo.setServo(1, pos);   // tell servo to go to position in variable 'pos'
    servo.setServo(2, pos);
    servo.setServo(3, pos);
    servo.setServo(4, pos);
    
    delay(20);                                  // wait 20ms for the servo to reach the position
  }
}