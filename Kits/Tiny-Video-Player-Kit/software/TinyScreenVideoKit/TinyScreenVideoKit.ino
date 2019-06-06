//-------------------------------------------------------------------------------
//  TinyCircuits Tiny Arcade Video Player
//
//  Changelog:
//  1.0.3 15 Nov 3018 - Add option for TinyScreen+ button input
//  1.0.2 10 April 2017 - Cleanup, add messages about SD card/video detection
//  1.0.1 3 October 2016 - Add compatibility for new Arcade revision, change
//        folder structure for IDE compatibility
//  1.0.0 Initial release
//
//  Written by Ben Rose for TinyCircuits, http://TinyCircuits.com
//
//-------------------------------------------------------------------------------

#include <TinyScreen.h>
#include "SdFat.h"
#include <Wire.h>
#include <SPI.h>

TinyScreen display = TinyScreen(TinyScreenPlus);

#define compileForArcade false

#if compileForArcade
#include "TinyArcade.h"
#else
//TinyScreen+ adaption
const uint8_t TAJoystickLeft  = TSButtonLowerLeft;
const uint8_t TAJoystickRight = TSButtonLowerRight;
const uint8_t TAButton1  = TSButtonUpperRight;
const uint8_t TAButton2 = TSButtonUpperLeft;

uint8_t checkButton(uint8_t btn) {
  return display.getButtons(btn);
}

uint8_t checkJoystick(uint8_t btn) {
  return display.getButtons(btn);
}

#endif


SdFat sd;
SdFile dir;
SdFile vidFile;
uint8_t buffer[96 * 64 * 2];
uint16_t audioBuffer[1024];
volatile uint32_t sampleIndex = 0;

int currentFileNum = 0;
int doVideo = 0;
int foundValidVideo = 0;
char currentVideoName[50] = "No video found";

int printNicelyOffset = 0;
int printNicelyDir = 0;
unsigned long lastPrintNicelyOffsetChange = 0;
unsigned long printNicelyOffsetChangeInterval = 200;

void setup(void) {
  display.begin();
  display.setFlip(true);
#ifdef arcadeInit
  arcadeInit();
  display.setFlip(false);
#endif
  display.setBitDepth(1);
  display.setFont(thinPixel7_10ptFontInfo);
  display.initDMA();
  SPI.begin();
  if (!sd.begin(10, SPI_FULL_SPEED)) {
    int xPosition = 48 - ((int)display.getPrintWidth("Card not found!") / 2);
    display.setCursor(xPosition, 26);
    display.print("Card not found!");
    while (1);
  }
  SPI.setClockDivider(0);

  //audio output config
  analogWrite(A0, 0);
  tcConfigure(30720);

  printNextFile();
}
int blockInput = 1;

void loop() {
  if (!checkJoystick(TAJoystickLeft) && !checkJoystick(TAJoystickRight)) {
    blockInput = 0;
  }
  if (!blockInput && checkJoystick(TAJoystickRight)) {
    printNextFile();
    blockInput = 1;
  }
  if (!blockInput && checkJoystick(TAJoystickLeft)) {
    printPreviousFile();
    blockInput = 1;
  }
  if (checkButton(TAButton1)  && currentFileNum) {
    if (doVideo)
      playVideo();
  }
  if (doVideo) {
    bufferVideoFrame();
    printNicely(currentVideoName);
    writeToDisplay();
  } else {
    memset(buffer, 0, 96 * 64 * 2);
    printNicely(currentVideoName);
    writeToDisplay();
  }
}

void bufferVideoFrame() {
  if (!vidFile.available())
    vidFile.rewind();
  vidFile.read(buffer, 96 * 64 * 2);
  vidFile.read((uint8_t*)audioBuffer, 1024 * 2);
}

void writeToDisplay() {
  display.goTo(0, 0);
  display.startData();
  display.writeBuffer(buffer, 96 * 64 * 2);
  display.endTransfer();
}

void printNextFile() {
  doVideo = 0;
  vidFile.close();
  int foundVideo = 0;
  while (!foundVideo) {
    dir.close();
    if (dir.openNext(sd.vwd(), O_READ)) {
      currentFileNum++;
    } else if (!foundValidVideo) {
      //This should mean there are no valid videos or SD card problem
      int xPosition = 48 - ((int)display.getPrintWidth("No video files!") / 2);
      display.setCursor(xPosition, 26 - 10);
      display.print("No video files!");
      xPosition = 48 - ((int)display.getPrintWidth("Place .TSV files") / 2);
      display.setCursor(xPosition, 26);
      display.print("Place .TSV files");
      xPosition = 48 - ((int)display.getPrintWidth("in main directory.") / 2);
      display.setCursor(xPosition, 26 + 10);
      display.print("in main directory.");
      while (1);
    } else {
      sd.vwd()->rewind();
      dir.openNext(sd.vwd(), O_READ);
      currentFileNum = 1;
    }
    if (dir.isFile()) {
      char fileName[100];
      memset(fileName, 0, 100);
      dir.getName(fileName, 50);
      if (!strcmp(fileName + strlen(fileName) - 4, ".tsv")) {
        dir.close();
        if (vidFile.open(fileName, O_READ)) {
          doVideo = 1;
          foundValidVideo = 1;
          foundVideo = 1;
        }
      }
    }
  }
  vidFile.getName(currentVideoName, 50);
  currentVideoName[strlen(currentVideoName) - 4] = '\0';
  printNicelyOffset = 0;
  printNicelyDir = 0;
  lastPrintNicelyOffsetChange = millis();
}

void printPreviousFile() {
  doVideo = 0;
  vidFile.close();
  int foundVideo = 0;
  while (!foundVideo) {
    if (currentFileNum > 1) {
      currentFileNum--;
    } else {
      currentFileNum = 100;
      int i;
      for (i = 1; i < currentFileNum; i++) {
        dir.close();
        if (!dir.openNext(sd.vwd(), O_READ))break;
      }
      currentFileNum = i;
    }
    sd.vwd()->rewind();
    int i;
    for (i = 1; i <= currentFileNum; i++) {
      dir.close();
      if (!dir.openNext(sd.vwd(), O_READ))break;
    }
    if (dir.isFile()) {
      char fileName[100];
      memset(fileName, 0, 100);
      dir.getName(fileName, 50);
      if (!strcmp(fileName + strlen(fileName) - 4, ".tsv")) {
        dir.close();
        if (vidFile.open(fileName, O_READ)) {
          doVideo = 1;
          foundVideo = 1;
        }
      }
    }
  }
  vidFile.getName(currentVideoName, 50);
  currentVideoName[strlen(currentVideoName) - 4] = '\0';
  printNicelyOffset = 0;
  printNicelyDir = 0;
  lastPrintNicelyOffsetChange = millis();
}

void printNicely(char * name) {
  int yOffset = 34;
  for (int y = yOffset + 0; y < yOffset + 12; y++) {
    for (int x = 0; x < 96; x++) {
      ((uint16_t *)buffer)[(y * 96) + x] = (((uint16_t *)buffer)[(y * 96) + x] >> 2) & 0xE739; //0xEF7B
    }
  }
  char displayName[50];
  strcpy(displayName, name + printNicelyOffset);

  int xPosition = 48 - ((int)display.getPrintWidth(displayName) / 2);

  if (millis() > lastPrintNicelyOffsetChange + printNicelyOffsetChangeInterval) {
    lastPrintNicelyOffsetChange = millis();
    if (xPosition < 9) {
      printNicelyOffset++;
    } else {
      printNicelyOffset = 0;
    }
  }

  while (xPosition < 9) {
    displayName[strlen(displayName) - 1] = '\0';
    xPosition = 48 - ((int)display.getPrintWidth(displayName) / 2);
  }
  /*display.setCursor(1, 1);
    display.print("<                               ");
    display.setCursor(87, 1);
    display.print(">");
    display.setCursor(xPosition, 1);
    display.print(name);*/

  for (int y = yOffset + 1; y < yOffset + 15; y++) {
    putString(y, xPosition, yOffset + 1, displayName, buffer + (96 * 2 * y), thinPixel7_10ptFontInfo);
  }
  char chbuf[] = "<";
  for (int y = yOffset + 1; y < yOffset + 15; y++) {
    putString(y, 1, yOffset + 1, chbuf, buffer + (96 * 2 * y), thinPixel7_10ptFontInfo);
  }
  chbuf[0] = '>';
  for (int y = yOffset + 1; y < yOffset + 15; y++) {
    putString(y, 87 + 5, yOffset + 1, chbuf, buffer + (96 * 2 * y), thinPixel7_10ptFontInfo);
  }
}

void putString(int y, int fontX, int fontY, char * string, uint8_t * buff, const FONT_INFO & fontInfo) {
  const FONT_CHAR_INFO* fontDescriptor = fontInfo.charDesc;
  int fontHeight = fontInfo.height;
  if (y >= fontY && y < fontY + fontHeight) {
    const unsigned char* fontBitmap = fontInfo.bitmap;
    int fontFirstCh = fontInfo.startCh;
    int fontLastCh = fontInfo.endCh;
    //if(!_fontFirstCh)return 1;
    //if(ch<_fontFirstCh || ch>_fontLastCh)return 1;
    //if(_cursorX>xMax || _cursorY>yMax)return 1;
    int stringChar = 0;
    int ch = string[stringChar++];
    while (ch) {
      uint8_t chWidth = pgm_read_byte(&fontDescriptor[ch - fontFirstCh].width);
      int bytesPerRow = chWidth / 8;
      if (chWidth > bytesPerRow * 8)
        bytesPerRow++;
      unsigned int offset = pgm_read_word(&fontDescriptor[ch - fontFirstCh].offset) + (bytesPerRow * fontHeight) - 1;

      for (uint8_t byte = 0; byte < bytesPerRow; byte++) {
        uint8_t data = pgm_read_byte(fontBitmap + offset - (y - fontY) - ((bytesPerRow - byte - 1) * fontHeight));
        uint8_t bits = byte * 8;
        for (int i = 0; i < 8 && (bits + i) < chWidth; i++) {
          if (data & (0x80 >> i)) {
            buff[(fontX) * 2] = 0xFFFF >> 8;
            buff[(fontX) * 2 + 1] = 0xFFFF;
            // setPixelInBuff(y,16+fontX,0);
            //lineBuffer[16+fontX]=0;
          } else {
            //SPDR=_fontBGcolor;
          }
          fontX++;
        }
      }
      fontX += 1;
      ch = string[stringChar++];
    }
  }
}

void showPause() {
  int top = 20;
  int bottom = 40;
  int left = (96 / 2) - 10;
  int right = left + 8;
  for (int y = top; y < bottom; y++) {
    for (int x = left; x < right; x++) {
      byte color = 0xFF;
      if (y == top || y == bottom - 1)color = 0;
      if (x == left || x == right - 1)color = 0;
      buffer[(((y * 96) + x) * 2)] = color;
      buffer[(((y * 96) + x) * 2) + 1] = color;
    }
  }
  left = (96 / 2) + 2;
  right = left + 8;
  for (int y = top; y < bottom; y++) {
    for (int x = left; x < right; x++) {
      byte color = 0xFF;
      if (y == top || y == bottom - 1)color = 0;
      if (x == left || x == right - 1)color = 0;
      buffer[(((y * 96) + x) * 2)] = color;
      buffer[(((y * 96) + x) * 2) + 1] = color;
    }
  }
}

void showPlay() {
  int top = 20;
  int bottom = 40;
  int left = (96 / 2) - 3;
  int right = left;
  for (int y = top; y < bottom; y++) {
    if (y < top + ((bottom - top) / 2)) {
      right++;
    } else {
      right--;
    }
    for (int x = left; x < right; x++) {
      byte color = 0xFF;
      if (y == top || y == bottom - 1)color = 0;
      if (x == left || x == right - 1)color = 0;
      buffer[(((y * 96) + x) * 2)] = color;
      buffer[(((y * 96) + x) * 2) + 1] = color;
    }
  }
}

void playVideo() {
  vidFile.rewind();
  display.goTo(0, 0);
  display.startData();
  memset((uint8_t *)audioBuffer, 0, 1024 * 2);
  tcStartCounter();
  long videoSize = vidFile.fileSize();
  int videoFrames = (long)(videoSize / ((96 * 64 * 2) + (1024 * 2)));
  int videoSeconds = videoFrames / 30;
  int currentFrame = (long)(vidFile.curPosition() / ((96 * 64 * 2) + (1024 * 2)));
  int currentSeconds = currentFrame / 30;
  int displayMinutes = videoSeconds / 60;
  int displaySeconds = videoSeconds % 60;
  char totalTimeString[] = "00:00";
  totalTimeString[0] = displayMinutes / 10 + '0';
  totalTimeString[1] = displayMinutes % 10 + '0';
  totalTimeString[3] = displaySeconds / 10 + '0';
  totalTimeString[4] = displaySeconds % 10 + '0';
  int currentBarVisible = 0;
  int showBar = 50;
  int paused = false;
  int showPlaying = 0;
  int debounceX = 10;
  int debounceY = 10;
  int debounce1 = 10;
  int debounce2 = 10;
  int getSeekFrame = false;
  while (vidFile.available()) {
    if (!paused || getSeekFrame) {
      delay(5);
      vidFile.read(buffer, 96 * 64 * 2);
      if (!paused)while (sampleIndex < 1000);
      vidFile.read((uint8_t*)audioBuffer, 1024 * 2);
      getSeekFrame = false;
    }
    if (!checkJoystick(TAJoystickLeft) && !checkJoystick(TAJoystickRight)) {
      if (debounceX) debounceX--;
    }
    if (!checkButton(TAButton1)) {
      if (debounce1) debounce1--;
    }
    if (!checkButton(TAButton2)) {
      if (debounce2) debounce2--;
    }
    if (checkJoystick(TAJoystickRight)) {
      int frames = 30;
      vidFile.seekSet(vidFile.curPosition() + (frames * ((96 * 64 * 2) + (1024 * 2))));
      showBar = 50;
      getSeekFrame = true;
    }
    if (checkJoystick(TAJoystickLeft)) {
      int frames = 30;
      vidFile.seekSet(vidFile.curPosition() - (frames * ((96 * 64 * 2) + (1024 * 2))));
      showBar = 50;
      getSeekFrame = true;
    }
    if (!getSeekFrame && !debounce1 && checkButton(TAButton1)) {
      if (paused) {
        tcStartCounter();
        paused = false;
        showBar = 50;
        showPlaying = 25;
      } else {
        tcDisable();
        paused = true;
        showBar = 50;
      }
      debounce1 = 10;
    }
    if (!debounce2 && checkButton(TAButton2)) {
      break;
    }
    if (showBar) {
      currentFrame = (long)(vidFile.curPosition() / ((96 * 64 * 2) + (1024 * 2)));
      currentSeconds = currentFrame / 30;
      displayMinutes = currentSeconds / 60;
      displaySeconds = currentSeconds % 60;
      char currentTimeString[] = "00:00";
      currentTimeString[0] = displayMinutes / 10 + '0';
      currentTimeString[1] = displayMinutes % 10 + '0';
      currentTimeString[3] = displaySeconds / 10 + '0';
      currentTimeString[4] = displaySeconds % 10 + '0';
      int timeStringWidth = display.getPrintWidth(totalTimeString);
      if (currentBarVisible < 10)currentBarVisible++;
      if (currentBarVisible > showBar)currentBarVisible = showBar;
      int yOffset = 63 - currentBarVisible;
      //if (yOffset < 53)yOffset = 53;
      for (int y = yOffset + 0; y < yOffset + 12 && y < 64; y++) {
        for (int x = 0; x < 96; x++) {
          ((uint16_t *)buffer)[(y * 96) + x] = (((uint16_t *)buffer)[(y * 96) + x] >> 2) & 0xE739; //0xEF7B
        }
      }
      for (int y = yOffset; y < yOffset + 15; y++) {
        if (y < 64)
          putString(y, 1, yOffset + 1, currentTimeString, buffer + (96 * 2 * y), thinPixel7_10ptFontInfo);
      }
      for (int y = yOffset; y < yOffset + 15; y++) {
        if (y < 64)
          putString(y, 95 - timeStringWidth, yOffset + 1, totalTimeString, buffer + (96 * 2 * y), thinPixel7_10ptFontInfo);
      }
      int barTotal = 93 - timeStringWidth - (timeStringWidth + 2);
      int progressBar = (long)((currentFrame * barTotal) / videoFrames);
      for (int y = yOffset + 4; y < yOffset + 8 && y < 64; y++) {
        for (int x = timeStringWidth + 2; x < 93 - timeStringWidth; x++) {
          if (x - timeStringWidth - 2 <= progressBar) {
            ((uint16_t *)buffer)[(y * 96) + x] =  0xEF7B;
          } else {
            ((uint16_t *)buffer)[(y * 96) + x] =  0xE739;
          }
        }
      }
      if (!paused) {
        showBar--;
      }
    }
    if (paused) {
      showPause();
      delay(30);
    } else {
      if (showPlaying) {
        if (showPlaying != 25)
          showPlay();
        showPlaying--;
      }
    }
    display.writeBufferDMA(buffer, 96 * 64 * 2);
  }
  tcDisable();
  memset((uint8_t *)audioBuffer, 0, 1024 * 2);
  sampleIndex = 0;
  display.endTransfer();
  vidFile.rewind();
}





void tcConfigure(uint32_t sampleRate)
{
  // Enable GCLK for TCC2 and TC5 (timer counter input clock)
  GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5)) ;
  while (GCLK->STATUS.bit.SYNCBUSY);

  tcReset();

  // Set Timer counter Mode to 16 bits
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;

  // Set TC5 mode as match frequency
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1;

  TC5->COUNT16.CC[0].reg = (uint16_t) (SystemCoreClock / sampleRate - 1);
  while (tcIsSyncing());

  // Configure interrupt request
  NVIC_DisableIRQ(TC5_IRQn);
  NVIC_ClearPendingIRQ(TC5_IRQn);
  NVIC_SetPriority(TC5_IRQn, 0);
  NVIC_EnableIRQ(TC5_IRQn);

  // Enable the TC5 interrupt request
  TC5->COUNT16.INTENSET.bit.MC0 = 1;
  while (tcIsSyncing());
}


bool tcIsSyncing()
{
  return TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY;
}

void tcStartCounter()
{
  // Enable TC

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
  while (tcIsSyncing());
}

void tcReset()
{
  // Reset TCx
  TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  while (tcIsSyncing());
  while (TC5->COUNT16.CTRLA.bit.SWRST);
}

void tcDisable()
{
  // Disable TC5
  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (tcIsSyncing());
}


#ifdef __cplusplus
extern "C" {
#endif

extern void Audio_Handler (void);

void Audio_Handler (void)
{
  while (DAC->STATUS.bit.SYNCBUSY == 1);
  DAC->DATA.reg =  audioBuffer[sampleIndex++];
  while (DAC->STATUS.bit.SYNCBUSY == 1);

  if (sampleIndex > 1023 ) sampleIndex = 0;

  // Clear the interrupt
  TC5->COUNT16.INTFLAG.bit.MC0 = 1;
}

void TC5_Handler (void) __attribute__ ((weak, alias("Audio_Handler")));

#ifdef __cplusplus
}
#endif



