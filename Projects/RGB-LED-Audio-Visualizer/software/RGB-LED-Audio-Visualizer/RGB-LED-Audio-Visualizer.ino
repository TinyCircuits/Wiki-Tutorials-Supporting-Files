/*
  TinyCircuits RGB LED Audio Visualizer

  This sketch takes input from the MEMS Microphone
  Whisker and outputs a lighting effect on a strand
  of RGB LEDs. Brightness can be adjusted if a
  Rotary Switch Whisker is used as well.

  This code is set up in the following way
  Port 0: LED Strand (line 35)
    60 RGB LEDs are present in the strand. (line 33)
  Port 1: Microphone Whisker (line 29)
  Port 3: Rotary Switch Whisker (line 39)
    10 positions are present on the rotary switch (line 48)

  Written 15 July 2019
  By Hunter Hykes
  Modified N/A
  By N/A

  https://TinyCircuits.com
*/

#include <Wire.h>
#include <FastLED.h>
#include "SH7010.h"

#define MIC_PIN A1 // Microphone whisker pin (port 1)
#define MIC_PORT 1
#define maxMicRange 511
#define micMidpoint 255

#define NUM_LEDS 60
#define LED_PIN A0 // port 0 on whisker board
#define LED_PORT 0
#define COLOR_ORDER GRB

#define NUM_POSITIONS 10 // number of positions on rotary switch
#define ROT_PORT 3 // port 3 on whisker board
SH7010 rotary = SH7010();

const int powerPin = 4;
const int brightness = 50;
const int updateXLEDs = 2; // number of LEDs to populate with each sound
const double totalDelay = 1000.0; // number of milliseconds to populate whole strip
const double indDelay = totalDelay/NUM_LEDS; // number of milliseconds to delay after each LED
CRGB leds[NUM_LEDS];
CRGB color;
CRGB prevColor;

float sample;
float prevSample;

unsigned long frequency;
unsigned long periodStart;
unsigned long periodEnd;

// Arduino setup Method
void setup() {
  Wire.begin();
  FastLED.addLeds<WS2812, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);
  pinMode(LED_PIN, OUTPUT);
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);

  rotary.begin();
  rotary.init();
  selectPort(ROT_PORT);

  color = CRGB::Black;
  populateLEDs();
  
  SerialUSB.begin(9600);
  delay(500);
}

// Arduino loop Method
void loop() {
  //selectPort(MIC_PORT);
  sample = analogRead(MIC_PIN);   // get a new sample
  //SerialUSB.println(sample);

  FastLED.setBrightness(setBright());
  SerialUSB.println(setBright());
  //frequency = getFrequency();
  //SerialUSB.println(frequency);
  //selectPort(LED_PORT);
  setColor();
  shiftLEDs();
  delay(indDelay);
  populateLEDs();

  prevSample = sample;
}

void selectPort(int port) {
  Wire.beginTransmission(0x70);
  Wire.write(0x04 + port);
  Wire.endTransmission();
}

int getSampleAmplitude() {
  int minSample = maxMicRange;
  int maxSample = 0;

  if (sample > maxSample) {
    maxSample = sample;
  }
  if (sample < minSample) {
    minSample = sample;
  }

  return (minSample + maxSample) >> 1; // >> 1 is the same as dividing by 2
}

unsigned long getFrequency() {
  unsigned long period;

  if (prevSample < micMidpoint && sample >= micMidpoint) { // if midpoint is crossed with a positive slope
    periodStart = periodEnd; // end of the previous period is the start of the new period
    periodEnd = millis();
    period = periodEnd - periodStart;
    //SerialUSB.println(period);
    return (1 / period);  // frequency = 1 / period
  } else {
    return 0.0;
  }
}

void shiftLEDs() {
  for(int i = NUM_LEDS - 1; i >= updateXLEDs; i--) {
    leds[i] = leds[i - updateXLEDs];
    //delay(indDelay);
  }
}

void populateLEDs() {
  for (int i = 0; i < updateXLEDs; i++) {
    //leds[i] = color;
    leds[i] = blend(prevColor, color, .50);
  }

  FastLED.show();
}

void setColor() {
  prevColor = color;
  if (sample > micMidpoint + 223) {
    color = CRGB::White;
  } else if (sample > micMidpoint + 191) {
    color = CRGB::Red;
  } else if (sample > micMidpoint + 159) {
    color = CRGB::Orange;
  } else if (sample > micMidpoint + 127) {
    color = CRGB::Yellow;
  } else if (sample > micMidpoint + 95) {
    color = CRGB::Green;
  } else if (sample > micMidpoint + 63) {
    color = CRGB::Blue;
  } else if (sample > micMidpoint + 31) {
    color = CRGB::Purple;
  } else {
    color = CRGB::Black;
  }
}

int setBright() {
  uint8_t rotaryValue = rotary.getPosition();

  return (255.0/NUM_POSITIONS) * (rotaryValue + 1);
}
