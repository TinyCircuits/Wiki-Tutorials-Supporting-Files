//-------------------------------------------------------------------------------
//  Fitness Tracker Sketch
//  Last Updated 16 August 2019
//
//  The fitness tracker uses data collected from the pulse oximetry and accelerometer wirelings
//  to count your steps, determine your sleep quality, and monitor your vital signs.
//  Abnormalities trigger an LRA buzzer, which can be felt by the user.
//  Both the raw and interpreted data is sent to a microSD card every DATA_INTERVAL seconds.
//  (You can adjust the reporting interval by changing DATA_INTERVAL on line 39.)
//  
//  Note: The pulse oximetry sensor detects your heart rate optically and the accuracy of the readings
//  is highly dependent on keeping a steady pressure between you and the sensor. The sensor will
//  return an oxygen saturation of -999 when it detects that you are too far away to measure properly.
//  The best way to maintain a steady pressure is to use a clip (as used by hospital grade pulse ox sesnors)
//  Another good way is a rubber band. We have found that fingers give the most accurate readings.
//  This project is for educational purposes only!
//
//  Written by Zachary Lee, TinyCircuits http://TinyCircuits.com
//-------------------------------------------------------------------------------

#include <SPI.h>
#include <TinyScreen.h> // This library is used to print sensor values to a TinyScreen
#include "BMA250.h" // Used to interface with the acceleromter wireling, which is used to track your steps
#include <Wire.h>
#include "Adafruit_DRV2605.h" // used to interface with the LRA wireling
#include <SdFat.h> // enables data to be logged to an sd card
#include <RTCZero.h>  // enables date and time to be recorded with each sensor reading
#include "MAX30105.h" // used to interface with the pulse oximetry sensor
#include "heartRate.h"
#include "sleepStages.h" // contains examples of ideal sleep chronologies, which are used as part of the sleep scoring algorithm 
#include "spo2_algorithm.h" // calculates oxygen saturation
#include <Wireling.h>

Adafruit_DRV2605 drv; // lra sensor object
MAX30105 particleSensor; // pulseOx sensor object
BMA250 accel_sensor; // accelerometer sensor object

// SD card variables
#define FILE_BASE_NAME "worktst.csv" // Log file base name.  Must be 13 characters or less.
char fileName[13] = FILE_BASE_NAME;
SdFat SD; // File system object.
SdFile file; // Log file.
SdFile quartiles; // keeps track of your historical heart rate during sleep, which is used in the sleep quality calculation
SdFile sleepHistory; // keeps track of your final nightly sleep quality in a seperate file. Cumulative sleep quality is calculated throughout the night and displayed every 30 seconds in the general log file, but
// the sleepQuality file is based on your entire night and includes an analysis of your sleep stage chronology, which is not included in the snapshots displayed in the regular log file.
const uint8_t ANALOG_COUNT = 4;
const int chipSelect = 10;

// TinyScreen Global Variables
TinyScreen display = TinyScreen(TinyScreenPlus);
int background = TS_8b_Black; // sets the background color to black

const int STEP_TRIGGER = 250; // The LRA wireling will notify you of inactivity if you complete less than half of this number of steps each hour. Step % is based on this * 16 waking hours.
const float memsPin = A2; // used for microphone
const int DATA_INTERVAL = 30; // data is recorded to the microSD every DATA_INTERVAL seconds
const bool DEBUG_MD = false; // if set to true, enables debug mode
const int FAST_DATA_INTERVAL = DATA_INTERVAL * 1000; // performance optimization
const int AGE = 25; // age has a significant impact on sleep composition, so inputting your age will increase the accuracy of your sleep quality calculation
const int DELAY_INTERVAL = 2800; // specifies the delay between LRA pulses in ms
int stepsTowardGoal = 0; // keeps track of how many steps you have taken the the past hour as compared to your goal. This will trigger inactivity pulses on the LRA

// heart rate and oxygen concentration variables
int heartData[2880] = {};
uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data
int32_t bufferLength = 50; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200, the lower the faster the oxygen saturation calculation
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid

/* Change these values to set the current time when you upload the sketch. */
const byte seconds = 10;
const byte minutes = 34;
const byte hours = 14;

/* Change these values to set the current initial date */
const byte day = 16;
const byte month = 8;
const byte year = 19;

// set your approximate bedtime. Your sleep quality score will be most accurate if you go to bed at this time.
// However, there is a variable time window (UP TO 30 MINUTES,you can change this number by modifying BEDTIME_ALLOWANCE) after this bedtime that will be exempt from the sleep quality calculation if you are quite active.
// The threshold to start calculating sleep quality is that the accelerometer cannot have detected more than 10 steps in the last 2 minutes. If at any point after the sleep quality calculation begins
// there is a minute with more than 15 steps, the sleep quality calculation will be reset and the entry condition of <= 10 steps in the last 2 minutes must be met again.
// At BEDTIME_ALLOWANCE minutes after your bedtime, sleep quality calculation will begin regardless of your activity level.
const int BEDTIME_HOUR = 23; // use 24 hour time only.
const int BEDTIME_MINUTE = 53;
const int BEDTIME_ALLOWANCE = 30; // see above

// used to store which sensors are connected and if so, what port they are connected to. initial 0 value represents that they are not connected
int microphoneSensorPresent = 0; // the microphone wireling is not used in this project, but we have included some code that will make it very easy to add if you so choose
int accelSensorPresent = 0;
int lraSensorPresent = 0;
int pulseOxSensorPresent = 0;

unsigned long stepTimestamps[STEP_TRIGGER] = {};
unsigned long loopStart = 0;
String sleepStage = "";
uint32_t doVibrate = 0;
bool firstSD = true;
byte sleepStageArray[1320] = {};
// when you are asleep, populated with sleep stage every 30 seconds, which is used to compare against optimal sleep stage transitions when calculating sleep quality. more details about
// methodology can be found the comments of the sleepQuality() function.
int sleepStageCounter = 0; // Used as a counter for the sleepStage array
int sleepMinutes[60] = {};

#include <RTCZero.h>
RTCZero rtc;

// heart rate variables
const byte RATE_SIZE = DATA_INTERVAL * 100; // Based on the data interval. this could take a lot of memory
byte rates[RATE_SIZE]; //heartDataay of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute = 0.00; // current bmp
int beatAvg = 0; // represents the average heart rate over the DATA_INTERVAL
int validMeasurements = 0; // used to represent how many valid measurments have been taken so far


uint8_t amtNotifications = 0;
int noiseReduced = 0;
int stepReduced = 0;
int pulseReduced = 0;
bool started = false; // once a buzz sequence has begun, it must complete

uint8_t vibratePin = 4;
uint8_t vibratePinActive = HIGH;
uint8_t vibratePinInactive = LOW;
unsigned long bedtimeEpoch = 0;
int stepArr[4] = {};

void setup(void)
{
  SerialUSB.begin(115200);
  //while (!SerialUSB);
  delay(5000); // replaces the above
  initStepTimestamps();
  Wire.begin();
  Wireling.begin();
  
  checkPorts(); // determines what Wireling is attached to which port, if any
  if (!validatePorts()) {
    SerialUSB.println("Critical error with port assignment, please assign ports manually!");
  }
  uint8_t effect = 1;
  if (lraSensorPresent) {
    drv.begin();
    drv.selectLibrary(1);
    drv.setMode(DRV2605_MODE_INTTRIG);
  }

  if (accelSensorPresent) {
    // Set the cursor to the following coordinates before it prints "BMA250 Test"
    Wireling.selectPort(accelSensorPresent - 1);
    accel_sensor.begin(BMA250_range_4g, BMA250_update_time_16ms); // Sets up the BMA250 accel_sensorerometer
  }

  if (pulseOxSensorPresent)
  {
    Wireling.selectPort(pulseOxSensorPresent - 1);
    particleSensor.setup(); //Configure sensor with default settings
    particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
    particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
  }

  // Check for SD card
  SerialUSB.println("Initializing SD card...");
  if (SD.begin(chipSelect, SD_SCK_MHZ(50)))
  {
    SerialUSB.println("card initialized.");

    SerialUSB.print(F("Logging to: "));
    SerialUSB.println(fileName);
    SerialUSB.println();
  }
  else
  {
    SerialUSB.println("SD Card not found, exiting program!");
    SerialUSB.println();
    delay(5000);
    while (1);
  }

  rtc.begin();
  rtc.setTime(hours, minutes, seconds);//h,m,s
  rtc.setDate(day, month, year);//d,m,y
  unsigned long tempEpoch = rtc.getEpoch();
  int tempHour = rtc.getHours();
  int tempMinute = rtc.getMinutes();
  rtc.setTime(BEDTIME_HOUR, BEDTIME_MINUTE, 0);
  if (BEDTIME_HOUR < tempHour || (BEDTIME_HOUR == tempHour && BEDTIME_MINUTE < tempMinute))
  {
    bedtimeEpoch = rtc.getEpoch() + 86400;
  }
  else {
    bedtimeEpoch = rtc.getEpoch();
  }
  rtc.setEpoch(tempEpoch); // reset back to current time
  unsigned long epochDiff = (max(bedtimeEpoch, tempEpoch) - min(bedtimeEpoch, tempEpoch));

  // This is the setup used to initialize the TinyScreen's appearance
  display.begin();
  display.setBrightness(15);
  display.setFlip(true);
  display.setFont(thinPixel7_10ptFontInfo);
  display.fontColor(TS_8b_White, background);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  String displayString = ""; // written once in the logfile to provide column headings along with the data
  String dataString = ""; // written to the logfile every data interval seconds. does not contain headings, just csv data only. see createstring for more details
  static int emptyIntsCounter = 0;
  static unsigned long validationEpoch = 0; // represents the hour you fell asleep
  static unsigned long screenClearTime = millis();
  static int currentHour = rtc.getHours(); // performance optimization
  static unsigned long timeUpdate = millis();
  static bool validatedPreviously = false; // avoids the need to constantly validate whether it is past bedtime or not by store the fact within this variable.
  // note that the many areas of the sketch are not executed except at night when calculating or recording sleep quality.
  static unsigned long inactivityCounter = millis();
  static unsigned long batt = millis(); // used to check the battery voltage and run some other code every data reporting inverval, default 30 seconds
  static unsigned long goalTimer = millis(); // used to check if you are meeting your daily goals
  static int nightCounter = 0;
  static int loopCounter = 0;
  static float mean = 0.00;
  static float micReading = 0.00;
  static int highestBeat = 0;
  static int lowestBeat = 0;
  static int heartIndex = 0;
  // these variables are used to keep track of how many steps were taken in recent minutes
  static unsigned long one = millis();
  static unsigned long two = millis();
  static unsigned long five = millis();
  static unsigned long fifteen = millis();
  static unsigned long oneMinute = millis();

  /*if (microphoneSensorPresent) { // example of how you would check for and reference the MEMS microphone wireling
    micReading = analogRead(memsPin);
  }*/
  Wireling.selectPort(pulseOxSensorPresent - 1);
  checkPulse(); // this needs to run as often as possible
  Wireling.selectPort(accelSensorPresent - 1);
  updatePedometer();
  if (millis() - batt > FAST_DATA_INTERVAL) // record battery voltage when needed
  {
    createString(displayString, dataString, firstSD, currentHour, validatedPreviously, validationEpoch); //create strings from recent data
    validateSD(dataString, displayString, firstSD); // write the strings to the SD card after validating the connection to the SD card is intact
    if (currentHour >= BEDTIME_HOUR && (validatedPreviously || bedtimeValidation(validatedPreviously, validationEpoch))) { // if you are asleep
      heartData[heartIndex] = beatAvg;
      ++heartIndex;
      wakeMinute(validationEpoch, emptyIntsCounter, validatedPreviously); // check if the user has been awake and moving about recently
    }
    batt = millis();
  }

  if (millis() - oneMinute > 60000)
  {
    sleepMovement(one, two, five, fifteen);
    oneMinute = millis();
    if (millis() - goalTimer > 3600000) // if it is near the end of an hour and you are not on pace to meet your daily step goal
    {
      stepsTowardGoal = getTotalSteps() - stepsTowardGoal;
      if (stepsTowardGoal < STEP_TRIGGER * 0.5) // you have completed less than half the of the steps you would need to have compelted in the past hour to be on pace to meet your goal.
      {
        buzzLRA(); // inactivity buzz
      }
      goalTimer  = millis();
    }
  }

  checkButtons(screenClearTime); // will activate display if user presses any button on the TS+

  /*
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (currentHour >= BEDTIME_HOUR && (validatedPreviously || bedtimeValidation(validatedPreviously, validationEpoch))) // this portion of the loop activates the sleep tracker mode after it detects you sleeping
    {

    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    else // this activates fitness tracker mode during the daytime hours
    {

    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
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
  rtc.setTime(k, m, s);
  rtc.setDate(d, M, y - 2000);
#endif
}

int minutesMS(int input)
{
  return input * 60000;
}

int minutesLeftInHour()
{
  return 60 - rtc.getMinutes();
}

int getStepTrigger()
{
  return STEP_TRIGGER;
}

bool heartDataayNotFull()
{
  if (stepTimestamps[STEP_TRIGGER - 1] == 4294967295)
  {
    return true;
  }
  return false;
}

float percentOfDailyStepGoal(int totalSteps)
{
  const float DAILY_GOAL = (float)STEP_TRIGGER * 16.00; // 16 waking hours in day
  if (DEBUG_MD) {
    SerialUSB.print("total steps: ");
    SerialUSB.println(totalSteps);
    SerialUSB.print("total steps %: ");
    SerialUSB.println((totalSteps / DAILY_GOAL) * 100.00);
  }
  return (totalSteps / DAILY_GOAL) * 100.00;
}

float sleepQuality(bool finalCalculation)
{
  // General literature suggests that a lack of sufficient deep or REM sleep have the most negative impact for the day ahead. We compare your deep and REM sleep quantities to averages for your
  // age and gender to determine your sleep quality. Once deep and REM are taken care of, sleep quality will increase with a higher light sleep duration and is also impacted by when
  // during the night you were in certain sleep stages. Ideally, you will be in your deepest sleep around 4 AM with a gentle transition into and out of that low point.
  // In comparison, deep sleep occuring near your wake up time lowers your sleep quality and causes you to feel groggy for couple of hours after you wake up, even if you got enough sleep total.
  // Sleep Quality is reported every 30 seconds throughout the night based on all data since the moment you fell asleep. However, the Sleep Chronology cannot be calculated until the night/entire sleeping period
  // is complete, so it is factored into the final reading of the sleeping period and that final reading is placed in a seperate text file called "sleepHistory.txt at the moment you wake up."
  // Sleep averages from here: https://www.ncbi.nlm.nih.gov/pubmed/15586779

  float deepScore = (actualDeepPercentage() / expectedDeepPercentage()) * 100; // weighting: 30% in the final calculation, 37% in intermediate calculations
  float lightScore = (actualLightPercentage() / expectedLightPercentage()) * 100; // weighting: 15%, 26% in intermediate calculations
  float REMScore = (actualREMPercentage() / expectedREMPercentage()) * 100; // weighting: 30%, 37% in intermediate calculations
  float sleepQualityPercent = 0.00;

  if (finalCalculation) { // include sleep chronology and record to a seperate file.
    sleepQualityPercent = min(((0.25 * sleepChronologyScore()) + (0.3 * deepScore) + (0.15 * lightScore) + (0.3 * REMScore)), 100);
    //SerialUSB.print("((0.25 * sleepChronologyScore()) + (0.3 * deepScore) + (0.15 * lightScore) + (0.3 * REMScore)): ");
    //SerialUSB.println(((0.25 * sleepChronologyScore()) + (0.3 * deepScore) + (0.15 * lightScore) + (0.3 * REMScore)));
    String sleepQuality = "";
    sleepQuality += String(rtc.getMonth());
    sleepQuality += "/";
    sleepQuality += String(rtc.getDay());
    sleepQuality += "/";
    sleepQuality += String(rtc.getYear());
    sleepQuality += ",";
    sleepQuality += String(sleepQualityPercent);
          if (!sleepHistory.open("sleepHistory.txt", O_CREAT | O_RDWR | O_APPEND)) {
        SerialUSB.println("File open error!");
      }
      else
      {
        uint16_t data[ANALOG_COUNT];

        // Read all channels to avoid SD write latency between readings.
        for (uint8_t i = 0; i < ANALOG_COUNT; i++) {
          data[i] = analogRead(i);
        }

        SerialUSB.print("Writing to sleepHistory: ");
        SerialUSB.println(sleepQuality);
        sleepHistory.println(sleepQuality);
        sleepHistory.close();
      }       // create a special line in a different file for historical quartile readings
  }
  else {

    sleepQualityPercent = min(((0.37 * deepScore) + (0.26 * lightScore) + (0.37 * REMScore)), 100);
  }
  SerialUSB.print("sleepQualityPercent: ");
  SerialUSB.println(sleepQualityPercent);
  return sleepQualityPercent;
}

float sleepChronologyScore() // rates sleep chronology as compared to ideal sleep chronology found in sleepStages.h (5-11)
{
  float whichArray = sleepStageCounter / 2 / 60; // chooses the most relevant comparison array depending on how long you slept the night before

  if (whichArray <= 5)
  {
    SerialUSB.println("used cross correlation 5!");
    return normalizedCrossCorrelation(five, sleepStageArray, whichArray);
  }
  else if (whichArray <= 6)
  {
    return normalizedCrossCorrelation(six, sleepStageArray, whichArray);
  }
  else if (whichArray <= 7)
  {
    return normalizedCrossCorrelation(seven, sleepStageArray, whichArray);
  }
  else if (whichArray <= 8)
  {
    return normalizedCrossCorrelation(eight, sleepStageArray, whichArray);
  }
  else if (whichArray <= 9)
  {
    return normalizedCrossCorrelation(nine, sleepStageArray, whichArray);
  }
  else if (whichArray <= 10)
  {
    return normalizedCrossCorrelation(ten, sleepStageArray, whichArray);
  }
  else
  {
    return normalizedCrossCorrelation(eleven, sleepStageArray, whichArray);
  }
}

// Calculate the battery voltage
float getBattVoltage(void) {
  const int VBATTpin = A4;
  float VCC = getVCC();

  // Use resistor division and math to get the voltage
  float resistorDiv = 0.5;
  float ADCres = 1023.0;
  float battVoltageReading = analogRead(VBATTpin);
  battVoltageReading = analogRead(VBATTpin); // Throw out first value
  float battVoltage = VCC * battVoltageReading / ADCres / resistorDiv;

  return battVoltage;
}

// This function gets the battery VCC internally, you can checkout this link
// if you want to know more about how:
// http://atmel.force.com/support/articles/en_US/FAQ/ADC-example
float getVCC() {
  SYSCTRL->VREF.reg |= SYSCTRL_VREF_BGOUTEN;
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->SAMPCTRL.bit.SAMPLEN = 0x1;
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->INPUTCTRL.bit.MUXPOS = 0x19;         // Internal bandgap input
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->CTRLA.bit.ENABLE = 0x01;             // Enable ADC
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->SWTRIG.bit.START = 1;  // Start conversion
  ADC->INTFLAG.bit.RESRDY = 1;  // Clear the Data Ready flag
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->SWTRIG.bit.START = 1;  // Start the conversion again to throw out first value
  while ( ADC->INTFLAG.bit.RESRDY == 0 );   // Waiting for conversion to complete
  uint32_t valueRead = ADC->RESULT.reg;
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->CTRLA.bit.ENABLE = 0x00;             // Disable ADC
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  SYSCTRL->VREF.reg &= ~SYSCTRL_VREF_BGOUTEN;
  float vcc = (1.1 * 1023.0) / valueRead;
  return vcc;
}

float getBattPercent()
{
  float batteryLeft = max((getBattVoltage() - 3.00), 0);
  if (DEBUG_MD) {
    SerialUSB.print("battery left: ");
    SerialUSB.println(min(batteryLeft * 83.333333, 100));
  }
  return min((batteryLeft * 83.333333), 100); // hard upper limit of 100 as it often shows over 100 when charging
}

bool microphoneConnected()
{
  return false; // for now, as microphone is not part of this project. if you wish to add it, remove this statement
  //selectPort(3);
  //Wire.beginTransmission(); // you will need to google the microphone part number to find the correct i2c address in its datasheet
  Wire.write(1);
  Wire.endTransmission();
  delay(5);
  //Wire.requestFrom(, 2); // then, plug the i2c address you found in this line and uncomment it
  if (Wire.available() != 2)
  {
    SerialUSB.println();
    return false;
  }
  return true;
}

bool accelConnected()
{
  Wire.beginTransmission(0x18);
  Wire.write(1);
  Wire.endTransmission();
  delay(5);
  Wire.requestFrom(0x18, 2);
  if (Wire.available() != 2)
  {
    return false;
  }
  return true;
}

bool lraConnected()
{
  Wire.beginTransmission(0x5A);
  Wire.write(1);
  Wire.endTransmission();
  delay(5);
  Wire.requestFrom(0x5A, 2);
  if (Wire.available() != 2)
  {
    return false;
  }
  return true;
}

bool pulseOxConnected()
{
  if (particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    return true;
  }
  return false;
}

int whichWireling(int port) // determines which wireling is connected to the input port number. returns 0 = microphone; 1=accel; 2= lra; 3=pulseOx;
{
  Wireling.selectPort(port);
  if (microphoneConnected())
  {
    microphoneSensorPresent = port + 1; // + 1 because they are initialized to 0, which continutes to represent that the sensor
    // has not been connected in the rest of the source code
    return 0;
  }
  else if (accelConnected())
  {
    accelSensorPresent = port + 1;
    return 1;
  }
  else if (lraConnected())
  {
    lraSensorPresent = port + 1;
    return 2;
  }
  else if (pulseOxConnected())
  {
    pulseOxSensorPresent = port + 1;
    return 3;
  }
  else
  {
    return 5; // Wireling port empty
  }
}

void checkPorts()
{
  for (int i = 0; i < 4; ++i)
  {
    if (DEBUG_MD) {
      SerialUSB.print("Wireling: ");
    }

    if (DEBUG_MD) {
      SerialUSB.print(whichWireling(i));
    }
    else {
      whichWireling(i);
    }

    if (DEBUG_MD) {
      SerialUSB.print("found on port ");
      SerialUSB.println(i);
    }
  }
}

bool bedtimeValidation(bool &validatedPreviously, unsigned long &validationEpoch) // determines when to start calculating sleep quality based on your bedtime setting and movements around that time
{
  if ((rtc.getEpoch() > bedtimeEpoch) && pastWindow())
  {
    SerialUSB.println("You are officially considered asleep!!");
    dailyStepReset(); // reset step totals for the day once you fall asleep
    validatedPreviously = true;
    validationEpoch = rtc.getEpoch();
    return true;
  }
  return false;
}

bool pastWindow() // determines if current time is past BEDTIME_ALLOWANCE window, not called often
{
  int stepOne = stepsInPastXMinutes(1); // changing inputs to this function will result in errors
  SerialUSB.print("stepOne :");
  SerialUSB.println(stepOne);
  int stepTwo = stepsInPastXMinutes(2);
  SerialUSB.print("stepTwo :");
  SerialUSB.println(stepTwo);
  int stepFive = stepsInPastXMinutes(5);
  SerialUSB.print("stepFive :");
  SerialUSB.println(stepFive);
  bool bedtimeStarted = false;
  if (stepOne < 10 && stepTwo < 15)
  {
    bedtimeStarted = true;
  }

  if (((rtc.getEpoch() > bedtimeEpoch) && (rtc.getEpoch() - bedtimeEpoch) > (BEDTIME_ALLOWANCE * 60)) || bedtimeStarted)
  {
    return true;
  }
  return false;
}

int stepsInPastXMinutes(int x)
{
  if (x == 1)
  {
    return stepArr[0];
  }
  else if (x == 2)
  {
    return stepArr[1];
  }
  else if (x == 5)
  {
    return stepArr[2];
  }
  else if (x == 15)
  {
    return stepArr[3];
  }
  else
  {
    SerialUSB.println("Critical Error: stepsInPastXMinutes expected a different input");
  }
}

void resetSleepStageArray()
{
  for (int i = 0; i < 1320; ++i)
  {
    sleepStageArray[i] = 0;
  }
}

String currentSleepStage() // pulse calculations based on this study https://www.nature.com/articles/1300146
{
  if (stepsInPastXMinutes(15) > 15) // this means you have tossed and turned several times, which should not happen in deep or REM
  {
    sleepStageArray[sleepStageCounter] = 0;
    ++sleepStageCounter;
    return "Light";
  }

  int deepSleepRate = firstQuartile(); // represents the first quartile of all observed sleeping heart rates for the life of the sketch. This leads to a much more personalized reading than hard coding a number.
  int REMSleepRate = thirdQuartile(); // on average, REM sleep has a heart rate near the third quartile of all observed heart rates. deep sleep is near the first quartile.
  int lightSleepRate = (abs(deepSleepRate - REMSleepRate) / 1.95) + deepSleepRate; // heart rate during light sleep tends to be slightly above average
  // calculate the difference between the average heart rate for the current block of time and each of the expected heart rates for a given sleep stage. that way, we can assign this block of time
  // a sleep stage and incorporate it into future calculations as well.
  int lightDiff = abs(beatAvg - lightSleepRate);
  int deepDiff = abs(beatAvg - deepSleepRate);
  int REMDiff = abs(beatAvg - REMSleepRate);
  int minDiff = min(lightDiff, min(deepDiff, REMDiff));

  if (lightDiff == minDiff)
  {
    sleepStageArray[sleepStageCounter] = 0;
    ++sleepStageCounter;
    return "Light";
  }
  else if (deepDiff == minDiff)
  {
    sleepStageArray[sleepStageCounter] = 2;
    ++sleepStageCounter;
    return "Deep";
  }
  else if (REMDiff == minDiff)
  {
    sleepStageArray[sleepStageCounter] = 1;
    ++sleepStageCounter;
    return "REM";
  }
  else
  {
    return "Error";
    SerialUSB.println("Error assigning sleep stage!");
  }
}

int firstQuartile() { // this returns the first quartile of your heart rate when sleeping by reading all data within quartiles.txt. this is used as a benchmark for deep sleep, which has the lowest heartrate.
  // Maximum line length plus space for zero byte. It finds this by calculating the mean of the first quartiles of all previous nights
  const int line_buffer_size = 8;
  char buffer[line_buffer_size];
  char holdingArr[line_buffer_size];
  ifstream sdin("quartiles.txt");
  int line_number = 0;
  int quartileTemp = 0;
  int quartileCounter = 0;
  unsigned long sum = 0; // used to calculate the mean of the quartiles at the end of the function

  while (sdin.getline(buffer, line_buffer_size, '\n') || sdin.gcount()) {
    int count = sdin.gcount();
    if (sdin.fail()) {
      SerialUSB.println("Partial long line");
      sdin.clear(sdin.rdstate() & ~ios_base::failbit);
    } else if (sdin.eof()) {
      SerialUSB.println();  // sdin.fail() is false
    } else {
      count--;  // Don’t include newline in count
    }
    for (int i = 0; i < line_buffer_size; ++i)
    {
      holdingArr[i] = buffer[i];
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < line_buffer_size; ++i)
    {
      char two[3]; // heart rate could be two digits
      char three[4]; // it could also be three digits
      if (holdingArr[i] == ',')
      {
        if (i == 2) // first quartile is 2 digits
        {
          two[0] = holdingArr[0];
          two[1] = holdingArr[1];
          two[2] = '\0';
          quartileTemp = (atoi(two));
          sum += quartileTemp;
          ++quartileCounter;
        }
        else if (i == 3) // first quartile is 3 digits
        {
          three[0] = holdingArr[0];
          three[1] = holdingArr[1];
          three[2] = holdingArr[2];
          three[3] = '\0';
          quartileTemp = atoi(three);
          sum += quartileTemp;
          ++quartileCounter;
        }
        else {
          SerialUSB.println("Critical Error reading quartiles.txt!!");
        }
      }
    }
  }
  if ((sum / quartileCounter) < 10)
  {
    return 48;
  }
  if(DEBUG_MD){
  SerialUSB.print("first quartile: ");
  SerialUSB.println((sum / quartileCounter));
  }
  return (sum / quartileCounter);
}

int thirdQuartile() { // this returns the third quartile of your heart rate when sleeping by reading all data within quartiles.txt. this is used as a benchmark for REM sleep, which has the highest heartrate.
  const int line_buffer_size = 8; // worst case: both heart rates are three digits +comma+null terminator
  char buffer[line_buffer_size] = { '0', '0', '0', '0', '0', '0', 't', '\0' };
  char holdingArr[line_buffer_size] = { '0', '0', '0', '0', '0', '0', 't', '\0' };
  ifstream sdin("quartiles.txt");
  int line_number = 0;
  int quartileTemp = 0;
  int quartileCounter = 0;
  unsigned long sum = 0; // used to calculate the mean of the quartiles at the end of the function
  bool firstTwo = false;
  bool firstThree = false;

  while (sdin.getline(buffer, line_buffer_size, '\n') || sdin.gcount()) {
    int count = sdin.gcount();
    if (sdin.fail()) {
      sdin.clear(sdin.rdstate() & ~ios_base::failbit);
    }
    else if (sdin.eof()) {
      SerialUSB.println();  // sdin.fail() is false
    }
    else {
      count--;  // Don’t include newline in count
    }
    for (int i = 0; i < line_buffer_size; ++i)
    {
      holdingArr[i] = buffer[i];
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < line_buffer_size; ++i)
    {
      char two[3]; // heart rate could be two digits
      char three[4]; // it could also be three digits
      if (holdingArr[i] == ',')
      {
        if (i == 2) // first quartile is 2 digits
        {
          firstTwo = true;
          firstThree = false;
        }
        else if (i == 3) // first quartile is 3 digits
        {
          firstThree = true;
          firstTwo = false;
        }
        else {
          SerialUSB.println("Critical Error reading quartiles.txt!!");
        }
      }
      else if (holdingArr[i] == '\0') // the final character in a c string
      {
        if (holdingArr[i - 1] == 't' || holdingArr[i - 1] == '\0') // at least one of the heart rates was only two digits
        {
          if (firstTwo && holdingArr[i - 1] == 't') // both of the heart rates are two digits
          {
            two[0] = holdingArr[i - 4];
            two[1] = holdingArr[i - 3];
            two[2] = '\0';
            quartileTemp = (atoi(two));
            ++quartileCounter;
          }
          else if (firstTwo && holdingArr[i - 1] == '\0') // only the first heart rate was two digits
          {
            three[0] = holdingArr[i - 4];
            three[1] = holdingArr[i - 3];
            three[2] = holdingArr[i - 2];
            three[3] = '\0';
            quartileTemp = atoi(three);
            ++quartileCounter;
          }
          else if (firstThree) // only the second heart rate was two digits
          {
            two[0] = holdingArr[i - 3];
            two[1] = holdingArr[i - 2];
            two[2] = '\0';
            quartileTemp = (atoi(two));
            ++quartileCounter;
          }
          else {
            SerialUSB.println("Critical Error: array length could not be determined");
          }
        }
        else if (holdingArr[line_buffer_size - 2] == '1' || holdingArr[line_buffer_size - 2] == '2' || holdingArr[line_buffer_size - 2] == '3' || holdingArr[line_buffer_size - 2] == '4' || holdingArr[line_buffer_size - 2] == '5' || holdingArr[line_buffer_size - 2] == '6' || holdingArr[line_buffer_size - 2] == '7' || holdingArr[line_buffer_size - 2] == '8' || holdingArr[line_buffer_size - 2] == '9' || holdingArr[line_buffer_size - 2] == '0')
        {
          three[0] = holdingArr[i - 3];
          three[1] = holdingArr[i - 2];
          three[2] = holdingArr[i - 1];
          three[3] = '\0';
          quartileTemp = atoi(three);
          ++quartileCounter;
        }
      }
    }
    sum += quartileTemp;
  }
  if ((sum / quartileCounter) < 10)
  {
    return 56;
  }
  if(DEBUG_MD){
  SerialUSB.print("third quartile: ");
  SerialUSB.println((sum / quartileCounter));
  }
  return (sum / quartileCounter);
}

void checkPulse() // this needs to be run extremely often to return an accurate value.
{
  //selectPort(pulseOxSensorPresent-1);
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the heartDataay
      rateSpot %= RATE_SIZE; //Wrap variable
      if (validMeasurements < RATE_SIZE) {
        ++validMeasurements;
      }

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < min(validMeasurements, RATE_SIZE) ; x++)
        beatAvg += rates[x];
      beatAvg /= min(validMeasurements, RATE_SIZE);
    }
  }

  /*SerialUSB.print("IR=");
    SerialUSB.print(irValue);
    SerialUSB.print(", BPM=");
    SerialUSB.print(beatsPerMinute);
    SerialUSB.print(", Avg BPM=");
    SerialUSB.print(beatAvg);

    if (irValue < 50000) {
    SerialUSB.print(" No finger?");
    }

    SerialUSB.println();*/
}

int32_t checkOxygenSaturation()
{
  //read the first 100 samples, and determine the signal range
  for (byte i = 0; i < bufferLength; i++)
  {
    while (particleSensor.available() == false) {//do we have new data?
      particleSensor.check(); //Check the sensor for new data
    }
    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();

    particleSensor.nextSample();
  }
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  return spo2;
}

void validateSD(String dataString, String displayString, bool firstSD)
{
  if (!file.open(fileName, O_CREAT | O_RDWR | O_APPEND)) {
    SerialUSB.println("File open error!");
  }
  else
  {
    // if the file is available, write to it:
    logData(dataString, displayString);
  }

  // Force data to SD and update the directory entry to avoid data loss.
  if (!file.sync() || file.getWriteError()) {
    SerialUSB.println("write error");
  }
  file.close();
}

// Log a data record.
void logData(String dataString, String displayString) {
  uint16_t data[ANALOG_COUNT];

  // Read all channels to avoid SD write latency between readings.
  for (uint8_t i = 0; i < ANALOG_COUNT; i++) {
    data[i] = analogRead(i);
  }
  if (DEBUG_MD) {
    SerialUSB.println("WRITING TO FILE!!");
  }
 if (firstSD)
  {
    file.println(displayString);
    firstSD = false;
  }
  else if (firstSD == false) {
    file.println(dataString);
  }
  if (DEBUG_MD) {
    SerialUSB.println("WRITING Complete!");
    SerialUSB.print("dataString: ");
    SerialUSB.println(dataString);
  }
}

void createString(String &displayString, String &dataString, bool firstSD, int currentHour, bool &validatedPreviously, unsigned long &validationEpoch) {
  int total = getTotalSteps();
  unsigned long epoch = rtc.getEpoch();
  int percent = percentOfDailyStepGoal(total);
  int battery = getBattPercent();
  if (firstSD) // only the first line will look like this so that you know what data is in each column
  {
    displayString += "epochTime: ";
    displayString += String(epoch);
    displayString += ",";
    displayString += " stepCount: ";
    displayString += String(total);
    displayString += ",";
    displayString += " stepPercent: ";
    displayString += String(percent);
    displayString += ",";
    displayString += " pulse: ";
    displayString += String(beatAvg); // represents the average pulse recorded since the last time file was written to
    displayString += ",";
    if (currentHour >= BEDTIME_HOUR && (validatedPreviously || bedtimeValidation(validatedPreviously, validationEpoch))) {
      displayString += " sleepStage: ";
      displayString += String(currentSleepStage()); // run this before sleep quality as it increments sleepStageCounter
      displayString += ",";
      displayString += " sleepQualityPercent: ";
      displayString += String(sleepQuality(false));
      displayString += ",";
    }
    else
    {
      displayString += " sleepQualityPercent: ";
      displayString += "N/A";
      displayString += ",";
      displayString += " sleepStage: ";
      displayString += "Awake";
      displayString += ",";
    }
    Wireling.selectPort(pulseOxSensorPresent - 1);
    displayString += " Oxygen Saturation: ";
    displayString += String(checkOxygenSaturation());
    displayString += ",";
    Wireling.selectPort(accelSensorPresent - 1);
    displayString += " batt: ";
    displayString += String(battery);
  }
  else
  {
    dataString += String(epoch);
    dataString += ",";
    dataString += String(total);
    dataString += ",";
    dataString += String(percent);
    dataString += ",";
    dataString += String(beatAvg);
    dataString += ",";
    if (currentHour >= BEDTIME_HOUR && (validatedPreviously || bedtimeValidation(validatedPreviously, validationEpoch))) {
      dataString += String(currentSleepStage());
      dataString += ",";
      dataString += String(sleepQuality(false));
      dataString += ",";
    }
    else
    {
      dataString += "N/A";
      dataString += ",";
      dataString += "Awake";
      dataString += ",";
    }
    Wireling.selectPort(pulseOxSensorPresent - 1);
    dataString += String(checkOxygenSaturation());
    dataString += ",";
    Wireling.selectPort(accelSensorPresent - 1);
    dataString += String(battery);
  }
}

void wakeMinute(int validationEpoch, int &emptyIntsCounter, bool &validatedPreviously) { // detects the approximate time that you woke up and records it to the wakeTime variable
  SerialUSB.println("wake mnute run");
  delay(9999);
  if (millis() - validationEpoch > 18000000) // if you have been asleep at least 5 hours, begin checking for wakeup
  {
    if (stepsInPastXMinutes(15) > 110) // you are now considered awake
    {
      SerialUSB.println("You are now considered awake!!!");
      if (!quartiles.open("quartiles.txt", O_CREAT | O_RDWR | O_APPEND)) {
        SerialUSB.println("File open error!");
      }
      else
      {
        uint16_t data[ANALOG_COUNT];

        // Read all channels to avoid SD write latency between readings.
        for (uint8_t i = 0; i < ANALOG_COUNT; i++) {
          data[i] = analogRead(i);
        }
        String quartile = "";
        quartile += getFirst(emptyIntsCounter);
        quartile += ",";
        quartile += getThird(emptyIntsCounter);
        SerialUSB.print("quartile: ");
        SerialUSB.println(quartile);
        quartiles.println(quartile);
        quartiles.close();
      }       // create a special line in a different file for historical quartile readings
      sleepQuality(true);
      validationEpoch = 4294967000; // reset to a high value to prevent false positives
      validatedPreviously = false;
      resetSleepStageArray();
      unsigned long tempEpoch = rtc.getEpoch();
      int tempHour = rtc.getHours();
      int tempMinute = rtc.getMinutes();
      rtc.setTime(BEDTIME_HOUR, BEDTIME_MINUTE, 0);
      if (BEDTIME_HOUR < tempHour || (BEDTIME_HOUR == tempHour && BEDTIME_MINUTE < tempMinute))
      {
        bedtimeEpoch = rtc.getEpoch() + 86400; // adjust bedtimeEpoch for the next day
      }
      else {
        bedtimeEpoch = rtc.getEpoch();
      }
      rtc.setEpoch(tempEpoch); // reset back to current time
      unsigned long epochDiff = (max(bedtimeEpoch, tempEpoch) - min(bedtimeEpoch, tempEpoch));
    }
  }
}

String getFirst(int &emptyIntsCounter)
{
  //sorting - ASCENDING ORDER
  for (int i = 0; i < 2880; i++)
  {
    for (int j = i + 1; j < 2880; j++)
    {
      if (heartData[i] > heartData[j])
      {
        int temp  = heartData[i];
        heartData[i] = heartData[j];
        heartData[j] = temp;
      }
    }
  }

  for (int i = 0; i < 2880; ++i)
  {
    if (heartData[i] == 0)
    {
      ++emptyIntsCounter;
    }
  }
  return String(max(10, heartData[(int)((2880 - emptyIntsCounter) * 0.25) + emptyIntsCounter])); // after the array is sorted, we know the index of the first quartile after adjusting for unfilled array positions characterized by 0's
}

String getThird(int &emptyIntsCounter) // must be called immediatly after getFirst, otherwise the array will not be sorted
{
  int thirdQuartile = heartData[(int)((2880 - emptyIntsCounter) * 0.75) + emptyIntsCounter];
  resetHeartData(); // reset the array to 0's once we have calculated the quartile values to avoid old values being used in the next quartile calculation
  emptyIntsCounter = 0; // same reason as above
  return String(max(10, thirdQuartile));
}

void resetHeartData()
{
  for (int i = 0; i < 2880; ++i)
  {
    heartData[i] = 0;
  }
}

float normalizedCrossCorrelation(const byte First[], byte Second[], float whichArray) // takes into account the similarity between the values of two arrays and the order in which those values occur https://en.wikipedia.org/wiki/Cross-correlation
{
  float sum1 = 0;
  float temp = 0;
  float temp2 = 0;
  float sqsum1 = 0;
  float sqsum2 = 0;
  float Fm = 0;
  float Sm = 0;
  int size = sleepStageCounter;
  SerialUSB.print("size: ");
  SerialUSB.println(size);

  for (int i = 0; i < size; i++) {
    temp += First[i];
  }
  for (int i = 0; i < size; i++) {
    temp2 += Second[i];
  }

  Fm = temp / size;
  Sm = temp2 / size;

  for (int i = 0; i < size; i++) {
    sum1 += (First[i] - Fm) * (Second[i] - Sm);
    sqsum1 += pow((First[i] - Fm), 2);
    sqsum2 += pow((Second[i] - Sm), 2);
  }
  float result = (sum1 / (sqrt(sqsum1 * sqsum2)));
  SerialUSB.print("NCC Second & First : ");
  SerialUSB.println(result);
  if(isnan(result)) 
  {
    SerialUSB.println("error calcualting NCC!!");
    return 0.5;
  }
  return result;
}

int expectedDeepPercentage() // expected amount of deep sleep decreases as age increases https://pdfs.semanticscholar.org/757d/7cb701ea5e734af50e8d700ecb763359cd57.pdf
{
  if (AGE <= 15) {
    return 25;
  }
  else if (AGE <= 20) {
    return 21;
  }
  else if (AGE <= 25) {
    return 19;
  }
  else if (AGE <= 30) {
    return 18;
  }
  else if (AGE <= 35) {
    return 16;
  }
  else if (AGE <= 40) {
    return 15;
  }
  else if (AGE <= 45) {
    return 14;
  }
  else if (AGE <= 50) {
    return 13;
  }
  else if (AGE <= 55) {
    return 12;
  }
  else if (AGE <= 60) {
    return 11;
  }
  else if (AGE <= 65) {
    return 10;
  }
  else if (AGE <= 80) {
    return 9;
  }
  else if (AGE <= 90) {
    return 8;
  }
  else {
    return 7;
  }
}

int expectedREMPercentage() // expected amount of REM sleep decreases as age increases https://pdfs.semanticscholar.org/757d/7cb701ea5e734af50e8d700ecb763359cd57.pdf
{
  if (AGE <= 30) {
    return 22;
  }
  else if (AGE <= 40) {
    return 21;
  }
  else if (AGE <= 60) {
    return 20;
  }
  else if (AGE <= 75) {
    return 19;
  }
  else {
    return 18;
  }
}

int expectedLightPercentage() // expected amount of light sleep increases as age increases https://pdfs.semanticscholar.org/757d/7cb701ea5e734af50e8d700ecb763359cd57.pdf
{
  if (AGE <= 15) {
    return 46;
  }
  else if (AGE <= 20) {
    return 48;
  }
  else if (AGE <= 25) {
    return 49;
  }
  else if (AGE <= 35) {
    return 50;
  }
  else if (AGE <= 45) {
    return 51;
  }
  else if (AGE <= 50) {
    return 52;
  }
  else if (AGE <= 55) {
    return 53;
  }
  else if (AGE <= 60) {
    return 54;
  }
  else if (AGE <= 70) {
    return 55;
  }
  else if (AGE <= 80) {
    return 56;
  }
  else if (AGE <= 90) {
    return 57;
  }
  else {
    return 58;
  }
}

float actualDeepPercentage() // returns the percentage of deep sleep for the previous night
{
  int deepCounter = 0;
  for (int i = 0; i < sleepStageCounter; ++i)
  {
    if (sleepStageArray[i] == 2)
    {
      ++deepCounter;
    }
  }
  float test = (deepCounter / sleepStageCounter) * 100;
  return test;
}

float actualLightPercentage() // returns the percentage of light sleep for the previous night
{
  int lightCounter = 0;
  for (int i = 0; i < sleepStageCounter; ++i)
  {
    if (sleepStageArray[i] == 0)
    {
      ++lightCounter;
    }
  }
  return (lightCounter / sleepStageCounter) * 100;
}

float actualREMPercentage() // returns the percentage of light sleep for the previous night
{
  int REMCounter = 0;
  for (int i = 0; i < sleepStageCounter; ++i)
  {
    if (sleepStageArray[i] == 1)
    {
      ++REMCounter;
    }
  }
  return (REMCounter / sleepStageCounter) * 100;
}

bool validatePorts() // confirms that there are no duplicate ports; only run once during setup
{
  if (pulseOxSensorPresent == accelSensorPresent) // these two ports have the same number?
  {
    if (pulseOxSensorPresent != 0) // confirm that they both aren't empty
    {
      return false; // oops, this shouldn't happen. alert setup that multiple wirelings have been assigned the same port number
    }
  }
  if (pulseOxSensorPresent == microphoneSensorPresent) // these two ports have the same number?
  {
    if (pulseOxSensorPresent != 0) // confirm that they both aren't empty
    {
      return false; // oops, this shouldn't happen. alert setup that multiple wirelings have been assigned the same port number
    }
  }
  if (pulseOxSensorPresent == lraSensorPresent) // these two ports have the same number?
  {
    if (pulseOxSensorPresent != 0) // confirm that they both aren't empty
    {
      return false; // oops, this shouldn't happen. alert setup that multiple wirelings have been assigned the same port number
    }
  }
  if (accelSensorPresent == lraSensorPresent) // these two ports have the same number?
  {
    if (accelSensorPresent != 0) // confirm that they both aren't empty
    {
      return false; // oops, this shouldn't happen. alert setup that multiple wirelings have been assigned the same port number
    }
  }
  if (accelSensorPresent == microphoneSensorPresent) // these two ports have the same number?
  {
    if (accelSensorPresent != 0) // confirm that they both aren't empty
    {
      return false; // oops, this shouldn't happen. alert setup that multiple wirelings have been assigned the same port number
    }
  }
  if (accelSensorPresent == pulseOxSensorPresent) // these two ports have the same number?
  {
    if (accelSensorPresent != 0) // confirm that they both aren't empty
    {
      return false; // oops, this shouldn't happen. alert setup that multiple wirelings have been assigned the same port number
    }
  }
  if (microphoneSensorPresent == lraSensorPresent) // these two ports have the same number?
  {
    if (microphoneSensorPresent != 0) // confirm that they both aren't empty
    {
      return false; // oops, this shouldn't happen. alert setup that multiple wirelings have been assigned the same port number
    }
  }
  return true;
}

void sleepMovement(unsigned long &one, unsigned long &two, unsigned long &five, unsigned long &fifteen)
{
  int minutes = rtc.getMinutes();
  if (millis() - one > 60000)
  {
    sleepMinutes[minutes] = getTotalSteps();
    if (minutes - 1 >= 0) {
      stepArr[0] = getTotalSteps() - sleepMinutes[minutes - 1];
      one = millis();
    }
    else
    {
      stepArr[0] = getTotalSteps() - sleepMinutes[59-minutes];
      one = millis();
    }
  }
  if (millis() - two > 120000)
  {
    if (minutes - 2 >= 0) {
      stepArr[1] = getTotalSteps() - sleepMinutes[minutes - 2];
      two = millis();
    }
    else
    {
      stepArr[1] = getTotalSteps() - sleepMinutes[59-minutes];
      two = millis();
    }
  }
  if (millis() - five > 300000)
  {
    if (minutes - 5 >= 0) {
      stepArr[2] = getTotalSteps() - sleepMinutes[minutes - 5];
      five = millis();
    }
    else
    {
      stepArr[2] = getTotalSteps() - sleepMinutes[59-minutes];
      five = millis();
    }
  }
  if (millis() - fifteen > 900000)
  {
    if (minutes - 15 >= 0) {
      stepArr[3] = getTotalSteps() - sleepMinutes[minutes - 15];
      fifteen = millis();
    }
    else
    {
      stepArr[3] = getTotalSteps() - sleepMinutes[59-minutes];
      fifteen = millis();
    }
  }
}

void buzzLRA()
{
  Wireling.selectPort(lraSensorPresent - 1);
  activateLRA(2000);
  delay(2000);
  activateLRA(2000);
}

void activateLRA(int duration)
{
  unsigned long startTime = millis();
  uint8_t effect = 1;
  while (millis() - startTime < duration) {
    Wire.beginTransmission(0x70);
    Wire.write(0x04 + 0);  //0x04 + port (0-3)
    Wire.endTransmission();
    drv.useLRA();
    // play the effect!
    drv.go();
    // wait a bit
    delay(100);
    effect++;
    if (effect > 117) {
      effect = 1;
    }
  }
}

void resetStepCounters()
{
  for (int i = 0; i < 4; ++i)
  {
    stepArr[i] = 0;
  }
  stepsTowardGoal = 0;
}

void checkButtons(unsigned long &screenClearTime)
{
  if(display.getButtons(TSButtonUpperLeft) || display.getButtons(TSButtonUpperRight) || display.getButtons(TSButtonLowerLeft) || display.getButtons(TSButtonLowerRight))
  {
      int total = getTotalSteps();
  int percent = percentOfDailyStepGoal(total);
  int battery = getBattPercent();
  if(rtc.getSeconds() == 0 && millis()-screenClearTime > 1000){
  display.clearScreen();
  screenClearTime = millis();
  }
    display.on();
    display.setCursor(0,0);
    display.print(rtc.getMonth());
    display.print("/");
    display.print(rtc.getDay());
    display.print("/");
    display.print(rtc.getYear());
    display.print(" ");
    display.print(rtc.getHours());
    display.print(":");
    display.print(rtc.getMinutes());
    display.print(":");
    display.print(rtc.getSeconds());
    display.setCursor(0,10);
    display.print("Steps: ");
    display.println(total);
    display.setCursor(0,20);
    display.print("Step %: "); 
    display.println(percent);
    display.setCursor(0,30);
    display.print("Heart Rate: ");
    display.println(beatAvg);
    display.setCursor(0,40);
    display.print("Oxygen: ");
    display.println(spo2);
    display.setCursor(0,50);
    display.print("Battery %: ");
    display.println(battery);
  }
  else
  {
    display.off();
  }
}
