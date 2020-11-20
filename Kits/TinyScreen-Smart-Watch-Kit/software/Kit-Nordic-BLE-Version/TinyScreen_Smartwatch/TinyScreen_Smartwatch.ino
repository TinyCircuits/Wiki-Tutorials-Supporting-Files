//-------------------------------------------------------------------------------
//  TinyCircuits TinyScreen/NRF8001 Smartwatch Example Sketch
//  Last Updated 29 February 2016
//
//  This demo sets up the NRF8001 for Nordic's BLE virtual UART connection, then
//  accepts a date setting string with the format "Dyyyy MM dd k m s" or a
//  notification string starting with a 1 or 2(for line 1 or 2) followed by the
//  short text string to display. Time, date, notifications are displayed, and a
//  simple sleep mode is implemented to save some power.
//
//  Written by Ben Rose, TinyCircuits http://Tiny-Circuits.com
//
//-------------------------------------------------------------------------------


#include <Wire.h>
#include <SPI.h>
#include <TinyScreen.h>
#include "lib_aci.h"
#include <EEPROM.h>
#include <TimeLib.h>

#define BLE_DEBUG false
uint8_t ble_rx_buffer[21];
uint8_t ble_rx_buffer_len = 0;
uint8_t ble_connection_state = false;
uint8_t ble_connection_displayed_state = true;

//when using this project in the Arduino IDE, set 'codebender' false and rename UART.h to UART.ino
#define codebender false

#if codebender
#include "UART.h"
#endif

#define menu_debug_print false

TinyScreen display = TinyScreen(0);

#define  BLACK           0x00
#define BLUE            0xE0
#define RED             0x03
#define GREEN           0x1C
#define WHITE           0xFF
#define GREY            0x6D


uint8_t defaultFontColor = WHITE;
uint8_t defaultFontBG = BLACK;
uint8_t inactiveFontColor = GREY;
uint8_t inactiveFontBG = BLACK;

uint8_t topBarHeight = 10;
uint8_t timeY = 14;
uint8_t menuTextY[4] = {12, 25, 38, 51};

unsigned long lastReceivedTime = 0;

unsigned long batteryUpdateInterval = 10000;
unsigned long lastBatteryUpdate = 0;

unsigned long sleepTimer = 0;
int sleepTimeout = 5000;

uint8_t rewriteTime = true;
uint8_t lastAMPMDisplayed = 0;
uint8_t lastHourDisplayed = -1;
uint8_t lastMinuteDisplayed = -1;
uint8_t lastSecondDisplayed = -1;
uint8_t lastDisplayedDay = -1;

uint8_t displayOn = 0;
uint8_t buttonReleased = 1;
uint8_t rewriteMenu = false;
uint8_t amtNotifications = 0;
uint8_t lastAmtNotificationsShown = -1;
char notificationLine1[20] = "";
char notificationLine2[20] = "";
unsigned long mainDisplayUpdateInterval = 300;
unsigned long lastMainDisplayUpdate = 0;

int brightness = 3;
uint8_t lastSetBrightness = 100;

const FONT_INFO& font8pt = liberationSansNarrow_8ptFontInfo;
const FONT_INFO& font10pt = liberationSansNarrow_10ptFontInfo;
const FONT_INFO& font22pt = liberationSansNarrow_22ptFontInfo;


void setup(void)
{
  setTime(0, 0, 0, 16, 2, 2016);
  Serial.begin(115200);
  Wire.begin();
  display.begin();
  display.setFlip(true);
  initHomeScreen();
  requestScreenOn();
  BLEsetup();
  while (millis() < 1000)aci_loop();
}

void loop() {
  aci_loop();//Process any ACI commands or events from the NRF8001- main BLE handler, must run often. Keep main loop short.
  if (ble_rx_buffer_len) {
    if (ble_rx_buffer[0] == 'D') {
      //expect date/time string- example: D2015 03 05 11 48 42
      lastReceivedTime = millis();
      updateTime(ble_rx_buffer + 1);
      requestScreenOn();
    }
    if (ble_rx_buffer[0] == '1') {
      memcpy(notificationLine1, ble_rx_buffer + 1, ble_rx_buffer_len - 1);
      notificationLine1[ble_rx_buffer_len - 1] = '\0';
      amtNotifications = 1;
      requestScreenOn();
    }
    if (ble_rx_buffer[0] == '2') {
      memcpy(notificationLine2, ble_rx_buffer + 1, ble_rx_buffer_len - 1);
      notificationLine2[ble_rx_buffer_len - 1] = '\0';
      amtNotifications = 1;
      requestScreenOn();
    }
    ble_rx_buffer_len = 0;
  }
  if (displayOn && (millis() > mainDisplayUpdateInterval + lastMainDisplayUpdate)) {
    updateMainDisplay();
  }
  checkButtons();
  if (millis() > sleepTimer + (unsigned long)sleepTimeout) {
    if (displayOn) {
      displayOn = 0;
      display.off();
    }
  }
}

void updateTime(uint8_t * b) {
  int y, M, d, k, m, s;
  char * next;
  y = strtol((char *)b, &next, 10);
  M = strtol(next, &next, 10);
  d = strtol(next, &next, 10);
  k = strtol(next, &next, 10);
  m = strtol(next, &next, 10);
  s = strtol(next, &next, 10);
  setTime(k, m, s, d, M, y);
}

int requestScreenOn() {
  sleepTimer = millis();
  if (!displayOn) {
    displayOn = 1;
    updateMainDisplay();
    display.on();
    return 1;
  }
  return 0;
}

void checkButtons() {
  byte buttons = display.getButtons();
  if (buttonReleased && buttons) {
    if (displayOn)
      buttonPress(buttons);
    requestScreenOn();
    buttonReleased = 0;
  }
  if (!buttonReleased && !(buttons & 0x0F)) {
    buttonReleased = 1;
  }
}

const uint8_t displayStateHome = 0x01;
const uint8_t displayStateMenu = 0x02;
const uint8_t displayStateEditor = 0x03;

uint8_t currentDisplayState = displayStateHome;


void (*menuHandler)(uint8_t) = NULL;

#if codebender
uint8_t (*editorHandler)(uint8_t, int*, PGM_P, void (*)()) = NULL;
#else
uint8_t (*editorHandler)(uint8_t, int*, char*, void (*)()) = NULL;
#endif


typedef struct
{
  const uint8_t amtLines;
  const char* const * strings;
  void (*selectionHandler)(uint8_t);
} menu_info;

#if codebender
const uint8_t upButton = 0x01;
const uint8_t downButton = 0x02;
const uint8_t selectButton = 0x04;
const uint8_t backButton = 0x08;
const uint8_t menuButton = 0x04;
const uint8_t viewButton = 0x02;
const uint8_t clearButton = 0x02;
#else
const uint8_t upButton = TSButtonUpperRight;
const uint8_t downButton = TSButtonLowerRight;
const uint8_t selectButton = TSButtonLowerLeft;
const uint8_t backButton = TSButtonUpperLeft;
const uint8_t menuButton = TSButtonLowerLeft;
const uint8_t viewButton = TSButtonLowerRight;
const uint8_t clearButton = TSButtonLowerRight;
#endif

void buttonPress(uint8_t buttons) {
  if (currentDisplayState == displayStateHome) {
    if (buttons == viewButton) {
      menuHandler = viewNotifications;
      menuHandler(0);
    } else if (buttons == menuButton) {
      menuHandler = viewMenu;
      menuHandler(0);
    }
  } else if (currentDisplayState == displayStateMenu) {
    if (menuHandler) {
      menuHandler(buttons);
    }
  } else if (currentDisplayState == displayStateEditor) {
    if (editorHandler) {
      editorHandler(buttons, 0, 0, NULL);
    }
  }
}

void viewNotifications(uint8_t button) {
  if (!button) {
    if (menu_debug_print)Serial.println("viewNotificationsInit");
    currentDisplayState = displayStateMenu;
    display.clearWindow(0, 12, 96, 64);
    display.setFont(font10pt);
    display.fontColor(defaultFontColor, defaultFontBG);
    if (amtNotifications) {
      if (menu_debug_print)Serial.println("amtNotifications=true");
      display.setCursor(0, menuTextY[0]);
      display.print(notificationLine1);
      display.setCursor(0, menuTextY[1]);
      display.print(notificationLine2);
      display.setCursor(56, menuTextY[3]);
      display.print("clear >");
    } else {
      if (menu_debug_print)Serial.println("amtNotifications=false");
      display.setCursor(0, menuTextY[0]);
      display.print(F(" No notifications.   "));
      display.setCursor(59, menuTextY[3]);
      display.print(F("back >"));
    }
  } else {
    if (button == clearButton) {
      //currentNotification=NULL;
      amtNotifications = 0;
      currentDisplayState = displayStateHome;
      initHomeScreen();
    }
  }
}

uint8_t menuHistory[5];
uint8_t menuHistoryIndex = 0;
uint8_t currentMenu = 0;
uint8_t currentMenuLine = 0;
uint8_t lastMenuLine = -1;
uint8_t currentSelectionLine = 0;
uint8_t lastSelectionLine = -1;


void newMenu(char newIndex) {
  currentMenuLine = 0;
  lastMenuLine = -1;
  currentSelectionLine = 0;
  lastSelectionLine = -1;
  if (newIndex >= 0) {
    menuHistory[menuHistoryIndex++] = currentMenu;
    currentMenu = newIndex;
  } else {
    if (currentDisplayState == displayStateMenu) {
      menuHistoryIndex--;
      currentMenu = menuHistory[menuHistoryIndex];
    }
  }
  if (menuHistoryIndex) {
    currentDisplayState = displayStateMenu;
    if (menu_debug_print)Serial.print("New menu index ");
    if (menu_debug_print)Serial.println(currentMenu);
  } else {
    if (menu_debug_print)Serial.print("New menu index ");
    if (menu_debug_print)Serial.println("home");
    currentDisplayState = displayStateHome;
    initHomeScreen();
  }
}

static const char PROGMEM mainMenuStrings0[] = "Set date/time";
static const char PROGMEM mainMenuStrings1[] = "Set auto off";
static const char PROGMEM mainMenuStrings2[] = "Set brightness";

static const char* const PROGMEM mainMenuStrings[] =
{
  mainMenuStrings0,
  mainMenuStrings1,
  mainMenuStrings2,
};

const menu_info mainMenuInfo =
{
  3,
  mainMenuStrings,
  mainMenu,
};


static const char PROGMEM dateTimeMenuStrings0[] = "Set Year";
static const char PROGMEM dateTimeMenuStrings1[] = "Set Month";
static const char PROGMEM dateTimeMenuStrings2[] = "Set Day";
static const char PROGMEM dateTimeMenuStrings3[] = "Set Hour";
static const char PROGMEM dateTimeMenuStrings4[] = "Set Minute";
static const char PROGMEM dateTimeMenuStrings5[] = "Set Second";

static const char* const PROGMEM dateTimeMenuStrings[] =
{
  dateTimeMenuStrings0,
  dateTimeMenuStrings1,
  dateTimeMenuStrings2,
  dateTimeMenuStrings3,
  dateTimeMenuStrings4,
  dateTimeMenuStrings5,
};

const menu_info dateTimeMenuInfo =
{
  6,
  dateTimeMenuStrings,
  dateTimeMenu,
};

const menu_info menuList[] = {mainMenuInfo, dateTimeMenuInfo};
#define mainMenuIndex 0
#define dateTimeMenuIndex 1

int currentVal = 0;
int digits[4];
int currentDigit = 0;
int maxDigit = 4;
int *originalVal;
void (*editIntCallBack)() = NULL;

#if codebender
uint8_t editInt(uint8_t button, int *inVal, PGM_P intName, void (*cb)()) {
#else
uint8_t editInt(uint8_t button, int *inVal, char *intName, void (*cb)()) {
#endif
  if (menu_debug_print)Serial.println("editInt");
  if (!button) {
    if (menu_debug_print)Serial.println("editIntInit");
    editIntCallBack = cb;
    currentDisplayState = displayStateEditor;
    editorHandler = editInt;
    currentDigit = 0;
    originalVal = inVal;
    currentVal = *originalVal;
    digits[3] = currentVal % 10; currentVal /= 10;
    digits[2] = currentVal % 10; currentVal /= 10;
    digits[1] = currentVal % 10; currentVal /= 10;
    digits[0] = currentVal % 10;
    currentVal = *originalVal;
    display.clearWindow(0, 12, 96, 64);
    display.setFont(font10pt);
    display.fontColor(defaultFontColor, defaultFontBG);
    display.setCursor(0, menuTextY[0]);
    display.print("< back/undo");
    display.setCursor(90, menuTextY[0]);
    display.print('^');
    display.setCursor(10, menuTextY[1]);
#if codebender
    PGM_P str = intName;
    for (uint8_t c; (c = pgm_read_byte(str)); str++) display.write(c);
#else
    display.print(intName);
#endif
    display.setCursor(0, menuTextY[3]);
    display.print("< next/save");
    display.setCursor(90, menuTextY[3]);
    display.print('v');
  } else if (button == upButton) {
    if (digits[currentDigit] < 9)
      digits[currentDigit]++;
  } else if (button == downButton) {
    if (digits[currentDigit] > 0)
      digits[currentDigit]--;
  } else if (button == selectButton) {
    if (currentDigit < maxDigit - 1) {
      currentDigit++;
    } else {
      //save
      int newValue = (digits[3]) + (digits[2] * 10) + (digits[1] * 100) + (digits[0] * 1000);
      *originalVal = newValue;
      viewMenu(backButton);
      if (editIntCallBack) {
        editIntCallBack();
        editIntCallBack = NULL;
      }
      return 1;
    }
  } else if (button == backButton) {
    if (currentDigit > 0) {
      currentDigit--;
    } else {
      if (menu_debug_print)Serial.println("back");
      viewMenu(backButton);
      return 0;
    }
  }
  display.setCursor(10, menuTextY[2]);
  for (uint8_t i = 0; i < 4; i++) {
    if (i != currentDigit)display.fontColor(inactiveFontColor, defaultFontBG);
    display.print(digits[i]);
    if (i != currentDigit)display.fontColor(defaultFontColor, defaultFontBG);
  }
  display.print("   ");
  return 0;
}

void mainMenu(uint8_t selection) {
  if (menu_debug_print)Serial.println("mainMenuHandler");
  if (selection == 0) {
    newMenu(dateTimeMenuIndex);
  }
  if (selection == 1) {
#if codebender
    editInt(0, &sleepTimeout, menuList[currentMenu].strings[selection], NULL);
#else
    char buffer[20];
    strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[mainMenuIndex].strings[selection])));
    editInt(0, &sleepTimeout, buffer, NULL);
#endif
  }
  if (selection == 2) {
#if codebender
    editInt(0, &brightness, menuList[currentMenu].strings[selection], NULL);
#else
    char buffer[20];
    strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[mainMenuIndex].strings[selection])));
    editInt(0, &brightness, buffer, NULL);
#endif
  }
}


uint8_t dateTimeSelection = 0;
int dateTimeVariable = 0;

void saveChangeCallback() {
  int timeData[] = {year(), month(), day(), hour(), minute(), second()};
  timeData[dateTimeSelection] = dateTimeVariable;
  setTime(timeData[3], timeData[4], timeData[5], timeData[2], timeData[1], timeData[0]);
  if (menu_debug_print)Serial.print("set time ");
  if (menu_debug_print)Serial.println(dateTimeVariable);
}


void dateTimeMenu(uint8_t selection) {
  if (menu_debug_print)Serial.print("dateTimeMenu ");
  if (menu_debug_print)Serial.println(selection);
  if (selection >= 0 && selection < 6) {
    int timeData[] = {year(), month(), day(), hour(), minute(), second()};
    dateTimeVariable = timeData[selection];
    dateTimeSelection = selection;
#if codebender
    editInt(0, &dateTimeVariable, menuList[currentMenu].strings[selection], saveChangeCallback);
#else
    char buffer[20];
    strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[dateTimeMenuIndex].strings[selection])));
    editInt(0, &dateTimeVariable, buffer, saveChangeCallback);
#endif
  }
}

void viewMenu(uint8_t button) {
  if (menu_debug_print)Serial.print("viewMenu ");
  if (menu_debug_print)Serial.println(button);
  if (!button) {
    newMenu(mainMenuIndex);
    display.clearWindow(0, 12, 96, 64);
  } else {
    if (button == upButton) {
      if (currentSelectionLine > 0) {
        currentSelectionLine--;
      } else if (currentMenuLine > 0) {
        currentMenuLine--;
      }
    } else if (button == downButton) {
      if (currentSelectionLine < menuList[currentMenu].amtLines - 1 && currentSelectionLine < 3) {
        currentSelectionLine++;
      } else if (currentSelectionLine + currentMenuLine < menuList[currentMenu].amtLines - 1) {
        currentMenuLine++;
      }
    } else if (button == selectButton) {
      if (menu_debug_print)Serial.print("select ");
      if (menu_debug_print)Serial.println(currentMenuLine + currentSelectionLine);
      menuList[currentMenu].selectionHandler(currentMenuLine + currentSelectionLine);
    } else if (button == backButton) {
      newMenu(-1);
      if (!menuHistoryIndex)
        return;
    }
  }
  display.setFont(font10pt);
  if (lastMenuLine != currentMenuLine || lastSelectionLine != currentSelectionLine) {
    if (menu_debug_print)Serial.println("drawing menu ");
    if (menu_debug_print)Serial.println(currentMenu);
    for (int i = 0; i < 4; i++) {
      display.setCursor(7, menuTextY[i]);
      if (i == currentSelectionLine) {
        display.fontColor(defaultFontColor, inactiveFontBG);
      } else {
        display.fontColor(inactiveFontColor, inactiveFontBG);
      }
      if (currentMenuLine + i < menuList[currentMenu].amtLines) {
#if codebender
        PGM_P str = menuList[currentMenu].strings[currentMenuLine + i];
        for (uint8_t c; (c = pgm_read_byte(str)); str++) display.write(c);
#else
        char buffer[20];
        strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[currentMenu].strings[currentMenuLine + i])));
        display.print(buffer);
#endif
      }
      for (uint8_t i = 0; i < 25; i++)display.write(' ');
      if (i == 0) {
        display.fontColor(defaultFontColor, inactiveFontBG);
        display.setCursor(0, menuTextY[0]);
        display.print('<');
        display.setCursor(90, menuTextY[0]);
        display.print('^');
      }
      if (i == 3) {
        display.fontColor(defaultFontColor, inactiveFontBG);
        display.setCursor(0, menuTextY[3]);
        display.print('>');
        display.setCursor(90, menuTextY[3]);
        display.print('v');
      }
    }
    lastMenuLine = currentMenuLine;
    lastSelectionLine = currentSelectionLine;
  }
}


void initHomeScreen() {
  display.clearWindow(0, 12, 96, 64);
  rewriteTime = true;
  rewriteMenu = true;
  updateMainDisplay();
}

void updateDateDisplay() {
  if (lastDisplayedDay == day())
    return;
  lastDisplayedDay = day();
  display.setFont(font8pt);
  display.fontColor(defaultFontColor, defaultFontBG);
  display.setCursor(2, 1);
  display.print(dayShortStr(weekday()));
  display.print(' ');
  display.print(month());
  display.print('/');
  display.print(day());
  display.print("  ");
}

void updateMainDisplay() {
  if (lastSetBrightness != brightness) {
    display.setBrightness(brightness);
    lastSetBrightness = brightness;
  }
  updateDateDisplay();
  updateBLEstatusDisplay();
  displayBattery();
  if (currentDisplayState == displayStateHome) {
    updateTimeDisplay();
    if (rewriteMenu || lastAmtNotificationsShown != amtNotifications) {
      lastAmtNotificationsShown = amtNotifications;
      display.setFont(font10pt);
      display.setCursor(0, menuTextY[2]);
      if (!amtNotifications) {
        display.print(F(" No notifications.  "));
      } else {
        display.print(F("  "));
        display.print(amtNotifications);
        display.print(F(" notification"));
        if (amtNotifications > 1)
          display.write('s');
        display.print(".   ");
      }
      display.setCursor(0, menuTextY[3]);
      display.print(F("< Menu    View >"));
      rewriteMenu = false;
    }
  }
  lastMainDisplayUpdate = millis();
}


void updateTimeDisplay() {
  if (currentDisplayState != displayStateHome)
    return;
  char displayX;
  int hour12 = hour();
  int AMPM = 1;
  if (hour12 > 12) {
    AMPM = 2;
    hour12 -= 12;
  }
  display.fontColor(defaultFontColor, defaultFontBG);
  if (rewriteTime || lastHourDisplayed != hour12) {
    display.setFont(font22pt);
    lastHourDisplayed = hour12;
    displayX = 0;
    display.setCursor(displayX, timeY);
    if (lastHourDisplayed < 10)display.print('0');
    display.print(lastHourDisplayed);
    display.write(':');
    if (lastAMPMDisplayed != AMPM) {
      if (AMPM == 2)
        display.fontColor(inactiveFontColor, inactiveFontBG);
      display.setFont(font10pt);
      display.setCursor(displayX + 79, timeY - 1);
      display.print("AM");
      if (AMPM == 2) {
        display.fontColor(defaultFontColor, defaultFontBG);
      } else {
        display.fontColor(inactiveFontColor, inactiveFontBG);
      }
      display.setCursor(displayX + 79, timeY + 11);
      display.print("PM");
      display.fontColor(defaultFontColor, defaultFontBG);
    }
  }

  if (rewriteTime || lastMinuteDisplayed != minute()) {
    display.setFont(font22pt);
    lastMinuteDisplayed = minute();
    displayX = 14 + 14 - 1;
    display.setCursor(displayX, timeY);
    if (lastMinuteDisplayed < 10)display.print('0');
    display.print(lastMinuteDisplayed);
    display.write(':');
  }

  if (rewriteTime || lastSecondDisplayed != second()) {
    display.setFont(font22pt);
    lastSecondDisplayed = second();
    displayX = 14 + 14 + 14 + 14 - 2;
    display.setCursor(displayX, timeY);
    if (lastSecondDisplayed < 10)display.print('0');
    display.print(lastSecondDisplayed);
  }
  rewriteTime = false;
}

void updateBLEstatusDisplay() {
  if (ble_connection_state == ble_connection_displayed_state)
    return;
  ble_connection_displayed_state = ble_connection_state;
  int x = 62;
  int y = 6;
  int s = 2;
  uint8_t color = 0x03;
  if (ble_connection_state)
    color = 0xE0;
  display.drawLine(x, y + s + s, x, y - s - s, color);
  display.drawLine(x - s, y + s, x + s, y - s, color);
  display.drawLine(x + s, y + s, x - s, y - s, color);
  display.drawLine(x, y + s + s, x + s, y + s, color);
  display.drawLine(x, y - s - s, x + s, y - s, color);
}

void displayBattery() {
  int result = 0;
#if defined (ARDUINO_ARCH_AVR)
  //http://forum.arduino.cc/index.php?topic=133907.0
  const long InternalReferenceVoltage = 1100L;
  ADMUX = (0 << REFS1) | (1 << REFS0) | (0 << ADLAR) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0);
  delay(10);
  ADCSRA |= _BV( ADSC );
  while ( ( (ADCSRA & (1 << ADSC)) != 0 ) );
  result = (((InternalReferenceVoltage * 1024L) / ADC) + 5L) / 10L;
  //SerialMonitorInterface.println(result);
  //if(result>440){//probably charging
  uint8_t charging = false;
  if (result > 450) {
    charging = true;
  }
  result = constrain(result - 300, 0, 120);
  uint8_t x = 70;
  uint8_t y = 3;
  uint8_t height = 5;
  uint8_t length = 20;
  uint8_t amtActive = (result * length) / 120;
  uint8_t red, green, blue;
  display.drawLine(x - 1, y, x - 1, y + height, 0xFF); //left boarder
  display.drawLine(x - 1, y - 1, x + length, y - 1, 0xFF); //top border
  display.drawLine(x - 1, y + height + 1, x + length, y + height + 1, 0xFF); //bottom border
  display.drawLine(x + length, y - 1, x + length, y + height + 1, 0xFF); //right border
  display.drawLine(x + length + 1, y + 2, x + length + 1, y + height - 2, 0xFF); //right border
  for (uint8_t i = 0; i < length; i++) {
    if (i < amtActive) {
      red = 63 - ((63 / length) * i);
      green = ((63 / length) * i);
      blue = 0;
    } else {
      red = 32;
      green = 32;
      blue = 32;
    }
    display.drawLine(x + i, y, x + i, y + height, red, green, blue);
  }
#elif defined(ARDUINO_ARCH_SAMD)
  //http://atmel.force.com/support/articles/en_US/FAQ/ADC-example
  SYSCTRL->VREF.reg |= SYSCTRL_VREF_BGOUTEN;while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->SAMPCTRL.bit.SAMPLEN = 0x1;
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->INPUTCTRL.bit.MUXPOS = 0x19;         // Internal bandgap input
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->CTRLA.bit.ENABLE = 0x01;             // Enable ADC
  // Start conversion
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->SWTRIG.bit.START = 1;
  // Clear the Data Ready flag
  ADC->INTFLAG.bit.RESRDY = 1;
  // Start conversion again, since The first conversion after the reference is changed must not be used.
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->SWTRIG.bit.START = 1;
  // Store the value
  while ( ADC->INTFLAG.bit.RESRDY == 0 );   // Waiting for conversion to complete
  uint32_t valueRead = ADC->RESULT.reg;
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->CTRLA.bit.ENABLE = 0x00;             // Disable ADC
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  SYSCTRL->VREF.reg &= ~SYSCTRL_VREF_BGOUTEN;
  result = (((1100L * 1024L) / valueRead) + 5L) / 10L;
  uint8_t x = 70;
  uint8_t y = 3;
  uint8_t height = 5;
  uint8_t length = 20;
  uint8_t red, green;
  if (result > 325) {
    red = 0;
    green = 63;
  } else {
    red = 63;
    green = 0;
  }
  display.drawLine(x - 1, y, x - 1, y + height, 0xFF); //left boarder
  display.drawLine(x - 1, y - 1, x + length, y - 1, 0xFF); //top border
  display.drawLine(x - 1, y + height + 1, x + length, y + height + 1, 0xFF); //bottom border
  display.drawLine(x + length, y - 1, x + length, y + height + 1, 0xFF); //right border
  display.drawLine(x + length + 1, y + 2, x + length + 1, y + height - 2, 0xFF); //right border
  for (uint8_t i = 0; i < length; i++) {
    display.drawLine(x + i, y, x + i, y + height, red, green, 0);
  }
#endif
}
