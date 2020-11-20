//-------------------------------------------------------------------------------
//  TinyCircuits TinyScreen/ST BLE Smartwatch Example Sketch
//  Last Updated 26 October 2017
//
//  This demo sets up the ST BLE for Nordic's BLE virtual UART connection, then
//  accepts a date setting string with the format "Dyyyy MM dd k m s" or a
//  notification string starting with a 1 or 2(for line 1 or 2) followed by the
//  short text string to display. Time, date, notifications are displayed, and a
//  simple sleep mode is implemented to save some power. TinyScreen+ compatible.
//
//  2.0.0 26 Oct 2017 Initial update release
//
//  Written by Ben Rose, TinyCircuits http://TinyCircuits.com
//
//-------------------------------------------------------------------------------


#include <SPI.h>
#include <TinyScreen.h>
#include <STBLE.h>
#include <Wire.h>

unsigned long millisOffsetCount = 0;

#define BLE_DEBUG true
#define menu_debug_print true
uint32_t doVibrate = 0;

#if defined (ARDUINO_ARCH_AVR)
TinyScreen display = TinyScreen(TinyScreenDefault);
#define SerialMonitorInterface Serial
#include <TimeLib.h>

#elif defined(ARDUINO_ARCH_SAMD)
TinyScreen display = TinyScreen(TinyScreenDefault);
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
  if (doVibrate || ble_can_sleep)
    return;
  sleepTime = RTCZ.getEpoch();
  RTCZ.standbyMode();
}
#endif

uint8_t ble_rx_buffer[21];
uint8_t ble_rx_buffer_len = 0;
uint8_t ble_can_sleep = false;
uint8_t ble_connection_state = false;
uint8_t ble_connection_displayed_state = true;


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
char notificationLine1[20] = "";
char notificationLine2[20] = "";

uint8_t vibratePin = 6;
uint8_t vibratePinActive = HIGH;
uint8_t vibratePinInactive = LOW;


int brightness = 3;
uint8_t lastSetBrightness = 100;

const FONT_INFO& font10pt = thinPixel7_10ptFontInfo;
const FONT_INFO& font22pt = liberationSansNarrow_22ptFontInfo;


void setup(void)
{

#if defined (ARDUINO_ARCH_AVR)
  for (int i = 0; i < 20; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  setTime(1, 1, 1, 16, 2, 2016);
#elif defined(ARDUINO_ARCH_SAMD)
 /* for (int i = 0; i < 27; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  pinMode(28, INPUT_PULLUP);
  pinMode(29, INPUT_PULLUP);
  pinMode(42, INPUT_PULLUP);
  pinMode(44, INPUT_PULLUP);
  pinMode(45, INPUT_PULLUP);
  pinMode(A4, INPUT);
  pinMode(2, INPUT);*/
  RTCZ.begin();
  RTCZ.setTime(16, 15, 1);//h,m,s
  RTCZ.setDate(25, 7, 16);//d,m,y
  //RTCZ.attachInterrupt(RTCwakeHandler);
  //RTCZ.enableAlarm(RTCZ.MATCH_HHMMSS);
  //RTCZ.setAlarmEpoch(RTCZ.getEpoch() + 1);
//  attachInterrupt(TSP_PIN_BT1, wakeHandler, FALLING);
//  attachInterrupt(TSP_PIN_BT2, wakeHandler, FALLING);
//  attachInterrupt(TSP_PIN_BT3, wakeHandler, FALLING);
//  attachInterrupt(TSP_PIN_BT4, wakeHandler, FALLING);
#endif
  Wire.begin()
  SerialMonitorInterface.begin(115200);
  display.begin();
  display.setFlip(true);
  pinMode(vibratePin, OUTPUT);
  digitalWrite(vibratePin, vibratePinInactive);
  initHomeScreen();
  requestScreenOn();
  delay(100);
  BLEsetup();

#if defined(ARDUINO_ARCH_SAMD)
  // https://github.com/arduino/ArduinoCore-samd/issues/142
  // Clock EIC in sleep mode so that we can use pin change interrupts
  // The RTCZero library will setup generic clock 2 to XOSC32K/32
  // and we'll use that for the EIC. Increases power consumption by ~50uA
  //GCLK->CLKCTRL.reg = uint16_t(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_ID( GCLK_CLKCTRL_ID_EIC_Val ) );
  //while (GCLK->STATUS.bit.SYNCBUSY) {}
#endif
  //while (millisOffset() < 1000)BLEProcess();
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
  aci_loop();//Process any ACI commands or events from the NRF8001- main BLE handler, must run often. Keep main loop short.
  if (ble_rx_buffer_len) {
    if (ble_rx_buffer[0] == 'D') {
      //expect date/time string- example: D2015 03 05 11 48 42
      lastReceivedTime = millisOffset();
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
      rewriteMenu = true;
      updateMainDisplay();
      doVibrate = millisOffset();
    }
    ble_rx_buffer_len = 0;
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
  if (millisOffset() > sleepTimer + ((unsigned long)sleepTimeout*1000ul)) {
    if (displayOn) {
      displayOn = 0;
      display.off();
    }
#if defined(ARDUINO_ARCH_SAMD)
    //watchSleep();
#endif
  }
  checkButtons();
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
#if defined (ARDUINO_ARCH_AVR)
  setTime(k, m, s, d, M, y);
#elif defined(ARDUINO_ARCH_SAMD)
  RTCZ.setTime(k, m, s);
  RTCZ.setDate(d, M, y - 2000);
#endif
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
