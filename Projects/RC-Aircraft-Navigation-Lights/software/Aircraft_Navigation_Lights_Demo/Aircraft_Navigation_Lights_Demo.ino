//-------------------------------------------------------------------------------
//  TinyDuino RC Aircraft Navigation Lights Example Sketch
//  Last Updated 3/12/2018
//
//  This project controls a variety of aircraft navigation lights for radio
//  control airplanes using the millis() function to blink and pulse many LEDs
//  at different rates. This project also uses an RC receiver channel to turn
//  landing lights on or off, depending on a transmitter switch.
//
//  Written by Nick DiVitto for TinyCircuits, https://tinycircuits.com
//
//  This example is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//-------------------------------------------------------------------------------

//Pin Number Assignments:
const int wStrobe = 1;                //White wingtip strobes
const int Beacon = 0;                 //Red for & aft rotating beacons
const int Landing = 3;                //White landing lights
const int tStrobe = 4;                //White aft strobe

//LED States:
int wStrobeState = LOW;               //LED states to set LED
int BeaconState = LOW;
int tStrobeState = LOW;

//Time Variables:
unsigned long wStrobePreviousMillis = 0;     //Last time LED was updated
unsigned long tStrobePreviousMillis = 0;

volatile int pwm_value = 0;           //volotile variables for pwm measurement
volatile int prev_time = 0;

//Intervals                          //Sets time intervals for each on/off period
const long wStrobeON = 3;            //Time on in milliseconds
const long wStrobeOFF = 25;          //Time off in milliseconds
const long BeaconPeriod = 250;       //SINE period
const long tStrobeON = 7;            //Time on in milliseconds
const long tStrobeOFF = 50;          //Time off in milliseconds

void setup() {
  pinMode(wStrobe, OUTPUT);          //sets LED pins as outputs
  pinMode(Landing, OUTPUT);
  pinMode(tStrobe, OUTPUT);

  attachInterrupt(0, rising, RISING);    //When D2 goes high, call the RISING function
}

void loop() {
  unsigned long CurrentMillis = millis();

  //Landing Lights:
  if (pwm_value > 750) {                //If pulse width is larger than 750ms, landing lights should be on.
    digitalWrite(Landing, HIGH);
  }
  else {
    digitalWrite(Landing, LOW);
  }

  //wStrobe:
  if ((wStrobeState == HIGH) && (CurrentMillis - wStrobePreviousMillis >= wStrobeON)) {           //If wStrobe is on and it is time to turn it off...
    wStrobeState = LOW;               //Turn it off
    wStrobePreviousMillis = CurrentMillis;    //Remember the time
    digitalWrite(wStrobe, wStrobeState);      //Update the actual LED
  }
  else if ((wStrobeState == LOW) && (CurrentMillis - wStrobePreviousMillis >= wStrobeOFF)) {
    wStrobeState = HIGH;              //Turn it on
    wStrobePreviousMillis = CurrentMillis;    //Remember the time
    digitalWrite(wStrobe, wStrobeState);      //Update the actual LED
  }

  //Beacon (Sinusoidal Pulse):
  BeaconState = 128 + 127 * cos(2 * PI / BeaconPeriod * CurrentMillis); //Sets beacon state to follow a Sin wave depending on time
  analogWrite(Beacon, BeaconState);            //Sets analog output to match BeaconState

  //tStrobe:
  if ((tStrobeState == HIGH) && (CurrentMillis - tStrobePreviousMillis >= tStrobeON)) {           //If tStrobe is on and it is time to turn it off...
    tStrobeState = LOW;               //Turn it off
    tStrobePreviousMillis = CurrentMillis;    //Remember the time
    digitalWrite(tStrobe, tStrobeState);      //Update the LED
  }
  else if ((tStrobeState == LOW) && (CurrentMillis - tStrobePreviousMillis >= tStrobeOFF)) {
    tStrobeState = HIGH;              //Turn it on
    tStrobePreviousMillis = CurrentMillis;    //Remember the time
    digitalWrite(tStrobe, tStrobeState);      //Update the LED
  }
}

//functions for PWM Landing Gear channel measurement:
void rising() {
  attachInterrupt(0, falling, FALLING);
  prev_time = micros();
}

void falling() {
  attachInterrupt(0, rising, RISING);
  pwm_value = micros() - prev_time;
}
