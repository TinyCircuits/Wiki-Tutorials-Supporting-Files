/*  This is the main file for the menu scroll program. This file includes the code
 *  for the main screen that appears when you first upload the program. 
 *  
 *  Written by: Ben Rose and Laveréna Wienclaw
 *  Last update: May 2021
 */

#include <Wire.h>
#include <TinyScreen.h>
#include <GraphicsBuffer.h>
#include <TimeLib.h>

GraphicsBuffer displayBuffer = GraphicsBuffer(96, 64, colorDepth16BPP);

TinyScreen display = TinyScreen(TinyScreenPlus); // TinyScreen+
//TinyScreen display = TinyScreen(TinyScreenDefault); // TinyScreen TinyShield

uint8_t menuTextY[8] = {1 * 8 - 1, 2 * 8 - 1, 3 * 8 - 1, 4 * 8 - 1, 5 * 8 - 1, 6 * 8 - 1, 7 * 8 - 3, 8 * 8 - 3};

// You can change buttons here, but you will need to change the arrow graphics elsewhere in the program to
// match what you change here 
const uint8_t upButton     = TSButtonUpperRight;
const uint8_t downButton   = TSButtonLowerRight;
const uint8_t viewButton   = TSButtonLowerRight;
const uint8_t clearButton  = TSButtonLowerRight;
const uint8_t backButton   = TSButtonUpperLeft;
const uint8_t selectButton = TSButtonLowerLeft;
const uint8_t menuButton   = TSButtonLowerLeft;

unsigned long sleepTimer = 0;
int sleepTimeout = 0;

volatile uint32_t counter = 0;

uint32_t lastPrint = 0;

uint8_t rewriteTime = true;
uint8_t lastAMPMDisplayed = 0;
uint8_t lastHourDisplayed = -1;
uint8_t lastMinuteDisplayed = -1;
uint8_t lastSecondDisplayed = -1;
uint8_t lastDisplayedDay = -1;

uint8_t displayOn = 0;
uint8_t buttonReleased = 1;
uint8_t rewriteMenu = false;
unsigned long mainDisplayUpdateInterval = 20;
unsigned long lastMainDisplayUpdate = 0;

int brightness = 8;
uint8_t lastSetBrightness = 7;

const uint8_t displayStateHome = 0x01;
const uint8_t displayStateMenu = 0x02;
const uint8_t displayStateEditor = 0x03;
const uint8_t displayStateCalibration = 0x04;

uint8_t currentDisplayState = displayStateHome;

// Make Serial Monitor compatible for all TinyCircuits processors
#if defined(ARDUINO_ARCH_AVR)
  #define SerialMonitorInterface Serial
#elif defined(ARDUINO_ARCH_SAMD)
  #define SerialMonitorInterface SerialUSB
#endif

void setup() {
  Wire.begin();
  SerialMonitorInterface.begin(200000);
  delay(100);
  
  display.begin();
  display.setFlip(true);
  display.setBitDepth(1);
  display.initDMA();  

  if (displayBuffer.begin()) {
    //memory allocation error- buffer too big
    while (1) {
      SerialMonitorInterface.println("Display buffer memory allocation error!");
      delay(1000);
    }
  }

  displayBuffer.setFont(thinPixel7_10ptFontInfo);

  displayBuffer.clear();
  
  initHomeScreen();
  requestScreenOn();
}

void loop() {
  displayOn = 1;
  
  if (displayOn && (millisOffset() > mainDisplayUpdateInterval + lastMainDisplayUpdate)) {
    lastMainDisplayUpdate = millis();
    display.endTransfer();
    display.goTo(0, 0);
    display.startData();
    display.writeBufferDMA(displayBuffer.getBuffer(), displayBuffer.getBufferSize());
  }

  if (currentDisplayState == displayStateHome) {
    if ( display.getButtons(TSButtonLowerRight) || display.getButtons(TSButtonUpperRight) || display.getButtons(TSButtonLowerLeft) || display.getButtons(TSButtonUpperLeft)){
      displayBuffer.clear();  // if a button is pressed, clear the screen
    }

    updateMainDisplay(); 
    liveDisplay(); // This is the main home screen display
    display.writeBufferDMA(displayBuffer.getBuffer(), displayBuffer.getBufferSize());
    delay (500); // Making this smaller will make the screen more scattered, making it longer will mean you need to hold in buttons longer
  } else {
    drawMenu();
  }

  checkButtons();
  updateMainDisplay();
}

uint32_t millisOffset() {
  return millis();
}

uint32_t getSecondsCounter() {
  return millis()/1000;
}

int requestScreenOn() {
  sleepTimer = millisOffset();
  if (!displayOn) {
    setTime(counter);
    displayOn = 1;
    updateMainDisplay();
    return 1;
  }
  return 0;
}

void checkButtons() {
  byte buttons = display.getButtons();
  if (buttonReleased && buttons) {
    if (displayOn) {
      buttonPress(buttons);
    }
    requestScreenOn();
    buttonReleased = 0;
  }
  if (!buttonReleased && !(buttons & 0x1F)) {
    buttonReleased = 1;
  }
}

void initHomeScreen() {
  displayBuffer.clearWindow(0, 1, 96, 5);
  rewriteTime = true;
  rewriteMenu = true;
  updateMainDisplay();
}

void updateMainDisplay() {
  if (lastSetBrightness != brightness) {
    display.setBrightness(brightness);
    lastSetBrightness = brightness;
  }
  updateDateTimeDisplay();
  if (currentDisplayState == displayStateHome) {
    //if (rewriteMenu /*|| lastAmtNotificationsShown != amtNotifications*/) {
    displayBuffer.setCursor(9, menuTextY[6]);
    displayBuffer.print("Menu");
    displayBuffer.setCursor(70, menuTextY[6]);
    displayBuffer.print("Log");
    
    leftArrow(0, 57);
    rightArrow(90, 57);
    

    rewriteMenu = false;
  }
}


void updateDateTimeDisplay() {
  displayBuffer.clearWindow(0, 0, 96, 8);
  int currentDay = day();
  lastDisplayedDay = currentDay;
  displayBuffer.setCursor(0, -1);
  displayBuffer.print(dayShortStr(weekday()));
  displayBuffer.print(' ');
  displayBuffer.print(month());
  displayBuffer.print('/');
  displayBuffer.print(day());
  displayBuffer.print("  ");
  lastHourDisplayed = hour();
  lastMinuteDisplayed = minute();
  lastSecondDisplayed = second();

  int hour12 = lastHourDisplayed;
  int AMPM = 1;
  if (hour12 > 12) {
    AMPM = 2;
    hour12 -= 12;
  }
  lastHourDisplayed = hour12;
  displayBuffer.setCursor(58, -1);
  if (lastHourDisplayed < 10)displayBuffer.print(' ');
  displayBuffer.print(lastHourDisplayed);
  displayBuffer.write(':');
  if (lastMinuteDisplayed < 10)displayBuffer.print('0');
  displayBuffer.print(lastMinuteDisplayed);
  displayBuffer.setCursor(80, -1);
  if (AMPM == 2) {
    displayBuffer.print(" PM");
  } else {
    displayBuffer.print(" AM");
  }
  rewriteTime = false;
}

void liveDisplay() {
  displayBuffer.setCursor(0, menuTextY[0]);
  displayBuffer.print("Steps: 100");

  displayBuffer.setCursor(0, menuTextY[1]);
  displayBuffer.print("Temp: 70F");

  displayBuffer.setCursor(60, menuTextY[0]);
  displayBuffer.print("BPM: 75");

  displayBuffer.setCursor(0, menuTextY[4]);
  displayBuffer.print("Log: Off");
}
