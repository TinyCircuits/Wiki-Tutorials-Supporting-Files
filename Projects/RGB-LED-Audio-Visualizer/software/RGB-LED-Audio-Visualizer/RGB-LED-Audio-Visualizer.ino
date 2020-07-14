/*
  TinyCircuits RGB LED Audio Visualizer

  This sketch takes input from the MEMS Microphone
  Wireling and outputs a lighting effect on a strand
  of RGB LEDs. Brightness can be adjusted if a
  Rotary Switch Wireling is used as well.

  This code is set up in the following way
  Port 0: LED Strand (line 30)
    60 RGB LEDs are present in the strand. (line 29)
  Port 1: Microphone Wireling (line 34)
  Port 2: N/A
  Port 3: Rotary Switch Wireling (line 38)
    10 positions are present on the rotary switch (line 39)

  Written 15 July 2019
  By Hunter Hykes
  Modified 14 July 2020

  https://TinyCircuits.com
*/

#include <Wire.h>
#include <Wireling.h>
#include <FastLED.h>
#include <SX1505.h>       // For interfacing with Joystick and Rotary Switch Wirelings

#define NUM_LEDS 60
#define LED_PIN A0        // RGB LED Port
#define LED_PORT 0
#define COLOR_ORDER GRB

#define MIC_PIN A1        // Microphone Wireling pin (Port 1)
#define maxMicRange 511
#define micMidpoint 255

#define ROT_PORT 3        // Rotary Switch Wireling Port
#define NUM_POSITIONS 10  // number of positions on Rotary Switch
TinyRotary rotary = TinyRotary();

const int brightness = 50;
const int updateXLEDs = 2; // number of LEDs to populate with each sound
const double totalDelay = 1000.0; // number of milliseconds to populate whole strip
const double indDelay = totalDelay/NUM_LEDS; // number of milliseconds to delay after each LED

CRGB leds[NUM_LEDS];
CRGB color;
CRGB prevColor;

float sample;
float prevSample;

void setup() {
  Wire.begin();
  Wireling.begin();

  // start communicaiton to the Rotary Switch Wireling
  Wireling.selectPort(ROT_PORT);
  rotary.begin();

  // initialize RGB LEDs
  FastLED.addLeds<WS2812, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);
  pinMode(LED_PIN, OUTPUT);
  color = CRGB::Black;
  populateLEDs();

  SerialUSB.begin(9600);
  delay(500);
}

void loop() {
  sample = analogRead(MIC_PIN); // get a new microphone sample
  SerialUSB.print("Mic: " + String(sample));

  FastLED.setBrightness(setBright()); // update brightness based on Rotary Switch reading
  SerialUSB.println("\tRotary: " + String(setBright()));

  setColor();
  shiftLEDs();
  delay(indDelay);
  populateLEDs();
}

void shiftLEDs() {
  for(int i = NUM_LEDS - 1; i >= updateXLEDs; i--) {
    leds[i] = leds[i - updateXLEDs];
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
  if (sample > micMidpoint + 140) {
    color = CRGB::White;
  } else if (sample > micMidpoint + 120) {
    color = CRGB::Red;
  } else if (sample > micMidpoint + 100) {
    color = CRGB::Orange;
  } else if (sample > micMidpoint + 80) {
    color = CRGB::Yellow;
  } else if (sample > micMidpoint + 60) {
    color = CRGB::Green;
  } else if (sample > micMidpoint + 40) {
    color = CRGB::Blue;
  } else if (sample > micMidpoint + 20) {
    color = CRGB::Purple;
  } else {
    color = CRGB::Black;
  }
}

int setBright() {
  uint8_t rotaryValue = rotary.getPosition();

  return (255.0/NUM_POSITIONS) * (rotaryValue + 1);
}
