/*
Thermal Camera SD Capture

This program uses a Thermal Camera Wireling on Port 0 to display a thermal
array on a TinyScreen+. The Lower Right (bootloader) button on the TinyScreen+ 
can be used to capture the 8x8 array of thermal readings (in degrees Celsius)
to a CSV file on a microSD card attached via the microSD TinyShield.

Each time the system is powered on, it creates a new directory and all captures
taken in that boot cycle will be saved to that directory.
*/

#include <Wire.h>             // for interfacing with Wirelings and TinyScreen+
#include <SPI.h>              // for interfacing with microSD card
#include <SD.h>               // for microSD card
#include <TinyScreen.h>       // for TinyScreen
#include <Adafruit_AMG88xx.h> 
                              
/* * * * * * * * * * * * TINYSCREEN+ STUFF * * * * * * * * * * * */
TinyScreen display = TinyScreen(TinyScreenPlus);
uint8_t buffer[96 * 64 * 2]; // screen resolution * 2 (16-bit colors)
int xMax = display.xMax;
int yMax = display.yMax;
int width; // used for centering debug text
int pixelSize; // used to find the size of an onscreen "pixel" in terms of actual pixels...if that makes sense
uint8_t dispColors[AMG88xx_PIXEL_ARRAY_SIZE]; // used to store 8x8 grid of colors to display (*2 since actually 16-bit)

// the 255 colors we will be using
const uint16_t camColors[] = {
0x480F,0x400F,0x400F,0x400F,0x4010,0x3810,0x3810,0x3810,
0x3810,0x3010,0x3010,0x3010,0x2810,0x2810,0x2810,0x2810,
0x2010,0x2010,0x2010,0x1810,0x1810,0x1811,0x1811,0x1011,
0x1011,0x1011,0x0811,0x0811,0x0811,0x0011,0x0011,0x0011,
0x0011,0x0011,0x0031,0x0031,0x0051,0x0072,0x0072,0x0092,
0x00B2,0x00B2,0x00D2,0x00F2,0x00F2,0x0112,0x0132,0x0152,
0x0152,0x0172,0x0192,0x0192,0x01B2,0x01D2,0x01F3,0x01F3,
0x0213,0x0233,0x0253,0x0253,0x0273,0x0293,0x02B3,0x02D3,
0x02D3,0x02F3,0x0313,0x0333,0x0333,0x0353,0x0373,0x0394,
0x03B4,0x03D4,0x03D4,0x03F4,0x0414,0x0434,0x0454,0x0474,
0x0474,0x0494,0x04B4,0x04D4,0x04F4,0x0514,0x0534,0x0534,
0x0554,0x0554,0x0574,0x0574,0x0573,0x0573,0x0573,0x0572,
0x0572,0x0572,0x0571,0x0591,0x0591,0x0590,0x0590,0x058F,
0x058F,0x058F,0x058E,0x05AE,0x05AE,0x05AD,0x05AD,0x05AD,
0x05AC,0x05AC,0x05AB,0x05CB,0x05CB,0x05CA,0x05CA,0x05CA,
0x05C9,0x05C9,0x05C8,0x05E8,0x05E8,0x05E7,0x05E7,0x05E6,
0x05E6,0x05E6,0x05E5,0x05E5,0x0604,0x0604,0x0604,0x0603,
0x0603,0x0602,0x0602,0x0601,0x0621,0x0621,0x0620,0x0620,
0x0620,0x0620,0x0E20,0x0E20,0x0E40,0x1640,0x1640,0x1E40,
0x1E40,0x2640,0x2640,0x2E40,0x2E60,0x3660,0x3660,0x3E60,
0x3E60,0x3E60,0x4660,0x4660,0x4E60,0x4E80,0x5680,0x5680,
0x5E80,0x5E80,0x6680,0x6680,0x6E80,0x6EA0,0x76A0,0x76A0,
0x7EA0,0x7EA0,0x86A0,0x86A0,0x8EA0,0x8EC0,0x96C0,0x96C0,
0x9EC0,0x9EC0,0xA6C0,0xAEC0,0xAEC0,0xB6E0,0xB6E0,0xBEE0,
0xBEE0,0xC6E0,0xC6E0,0xCEE0,0xCEE0,0xD6E0,0xD700,0xDF00,
0xDEE0,0xDEC0,0xDEA0,0xDE80,0xDE80,0xE660,0xE640,0xE620,
0xE600,0xE5E0,0xE5C0,0xE5A0,0xE580,0xE560,0xE540,0xE520,
0xE500,0xE4E0,0xE4C0,0xE4A0,0xE480,0xE460,0xEC40,0xEC20,
0xEC00,0xEBE0,0xEBC0,0xEBA0,0xEB80,0xEB60,0xEB40,0xEB20,
0xEB00,0xEAE0,0xEAC0,0xEAA0,0xEA80,0xEA60,0xEA40,0xF220,
0xF200,0xF1E0,0xF1C0,0xF1A0,0xF180,0xF160,0xF140,0xF100,
0xF0E0,0xF0C0,0xF0A0,0xF080,0xF060,0xF040,0xF020,0xF800};

// button variables used to detect capture button press
bool buttonState = false;
bool lastButtonState = !buttonState;

/* * * * * * * * * * * THERMAL CAMERA STUFF * * * * * * * * * * */
#define CAMERA_PORT 0 // use Port 0 for Thermal Camera Wireling

Adafruit_AMG88xx amg; // thermal camera object

float pixels[AMG88xx_PIXEL_ARRAY_SIZE]; // data array from thermal camera
uint16_t displayPixelWidth, displayPixelHeight;

// our IR camera uses an 8x8 resolution
const uint8_t xCameraRes = 8;
const uint8_t yCameraRes = 8;

float minimum = 0x7F7FFFFF; // maximum value for a float
float maximum = -273.15; // absolute zero (you won't get this cold, I promise)

#define MINTEMP 22 // low range of the sensor (this will be blue on the screen)
#define MAXTEMP 34 // high range of the sensor (this will be red on the screen)

#define backgroundColor TS_16b_Black

/* * * * * * * * * * * * * SD CARD STUFF * * * * * * * * * * * * */
#define SD_BYPASS 0 // set to 1 to bypass SD card features for debugging, set to 0 to use SD card

const int chipSelect = 10;    // CS pin used on microSD TinyShield

String directoryName;
int file = 0;
String fileName = String(file);
String filePath;
String fileExtension = ".csv"; // file type: .csv, .txt, ..., etc.

void setup() {
  SerialUSB.begin(9600);
  Wire.begin();
  display.begin();
  display.setBrightness(10);
  display.clearScreen();
  display.setFlip(0);
  display.setBitDepth(TSBitDepth16);
  display.setFont(thinPixel7_10ptFontInfo);
  display.fontColor(TS_16b_White, TS_16b_Black);

  if (!SD_BYPASS) {
    if (!SD.begin(chipSelect)) {
      width = display.getPrintWidth("No SD card detected.");
      display.setCursor(48-(width/2), 16);
      display.print("No SD card detected.");
      // don't do anything more:
      //while (1);
    } else {
      width = display.getPrintWidth("Card Initialized.");
      display.setCursor(48-(width/2), 16);
      display.print("Card Initialized.");
  
      makeNewDir(); // create the new directory for this session
    }
  }

  delay(900); // let user read message on screen
  
  displayPixelWidth = yMax / yCameraRes;
  displayPixelHeight = xMax / xCameraRes;

  pixelSize = findPixelSize(xMax, yMax);
  
  selectPort(CAMERA_PORT);
  amg.begin();
  delay(100); // let sensor boot up
}

void loop() {
  amg.readPixels(pixels);
  
  bool buttonState = display.getButtons(TSButtonLowerRight);
  if(buttonState != lastButtonState) { // this is the bootloader button
    if(buttonState) {
      display.setCursor(95 - display.getPrintWidth("00>"), 54);
      display.println(fileName + ">");
      
      bitmapToSD(prepData());
    } else {
      // nothing
    }
    delay(50); // prevent bouncing
  }
  lastButtonState = buttonState;
  
  getMinMax();
  drawBuffer();
}

void selectPort(int port) {
  Wire.beginTransmission(0x70);
  Wire.write(0x04 + port);
  Wire.endTransmission();
}

void getMinMax() {
  // Reinitialize our absolute maximum and minimum values.
  // (note that minimum is set to the largest possible value
  // and maximum is set to the smallest possible value in order
  // to satisfy the functional requirements of the if statements below)
  minimum = 0x7F7FFFFF; // maximum value for a float
  maximum = -273.15;    // absolute zero (you won't get this cold)

  // get minimum and maximum values
  for(int i = 0; i < AMG88xx_PIXEL_ARRAY_SIZE; i++) {
    float val = pixels[i];
    if(val > maximum)
      maximum = val;
    if(val < minimum)
      minimum = val;
  }

  // calculate the range as well
  float range = maximum - minimum;
  int minimumRange = 7;
  if(range < minimumRange) {
    minimum += (minimumRange - (maximum-minimum)) / 2;
    maximum += (minimumRange - (maximum-minimum)) / 2;
  }
}

// this just converts an RGB value to the BGR scheme used by the TinyScreen+
// the colors array can be setup better to avoid needing this step
uint16_t convertColor(uint16_t original) {
  uint16_t converted = 0x0000;

  converted |= (original & 0xF800) >> 11; // RED
  converted |= (original & 0x07E0);       // GREEN
  converted |= (original & 0x001F) << 11; // BLUE

  return converted;
}

// "pixels" on display will be n by n pixels, where n is the returned value
int findPixelSize(int screenXwidth, int screenYheight) {
  if(screenXwidth == screenYheight) { // screen is a square (convenient!)
    return screenXwidth / xCameraRes; // 8 is from the 8x8 thermal camera resolution
  } else if(screenXwidth < screenYheight){
    return screenXwidth / xCameraRes;
  } else {
    return screenYheight / yCameraRes;
  }
}

void drawBuffer() {                             // populates the buffer, then writes it to the screen
  for (int i = 0; i < 64 * 96; i++) {           // For every pixel in the buffer, write the background color first
    buffer[i * 2] = backgroundColor >> 8;
    buffer[i * 2 + 1] = backgroundColor;
  }

  // puts specified string into the buffer in the font and colors specified at the given coordinates
  putImage();

  display.goTo(0, 0);
  display.startData();                          // Activates the OLED driver chip to ready it for receiving commands (drives CS line HIGH)
  display.writeBuffer(buffer, 96 * 64 * 2);     // Write all of the pixel data (saved in buffer) to the screen (update what is being displayed on screen)
  display.endTransfer();                        // Deactivate the OLED driver chip now that it has been updated (drives CS pin LOW)
}

void putImage() {
  // get adjusted origin to center image
  int xOrigin = (xMax - (pixelSize * xCameraRes)) / 2; // use 8 since camera is 8x8 resolution
  int yOrigin = (yMax - (pixelSize * yCameraRes)) / 2; // use 8 since camera is 8x8 resolution
  uint16_t color = 0x00;

  int xCamera = 0;
  int yCamera = 0;
  int xScreen = 0;
  int yScreen = 0;
  int colorIndex = 0;
  
  for(int i = 0; i < (xCameraRes * yCameraRes); i++) {
    xCamera = i % xCameraRes;
    yCamera = (i / yCameraRes);
    xScreen = (xCamera * pixelSize) + xOrigin;
    yScreen = yCamera * pixelSize;

    // map temperature reading to the appropriate color array index
    colorIndex = map(pixels[yCamera * yCameraRes + xCamera], minimum, maximum, 0, 255);
    // colorIndex = map(pixels[yCamera * yCameraRes + xCamera], MINTEMP, MAXTEMP, 0, 255);
    // constrain array index between 0 and 255 so we do not surpass array bounds
    colorIndex = constrain(colorIndex, 0, 255);
    
    // set color to the converted value from the color array at the proper index found above
    color = convertColor(camColors[colorIndex]);

    // for each color we get, we must insert a pixelSize by pixelSize square onto the screen
    for(int j = 0; j < pixelSize; j ++) {
      for(int k = 0; k < pixelSize; k ++) {
        putPixel(buffer, xScreen + j, yScreen + k, color);
      }
    }
  }
}

// writes a 16-bit color to a specified pixel (x0, y0) within the specified buffer
void putPixel(uint8_t * buff, int x0, int y0, uint16_t color) {                                 
  x0 = constrain(x0, 0, xMax);            // constrains pixel's x-coordinate to the screen bounds
  y0 = constrain(y0, 0, yMax);            // constrains pixel's y-coordinate to the screen bounds
  buff[(y0 * 96 + x0) * 2] = color >> 8;  // set the buffer at appropriate index to first 8 bits of color
  buff[(y0 * 96 + x0) * 2 + 1] = color;   // set the buffer at the following index to the last 8 bits of color
}

// directories will be numbered and increment by 1 with each system boot
// each file will be a number and will incremeent by 1 with each capture
// this function gets the last directory entry and creates a new one at boot
void makeNewDir() {
  File root = SD.open("/");
  File entry;
  
  int i = 0; // start checking at directory name 0, 1, 2, 3, ...
  while (true) {
    // if this directory exists
    if(SD.exists(String(i))) {
      i++; // try next number
    } else {
      // make new directory
      directoryName = String(i);
      if(SD.mkdir(directoryName)) {
        break; // exit loop if directory creation is successful
      } else {
        // print error message if directory creation fails
        //SerialUSB.println("error creating directory");
        width = display.getPrintWidth("error creating directory");
        display.setCursor(48-(width/2), 16);
        display.print("error creating directory");
      }
    }
  }
}

// prepare the data in a String with .csv format
String prepData() {
  // make a string for assembling the data to log:
  String dataString = "";
  int xCamera;
  int yCamera;

  for(int i = 0; i < (xCameraRes * yCameraRes); i++) {
    xCamera = i % xCameraRes;
    yCamera = (i / yCameraRes);

    // add the next temperature reading
    dataString += pixels[i];
    
    if(xCamera == xCameraRes - 1) {
      dataString += '\n'; // add a newline if at the end of a line
    } else {
      dataString += ',';  // otherwise, add a comma to separate values
    }
  }

  return dataString;
}

// write the current thermal camera reading to a CSV file on the microSD card
void bitmapToSD(String dataToLog) {
  filePath = directoryName + "/" + fileName + fileExtension;
  
  if(SD_BYPASS) {
    SerialUSB.println(dataToLog); // for debugging
    SerialUSB.println("Written to: " + filePath);
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(filePath, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataToLog); // dump the entire string containing the array
    dataFile.close();

    file++;
    fileName = String(file); // increment the file name/number if write is successful
  } else {
    // if the file isn't open, print an error message
    SerialUSB.println("Error opening " + filePath);
  }
}
