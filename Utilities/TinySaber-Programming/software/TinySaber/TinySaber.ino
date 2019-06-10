//-------------------------------------------------------------------------------
//  TinyCircuits TinySaber Firmware
//
//  TinySaber is a fun, low cost kit that is effectively a development board for
//  the ATtiny841 micro with 16 RGB LEDs, capacitive touch pads, an accelerometer,
//  software USB and other features. Arduino/any support for this is only possible
//  due to a few key projects:
//  Micronucleus bootloader: https://github.com/micronucleus/micronucleus
//  Digistump: http://digistump.com/ , https://github.com/digistump/DigistumpArduino
//  ATTinyCore: https://github.com/SpenceKonde/ATTinyCore
//
//  Changelog:
//  1.0.0 5 July 2017 Initial release
//
//  Written by Ben Rose for TinyCircuits, http://TinyCircuits.com
//
//-------------------------------------------------------------------------------

#include "light_ws2812.h"
#include "light_ws2812.c"
#include "BMA250.h"
#include "capTouch.h"

BMA250 accel;
capTouch touch;

/*
  Notes:
  Accel X is left/right wave
  Accel Y is up/down
  LED write = 550us
  Accel read = 300us
  LEDs are 0-15 extending from handle
  Sensors are 0-5 extending from handle
*/

#define text1Line1 "Tiny "
#define text1Line2 "Saber"

#define text2Line1 "Maker"
#define text2Line2 "Faire"

#define text3Big   "TinyCircuits"

#define LEDCount  16

struct cRGB led[LEDCount];
const struct cRGB red = {255, 0, 0};
const struct cRGB green = {0, 255, 0};
const struct cRGB blue = {0, 0, 255};
const struct cRGB white = {255, 255, 255};
const struct cRGB off = {0, 0, 0};

const uint8_t saberMode = 1;
const uint8_t rainbowMode = 2;
const uint8_t flashLightMode = 3;
const uint8_t textMode = 4;
uint8_t mode = 0;

int saberColor = 0;

void setup(void) {
  delay(100);
  touch.begin();
  delay(100);
  accel.begin(BMA250_range_8g, BMA250_update_time_4ms);
  accel.read();

  memset(led, 0, sizeof(led));
  changeMode(saberMode);
}


void loop() {
  accel.read();

  int completedTouch = touch.read();

  if (mode == saberMode) {
    saberModeLoop(touch.getMaxReading(), touch.scaleToRange(LEDCount - 1));
  } else if (mode == rainbowMode) {
    rainbowLoop();
  } else if (mode == flashLightMode) {
    //
  } else if (mode == textMode) {
    POVLoop();
  } else if (!mode) {
    //sleepMode
  }


  if (completedTouch) {
    if (touch.initialTouchPos == 0 && touch.finalTouchPos == 5) {
      if (touch.allTouchedPositions == (TS(5) | TS(4) | TS(3) | TS(2) | TS(1) | TS(0))) {
        changeMode(saberMode);
      }
    } else if (touch.initialTouchPos >= 4 && touch.finalTouchPos == 0) {
      if (touch.allTouchedPositions == (TS(5) | TS(4) | TS(3) | TS(2) | TS(1) | TS(0)) ||
          touch.allTouchedPositions == (TS(4) | TS(3) | TS(2) | TS(1) | TS(0))) {
        changeMode(0);
      }
    } else if (touch.duration() > 70 && touch.duration() < 400) {
      if (mode) {
        changeMode(saberMode);
      }
    }
  }
  if (mode) {
    if (touch.currentTouchedPositions == (TS(5) | TS(0)))
      changeMode(flashLightMode);

    if (touch.currentTouchedPositions == (TS(2) | TS(0)))
      changeMode(rainbowMode);

    if (touch.currentTouchedPositions == (TS(3) | TS(0)))
      changeMode(textMode);
  }

  ws2812_sendarray((uint8_t *)led, LEDCount * 3);
}

uint32_t changeModeTimeout = 0xFFFF;

void changeMode(uint8_t newMode) {
  if (millis() - changeModeTimeout < 500) return;
  if (newMode == mode)return;
  changeModeTimeout = millis();
  accel.clearInterrupt();
  if (newMode == saberMode) {
    fadeTo(off);
    saberModeInit();
    for (uint8_t j = 0; j < 16; j++) {
      if (saberColor == 0) {
        led[j].r = 5;
      } 
      if (saberColor == 1) {
        led[j].g = 5;
      }
      if (saberColor == 2) {
        led[j].b = 5;
      }
      ws2812_sendarray((uint8_t *)led, LEDCount * 3);
      delay(30);
    }
  } else if (newMode == rainbowMode) {
    rainbowLoop();
  } else if (newMode == flashLightMode) {
    fadeTo(white);
  } else if (newMode == textMode) {
  } else if (!newMode) {
    fadeTo(off);
  }
  mode = newMode;
}

void fadeTo(struct cRGB color) {
  uint8_t done = false;
  uint16_t i;
  while (!done) {
    done = true;
    for (i = 0; i < LEDCount; i++) {
      if (led[i].r != color.r) {
        done = false;
        if (led[i].r < color.r) {
          led[i].r++;
        } else {
          led[i].r--;
        }
      }
      if (led[i].g != color.g) {
        done = false;
        if (led[i].g < color.g) {
          led[i].g++;
        } else {
          led[i].g--;
        }
      }
      if (led[i].b != color.b) {
        done = false;
        if (led[i].b < color.b) {
          led[i].b++;
        } else {
          led[i].b--;
        }
      }
    }
    if (!done)ws2812_sendarray((uint8_t *)led, LEDCount * 3);
    if (!done)delay(3);
  }
}

uint32_t rainbowTimer = 0;
uint8_t rainbowCount = 0;

void rainbowLoop() {
  if (millis() - rainbowTimer < 30) return;
  rainbowTimer = millis();
  uint16_t i;
  for (i = 0; i < LEDCount; i++) {
    Wheel(i, (i + rainbowCount) * 4 & 255);
  }
  rainbowCount++;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
void Wheel(uint8_t i, uint8_t WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    led[i].r = (uint16_t)(255 - WheelPos * 3);
    led[i].g = (uint16_t)(0);
    led[i].b = (uint16_t)(WheelPos * 3);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    led[i].r = (uint16_t)(0);
    led[i].g = (uint16_t)(WheelPos * 3);
    led[i].b = (uint16_t)(255 - WheelPos * 3);
  } else {
    WheelPos -= 170;
    led[i].r = (uint16_t)(WheelPos * 3);
    led[i].g = (uint16_t)(255 - WheelPos * 3);
    led[i].b = (uint16_t)(0);
  }
  return;
}
