#include "capTouch.h"
#include <Arduino.h>
#include <util/delay.h>

capTouch::capTouch()
{
}

void capTouch::begin()
{
  ADCSRA = _BV(ADEN) | 4 | 1;
  uint8_t pin;
  for (pin = 0; pin < numSensors; pin++) {
    capTouchCal[pin] = getCapReading(pin);
    overCalCount[pin] = 0;
  }
}

int capTouch::scaleToRange(int range) {
  int output = primary * range / (numSensors - 1);
  if (primary < 5)
    output += (capTouchCurrent[primary + 1] / (getMaxReading() / 2));
  if (primary > 0)
    output -= (capTouchCurrent[primary - 1] / (getMaxReading() / 2));
  return output;
}

int capTouch::getMaxReading() {
  int maxReading = 0;
  uint8_t j;
  for (j = 0; j < 6; j++) {
    if (capTouchCurrent[j] > maxReading) {
      maxReading = capTouchCurrent[j];
      primary = j;
    }
  }
  return maxReading;
}

int capTouch::getMagnitude() {
  uint8_t j;
  long magnitude = 0;
  for (j = 0; j < 6; j++) {
    magnitude += pow(capTouchCurrent[j], 2);
  }
  return sqrt(magnitude);
}

uint8_t capTouch::read() {
  uint8_t j;
  for (j = 0; j < 6; j++) {
    int val = getCapReading(j);
    if (val < capTouchCal[j]) {
      capTouchCal[j] = val;
    } else if (val > capTouchCal[j] + 1) {
      overCalCount[j]++;
      if (overCalCount[j] > 3) {
        overCalCount[j] = 0;
        capTouchCal[j] += 1;
      } else {
        //overCalCount[j]=0;
      }
    }
    capTouchCurrent[j] = val  - capTouchCal[j];
  }

  uint8_t completedTouch = 0;

  if (getMaxReading() > 20) {
    if (!isTouch) {
      isTouch = 1;
      touchTimer = millis();
      initialTouchPos = primary;
      finalTouchPos = primary;
      currentTouchedPositions = 0;
      allTouchedPositions = 0;
    } else {
      finalTouchPos = primary;
      currentTouchedPositions = 0;
      for (j = 0; j < 6; j++) {
        if (capTouchCurrent[j] > 12)
          currentTouchedPositions |= (1 << j);
        allTouchedPositions |= (1 << j);
      }
    }
  } else {
    if (isTouch) {
      isTouch = 0;
      touchTimer = millis() - touchTimer;
      completedTouch = 1;
    }
  }

  return completedTouch;
}

uint32_t capTouch::duration() {
  if (isTouch) {
    return millis() - touchTimer;
  }
  return touchTimer;
}

unsigned int capTouch::getCapReading(uint8_t pin) {
  unsigned int i, avg = 0;
  for (i = 0; i < 5; i++) {
    avg += capRead(pin);
  }
  avg /= 5;
  return avg;
}

unsigned int capTouch::capRead(uint8_t pin) {
  PUEA |= (1 << pin); //charge external cap

  ADMUXA = 0x0E; //connect ADC input cap to GND
  ADCSRA |= _BV(ADSC); //run ADC
  while (ADCSRA & _BV(ADSC));

  PUEA &= ~(1 << pin); //float external cap

  ADMUXA = (pin) & 0x0F; //connect ADC cap to external cap
  ADCSRA |= _BV(ADSC); //run ADC
  while (ADCSRA & _BV(ADSC));

  unsigned int result = ADCL; //result is proportional to external/internal
  return (ADCH << 8) | result;
}

