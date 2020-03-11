/*
  TinyCircuits Wireling Driver for RGB LED

  This program uses three separate Wireling inputs to drive a strip of RGB LEDs.

  Port 0: RGB LED Strip
  Port 1: Capacitive Touch  - drives the brightness of the LEDs
  Port 2: Rotary            - drives the color of the LEDs
  Port 3: Joystick          - drives the position and number of LEDs shown

  Written March 2020
  By Hunter Hykes

  https://TinyCircuits.com
*/

#include <Wire.h>
#include <Wireling.h>
#include <FastLED.h>            // For interfacing with the RGB LED
#include "SX1505.h"             // For interfacing with Joystick and Rotary Switch Wirelings
#include <ATtiny841Lib.h>       // For ATtiny841 on Cap Touch Wireling
#include <CapTouchWireling.h>   // For Cap Touch Wireling

// Universal Serial Monitor Config
#if defined (ARDUINO_ARCH_AVR)
#define SerialMonitorInterface Serial
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#endif

/* * * * * * * * * * RGB LED * * * * * * * * * */
#define NUM_LEDS 60 //this is the number of LEDs in your strip
#define DATA_PIN A0 //this is the number on the silkscreen you want to use
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
int brightness = 10; //value from 0-255 to manipulate brightness
CRGB color = CRGB(0, 255, 0); // (g, r, b) -> red

/* * * * * * * * * Cap Touch * * * * * * * * * */
#define CAP_PORT 2
CapTouchWireling capTouch;
int pos;
int mag;

/* * * * * * * * * * Rotary * * * * * * * * * */
#define ROT_PORT 1
TinyRotary rotary = TinyRotary();
uint8_t rotaryValue;

/* * * * * * * * * * Joystick * * * * * * * * * */
#define JS_PORT 3
TinyJoystick joystick = TinyJoystick();

int width = 1, LEDpos = 0;

void setup() {
  SerialMonitorInterface.begin(9600);
  Wire.begin();
  Wireling.begin();

  /* * * * * * * RGB LED * * * * * * */
  pinMode(DATA_PIN, OUTPUT);
  FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);
  
  /* * * * * * Cap Touch * * * * * * */
  Wireling.selectPort(CAP_PORT);
  while(millis() < 5000);
  if(capTouch.begin()) {
    SerialMonitorInterface.println("Capacitive touch Wireling not detected!");
  }

  /* * * * * * Rotary Init * * * * */
  Wireling.selectPort(ROT_PORT);
  rotary.begin();

  /* * * * * * Joystick Init * * * * */
  Wireling.selectPort(JS_PORT);
  joystick.begin();

  delay(500);
}

void loop() {
  // Read Rotary
  Wireling.selectPort(ROT_PORT);
  rotaryValue = rotary.getPosition();

  // Read Joystick
  Wireling.selectPort(JS_PORT);
  joystick.getPosition();
  
  // Read Capacitive Touch Slider
  Wireling.selectPort(CAP_PORT);
  getCapPos();

  // Output to the LEDs based on input
  driveLEDs();
  //writeToMonitor(); // for debugging inputs
}

void driveLEDs() {
  updateFromJS();
  updateLEDs();

  // this is hardcoded for speed purposes
  for (int i = 0; i < LEDpos; i++) {
    leds[i] = CRGB(0, 0, 0);  // set leading "unused" LEDs to "off"
    FastLED.show();           // update the LEDs
  }
  for (int i = LEDpos; i < LEDpos + width; i++) {
    leds[i] = color;          // set active LEDs to desired color (from rotary switch)
    FastLED.show();           // update the LEDs
  }
  for (int i = LEDpos + width; i < NUM_LEDS; i++) {
    leds[i] = CRGB(0, 0, 0);  // set trailing "unused" LEDs to "off"
    FastLED.show();           // update the LEDs
  }
}

// drive LED color based on value received by Rotary Switch
void updateLEDs() {
  switch (rotaryValue) {
    case 0:
      color = CRGB(0, 255, 0); // (g, r, b) -> red
      break;

    case 1:
      color = CRGB(140, 220, 0); // (g, r, b) -> orange
      break;

    case 2:
      color = CRGB(220, 140, 0); // (g, r, b) -> yellow
      break;

    case 3:
      color = CRGB(255, 0, 0); // (g, r, b) -> green
      break;

    case 4:
      color = CRGB(220, 0, 140); // (g, r, b) -> green/blue
      break;

    case 5:
      color = CRGB(140, 0, 220); // (g, r, b) -> blue/green
      break;

    case 6:
      color = CRGB(0, 0, 255); // (g, r, b) -> blue
      break;

    case 7:
      color = CRGB(0, 140, 220); // (g, r, b) -> indigo
      break;

    case 8:
      color = CRGB(0, 220, 140); // (g, r, b) -> violet
      break;

    case 9:
      color = CRGB(255, 255, 255); // (g, r, b) -> white
      break;
  }
}

void updateFromJS() {
  int wCap = NUM_LEDS - LEDpos;
  int posCap = NUM_LEDS - width;

  if (joystick.left) {
    LEDpos = constrain(LEDpos + 1, 0, posCap);  // shift LEDs to the left
  } else if (joystick.right) {
    LEDpos = constrain(LEDpos - 1, 0, posCap);  // shift LEDs to the right
  } else if (joystick.up) {
    width = constrain(width + 1, 1, wCap);      // increase the number of LEDs that are on
  } else if (joystick.down) {
    width = constrain(width - 1, 1, wCap);      // decrease the number of LEDs that are on
  }
}

void getCapPos() {
  bool completedTouch = capTouch.update();
  if (completedTouch) {
    // do something on completion of touch
  }

  if (capTouch.isTouched()) {
    pos = capTouch.getPosition();  //getPosition() returns a position value from 0 to 100 across the sensor
    mag = capTouch.getMagnitude(); //getMagnitude() returns a relative 'pressure' value of about 50 to 500

    brightness = map(pos, 0, 100, 0, 255);

    FastLED.setBrightness(brightness);
  }
}

// updates all LEDs to the RGB value specified
void setLEDs(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(g, r, b);
    FastLED.show(); //update the LEDs
  }
}

// this can be used to debug inputs received from the Rotary Switch, Joystick, and Capacitive Touch Slider
void writeToMonitor() {
  // Rotary Switch value
  SerialMonitorInterface.println("Rotary: " + String(rotaryValue));

  // Joystick values
  SerialMonitorInterface.print("Joystick: ");
  if (joystick.up) {
    SerialMonitorInterface.print("UP\t");
  }
  else if (joystick.down) {
    SerialMonitorInterface.print("DOWN\t");
  }
  else if (joystick.left) {
    SerialMonitorInterface.print("LEFT\t");
  }
  else if (joystick.right) {
    SerialMonitorInterface.print("RIGHT\t");
  }
  SerialMonitorInterface.println();

  // Capacitive Touch Slider values
  SerialMonitorInterface.println("Cap Touch: " + String(pos));
  
  delay(100);
}
