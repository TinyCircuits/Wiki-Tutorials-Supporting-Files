/*************************************************************************
 * Speedometer records the time duration between magnet
 * detections by a Hall sensor mounted on a spinning wheel.
 * This time duration represents one wheel revolution. Requires
 * correct wheel diameter (in inches or centimeters) to determine speed.
 *
 * Hardware by: TinyCircuits
 * Written by: Zachary Lee and Laverena Wienclaw for TinyCircuits
 *
 * Initiated: 05/14/2018
 * Updated: Tue. 05/21/2019
 ************************************************************************/
 
// Note: sensor records a value approx 16.7 times per second
#include <Wire.h>               // For I2C communication
#include <TinyScreen.h>         // For the TinyScreen+
#include <Wireling.h>
#include "TinyCircuits_A1454.h" // For the sensor

// Please adjust these values before using the speedometer
const bool USE_MPH = true; // If set to false, kilometers per hour will be used instead of mph and centimeters will be used instead of inches
const int WHEEL_DIAMETER = 9; // please enter the wheel diameter in inches (or centimeters if USE_MPH is set to false)
const int RPM_SAMPLE_PERIOD = 10; // RPM is calculated based on the last 10 seconds of data

// Variables the for the TinyScreen+ and its background
TinyScreen display = TinyScreen(TinyScreenPlus);
int background = TS_16b_Black;

// A sensor object, the methods will be used to interface with the A1454
TinyCircuits_A1454 hall = TinyCircuits_A1454();

void setup()
{
    // Begin communication
    SerialUSB.begin(9600);
    Wire.begin();
    Wireling.begin();

    Serial.begin(115200);

    // Setup and style for TinyScreen+
    display.begin();
    display.setFlip(true);
    display.setBrightness(15);
    display.setFont(thinPixel7_10ptFontInfo);

    Wireling.selectPort(0); // This must match the port the Wireling is connected to
    // on the Adapter board
    hall.begin();
}

void loop()
{
    static bool firstTime = true; // The first datapoint is not used because previousMagValue has not been assigned a meaningful value
    static long thisMagValue = 0;
    static int rpm = 0;
    static long mph = 0;
    static long kph = 0;
    static float magnetCount = 0.00; // represents how many times a magnet has been recorded
    static unsigned long timeOld = 0; // represents the amount of seconds since the last minute, goes to 0 after it hits user defined const RPM_SAMPLE_PERIOD

    long previousMagValue = thisMagValue; // The value of the Hall Sensor right now
    thisMagValue = hall.readMag();// The value of the Hall Sensor right now

    if(!firstTime) // this code will be run on every loop except the first
    {
        //delay(500);
        int change = abs(thisMagValue-previousMagValue); // A large change indicates exposure to the rim magnet

        if (change > 1000)
        {
            ++magnetCount;
        }
        // Every RPM_SAMPLE_PERIOD seconds we reset the magnetData array to 0 
        if(floor((millis()-timeOld)/1000)==RPM_SAMPLE_PERIOD)
        {
            timeOld = millis();
            magnetCount = 0;
        }
        if(abs((((float)(millis()-timeOld)/1000)/60))>0.01)
        {
            rpm = (magnetCount/2) / (((float)(millis()-timeOld)/1000)/60);
        }
        delay(15);

        if(USE_MPH)
        {
            mph = ((WHEEL_DIAMETER * PI)* rpm) / 1056; // 1056 is the conversion factor between inches/minute and miles/hour
        }
        else // use kph
        {
            kph = ((WHEEL_DIAMETER * PI)* rpm) / 1666.67; // 1666.67 is the conversion factor between cm/minute and km/hour
        }

        printToScreen(thisMagValue, rpm, mph, kph); // Prints all values to screen, with mph/kph in large font
    }

    firstTime = false;
}

// Prints the hall sensor values to the TinyScreen+
void printToScreen(int mag, int rpm, int mph, int kph)
{
    // This will make the screen look a little unsteady but is needed in order
    // to clear old values
    display.clearScreen();

    display.fontColor(TS_8b_White, background);
    display.setCursor((48 - (display.getPrintWidth("Speedometer")) / 2), 0);
    display.print("Speedometer");

    display.fontColor(TS_8b_Blue, background);
    display.setCursor(0, 16);
    display.print("Mag = ");
    display.print(mag);

    display.fontColor(TS_8b_Red, background);
    display.setCursor(0, 28);
    display.print("rpm = ");
    display.print(rpm);

    if(USE_MPH)
    {
        display.fontColor(TS_8b_Green, background);
        display.setCursor(0, 40);
        display.print("mph = ");
        display.setFont(liberationSans_22ptFontInfo);
        display.print(mph);
        display.setFont(thinPixel7_10ptFontInfo);
    }
    else // use kph
    {
        display.fontColor(TS_8b_Green, background);
        display.setCursor(0, 40);
        display.print("kph = ");
        display.setFont(liberationSans_22ptFontInfo);
        display.print(kph);
        display.setFont(thinPixel7_10ptFontInfo);
    }
}
