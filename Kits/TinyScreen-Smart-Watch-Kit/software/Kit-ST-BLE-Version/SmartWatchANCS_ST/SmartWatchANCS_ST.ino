//-------------------------------------------------------------------------------
//  TinyCircuits TinyScreen/ST BLE ANCS Smartwatch Example
//  Last Updated 26 October 2017
//
//  This demo sets up the ST BLE for the Apple Notification Center Service as well
//  as the Current Time service. Now with TinyScreen+ compatibity.
//  Requires updated STBLE library or will not compile!
//
//  2.0.0 26 Oct 2017 Initial update release
//
//  Written by Ben Rose, TinyCircuits http://TinyCircuits.com
//
//-------------------------------------------------------------------------------


#include <SPI.h>
#include <TinyScreen.h>
#include <STBLE.h>
#include "BLEtypes.h"

#define BLE_DEBUG false
#define menu_debug_print false
uint32_t doVibrate = 0;

#if defined (ARDUINO_ARCH_AVR)
TinyScreen display = TinyScreen(TinyScreenDefault);
#define SerialMonitorInterface Serial
#include <TimeLib.h>

#elif defined(ARDUINO_ARCH_SAMD)
TinyScreen display = TinyScreen(TinyScreenPlus);
#define SerialMonitorInterface SerialUSB
#include <RTCZero.h>
#include <time.h>
RTCZero RTCZ;
uint32_t startTime = 0;
uint32_t sleepTime = 0;
unsigned long millisOffsetCount = 0;

void wakeHandler() {
  if (sleepTime) {
    millisOffsetCount += (RTCZ.getEpoch() - sleepTime);
    sleepTime = 0;
  }
}

void RTCwakeHandler() {
  //not used
}

void watchSleep() {
  if (doVibrate || ANCSRequestStayAwake())
    return;
  sleepTime = RTCZ.getEpoch();
  RTCZ.standbyMode();
}
#endif

BLEConn phoneConnection;
BLEServ timeService;
BLEServ ANCSService;
BLEChar currentTimeChar;
BLEChar NSchar;
BLEChar CPchar;
BLEChar DSchar;
int ANCSInitStep = -1;
unsigned long ANCSInitRetry = 0;

uint8_t ble_connection_state = false;
uint8_t ble_connection_displayed_state = true;
uint8_t TimeData[20];
uint32_t newtime = 0;


uint8_t defaultFontColor = TS_8b_White;
uint8_t defaultFontBG = TS_8b_Black;
uint8_t inactiveFontColor = TS_8b_Gray;
uint8_t inactiveFontBG = TS_8b_Black;

uint8_t topBarHeight = 10;
uint8_t timeY = 14;
uint8_t menuTextY[4] = {12, 25, 38, 51};

unsigned long lastReceivedTime = 0;

unsigned long batteryUpdateInterval = 10000;
unsigned long lastBatteryUpdate = 0;

unsigned long sleepTimer = 0;
int sleepTimeout = 5;

uint8_t rewriteTime = true;

uint8_t displayOn = 0;
uint8_t buttonReleased = 1;
uint8_t rewriteMenu = false;
uint8_t amtNotifications = 0;
uint8_t lastAmtNotificationsShown = -1;
unsigned long mainDisplayUpdateInterval = 300;
unsigned long lastMainDisplayUpdate = 0;

uint8_t vibratePin = 4;
uint8_t vibratePinActive = HIGH;
uint8_t vibratePinInactive = LOW;


int brightness = 3;
uint8_t lastSetBrightness = 100;

const FONT_INFO& font10pt = thinPixel7_10ptFontInfo;
const FONT_INFO& font22pt = liberationSansNarrow_22ptFontInfo;


void setup(void)
{
  display.begin();
  display.setFlip(true);
#if defined (ARDUINO_ARCH_AVR)
  for (int i = 0; i < 20; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  setTime(1, 1, 1, 12, 1, 2021);
#elif defined(ARDUINO_ARCH_SAMD)
  for (int i = 0; i < 20; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  pinMode(28, INPUT_PULLUP);
  pinMode(29, INPUT_PULLUP);
  pinMode(42, INPUT_PULLUP);
  pinMode(44, INPUT_PULLUP);
  pinMode(45, INPUT_PULLUP);
  RTCZ.begin();
  RTCZ.setTime(13, 15, 1);//h,m,s
  RTCZ.setDate(12, 1, 21);//d,m,y
  RTCZ.attachInterrupt(RTCwakeHandler);
  RTCZ.setAlarmSeconds(0);         //Just to enable GCLK
  RTCZ.enableAlarm(RTCZ.MATCH_SS); //Just to enable GCLK
  attachInterrupt(TSP_PIN_BT1, wakeHandler, FALLING);
  attachInterrupt(TSP_PIN_BT2, wakeHandler, FALLING);
  attachInterrupt(TSP_PIN_BT3, wakeHandler, FALLING);
  attachInterrupt(TSP_PIN_BT4, wakeHandler, FALLING);
#endif
  SerialMonitorInterface.begin(115200);
  pinMode(vibratePin, OUTPUT);
  digitalWrite(vibratePin, vibratePinInactive);
  initHomeScreen();
  requestScreenOn();
  BLEsetup(&phoneConnection, "TinyWatch", BLEConnect, BLEDisconnect);
  useSecurity(BLEBond);
  advertise("TinyWatch", "7905F431-B5CE-4E99-A40F-4B1E122D00D0");
  while (millisOffset() < 1000)BLEProcess();

#if defined(ARDUINO_ARCH_SAMD)
  // https://github.com/arduino/ArduinoCore-samd/issues/142
  // Clock EIC in sleep mode so that we can use pin change interrupts
  // The RTCZero library will setup generic clock 2 to XOSC32K/32
  // and we'll use that for the EIC. Does not seem to increase power consumption with the current SAMD21 Arduino core.
  GCLK->CLKCTRL.reg = uint16_t(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_ID( GCLK_CLKCTRL_ID_EIC_Val ) );
  while (GCLK->STATUS.bit.SYNCBUSY) {}
#endif


  //display.off();
  //while (1) RTCZ.standbyMode();
}

uint32_t millisOffset() {
#if defined (ARDUINO_ARCH_AVR)
  return millis();
#elif defined(ARDUINO_ARCH_SAMD)
  return (millisOffsetCount * 1000ul) + millis();
#endif
}

void loop() {

  BLEProcess();//Process any ACI commands or events from the NRF8001- main BLE handler, must run often. Keep main loop short.
  if (!ANCSInitStep) {
    ANCSInit();
  } else if (ANCSInitRetry && millisOffset() - ANCSInitRetry > 1000) {
    ANCSInit();
  }
  ANCSProcess();

  if (ANCSIsBusy()) {
    return;
  }

  amtNotifications = ANCSNotificationCount();

  if (newtime) {
    newtime = 0;
    newTimeData();
  }

  if (ANCSNewNotification()) {
    requestScreenOn();
    rewriteMenu = true;
    updateMainDisplay();
    doVibrate = millisOffset();
  }
  if (doVibrate) {
    uint32_t td = millisOffset() - doVibrate;
    if (td > 0 && td < 100) {
      digitalWrite(vibratePin, vibratePinActive);
    } else if (td > 200 && td < 300) {
      digitalWrite(vibratePin, vibratePinActive);
    } else {
      digitalWrite(vibratePin, vibratePinInactive);
      if (td > 300)doVibrate = 0;
    }
  }
  if (displayOn && (millisOffset() > mainDisplayUpdateInterval + lastMainDisplayUpdate)) {
    updateMainDisplay();
  }
  if (millisOffset() > sleepTimer + ((unsigned long)sleepTimeout * 1000ul)) {
    if (displayOn) {
      displayOn = 0;
      display.off();
    }
#if defined(ARDUINO_ARCH_SAMD)
    BLEProcess();
    watchSleep();
#endif
  }
  checkButtons();
}

int requestScreenOn() {
  sleepTimer = millisOffset();
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

void newTimeData() {
  int y, M, d, k, m, s;
  y = (TimeData[1] << 8) | TimeData[0];
  M = TimeData[2];
  d = TimeData[3];
  k = TimeData[4];
  m = TimeData[5];
  s = TimeData[6];
  //dayOfWeek = timeData[7];
  //fractionOfSecond = timeData[8];
#if defined (ARDUINO_ARCH_AVR)
  setTime(k, m, s, d, M, y);
#elif defined(ARDUINO_ARCH_SAMD)
  RTCZ.setTime(k, m, s);
  RTCZ.setDate(d, M, y - 2000);
#endif
}

void timeCharUpdate(uint8_t * newData, uint8_t length) {
  memcpy(TimeData, newData, length);
  newtime = millisOffset();
  //SerialMonitorInterface.println("Time Update Data RX");
}

void DSCharUpdate(byte * newData, byte length) {
  newDSdata(newData, length);
}

void NSCharUpdate(byte * newData, byte length) {
  newNSdata(newData, length);
}

void BLEConnect() {
  //SerialMonitorInterface.println("---------Connect");
  requestSecurity();
}

void BLEBond() {
  //SerialMonitorInterface.println("---------Bond");
  ANCSInitStep = 0;
}

void BLEDisconnect() {
  //SerialMonitorInterface.println("---------Disconnect");
  ANCSReset();
  ble_connection_state = false;
  ANCSInitStep = -1;
  advertise("TinyWatch", "7905F431-B5CE-4E99-A40F-4B1E122D00D0");
}

void ANCSInit() {
  if (ANCSInitStep == 0)if (!discoverService(&timeService, "1805"))ANCSInitStep++;
  if (ANCSInitStep == 1)if (!discoverService(&ANCSService, "7905F431-B5CE-4E99-A40F-4B1E122D00D0"))ANCSInitStep++;
  if (ANCSInitStep == 2)if (!discoverCharacteristic(&timeService, &currentTimeChar, "2A2B"))ANCSInitStep++;
  if (ANCSInitStep == 3)if (!discoverCharacteristic(&ANCSService, &NSchar, "9FBF120D-6301-42D9-8C58-25E699A21DBD"))ANCSInitStep++;
  if (ANCSInitStep == 4)if (!discoverCharacteristic(&ANCSService, &CPchar, "69D1D8F3-45E1-49A8-9821-9BBDFDAAD9D9"))ANCSInitStep++;
  if (ANCSInitStep == 5)if (!discoverCharacteristic(&ANCSService, &DSchar, "22EAC6E9-24D6-4BB5-BE44-B36ACE7C7BFB"))ANCSInitStep++;
  if (ANCSInitStep == 6)if (!enableNotifications(&currentTimeChar, timeCharUpdate))ANCSInitStep++;
  if (ANCSInitStep == 7)if (!readCharacteristic(&currentTimeChar, TimeData, sizeof(TimeData)))ANCSInitStep++;
  if (ANCSInitStep == 8)if (!enableNotifications(&DSchar, DSCharUpdate))ANCSInitStep++;
  if (ANCSInitStep == 9)if (!enableNotifications(&NSchar, NSCharUpdate))ANCSInitStep++;
  if (ANCSInitStep == 10) {
    //SerialMonitorInterface.println("Connected!");
    newtime = millisOffset();
    ble_connection_state = true;
  } else {
    ANCSInitRetry = millisOffset();
  }
}
