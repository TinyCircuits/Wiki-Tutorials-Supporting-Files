#include "font.h"

int textType = 0;
int textLoopLED = 0;
int textLoopLEDchange = 1;
int lastX = 0;

unsigned long avg;
const int amtAvg = 5;
int avgBuf[5];
int avgPos = 0;

void POVLoop() {

  if (accel.doubleTap) {
    textType++;
    if (textType > 2)textType = 0;
    delay(50);
    accel.clearInterrupt();
  }

  long avgX = 0;
  int j;
  int positiveCount = 0;
  avgBuf[avgPos] = accel.X - lastX;
  avgPos++;
  if (avgPos >= amtAvg)
    avgPos = 0;
  for (j = 0; j < amtAvg; j++) {
    avgX += abs(avgBuf[j]);
    if (avgBuf[j] > 0)positiveCount++;
  }
  avgX /= (long)amtAvg;


  if (avgX > 25 && positiveCount > 3) {
    if (textType == 0) printText(text1Line1, text1Line2);
    if (textType == 1) printText(text2Line1, text2Line2);
    if (textType == 2) printText(text3Big);
    delay(100);
    for (j = 0; j < amtAvg; j++) {
      avgBuf[j] = 0;
    }
  }
  if (avgX > 25 && positiveCount < 2) {
    for (j = 0; j < amtAvg; j++) {
      avgBuf[j] = 0;
    }
  }
  lastX = accel.X;
  memset(led, 0, sizeof(led));
  if (textType == 0) led[textLoopLED].r = 20;
  if (textType == 1) led[textLoopLED].g = 20;
  if (textType == 2) led[textLoopLED].b = 20;
  textLoopLED += textLoopLEDchange;
  if (textLoopLED >= 15)textLoopLEDchange = -1;
  if (textLoopLED <= 0)textLoopLEDchange = 1;
}

int delayPerLine = 100;

void printText(char * s1, char * s2) {
  //initially 3ms during testing- 400=4ms
  //delayPerLine=400;
  while (*s1 && *s2 ) {
    write2(*s1, *s2 );
    s1++;
    s2++;
  }
}

void printText(char * s) {
  //initially 3ms during testing- 400=4ms
  //delayPerLine=400;
  while (*s) {
    write(*s);
    s++;
  }
}

void write(char c) {
  if (c < ' ' || c > '}' + 2)return;
  c -= ' ';
  int i, j;
  for (i = 0; i < 5; i++) {
    uint8_t h = pgm_read_word_near(Font5x7 + (c * 5) + i);
    uint16_t line = 0;
    for (j = 0; j < 8; j++) {
      if (h & (0x01 << j)) {
        line |= (3 << (j * 2));
      }
    }
    writeLine(line);
    ws2812_sendarray((uint8_t *)led, LEDCount * 3);
    for (j = 0; j < delayPerLine; j++) {
      delayMicroseconds(3);
    }
    int change = (delayPerLine - 50) / 10;
    if (delayPerLine > 20)
      delayPerLine -= change;
  }
  for (j = 0; j < 16; j++) {
    led[j].r = 0;
    led[j].g = 0;
    led[j].b = 0;
  }
  ws2812_sendarray((uint8_t *)led, LEDCount * 3);
  delay(3);
}

void write2(char c1, char c2) {
  if (c1 < ' ' || c1 > '}' + 2)return;
  if (c2 < ' ' || c2 > '}' + 2)return;
  c1 -= ' ';
  c2 -= ' ';
  int i, j;
  for (i = 0; i < 5; i++) {
    uint8_t h1 = pgm_read_word_near(Font5x7 + (c1 * 5) + i);
    uint8_t h2 = pgm_read_word_near(Font5x7 + (c2 * 5) + i);
    uint16_t line = (h2 << 8) | h1;
    writeLine(line);
    ws2812_sendarray((uint8_t *)led, LEDCount * 3);
    for (j = 0; j < delayPerLine; j++) {
      delayMicroseconds(10);
    }
    int change = (delayPerLine - 50) / 10;
    if (delayPerLine > 20)
      delayPerLine -= change;
  }
  for (j = 0; j < 16; j++) {
    led[j].r = 0;
    led[j].g = 0;
    led[j].b = 0;
  }
  ws2812_sendarray((uint8_t *)led, LEDCount * 3);
  delay(3);
}

void writeLine(uint16_t line) {
  uint8_t j;
  for (j = 0; j < 16; j++) {
    if (line & (0x8000 >> j)) {
      led[j].r = 255;
      led[j].g = 255;
      led[j].b = 255;
    } else {
      led[j].r = 0;
      led[j].g = 0;
      led[j].b = 0;
    }
  }
}

