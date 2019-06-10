//**************************************************************
// Tiny Arcade Game Tutorial - Tiny Brick LITE
// Written by: Brandon Farmer for TinyCircuits
// Initiated:  Mon. 3/27/2017 @  4:00pm
//   Updated: Thur. 3/30/2017 @  2:30pm
//**************************************************************
#include <TinyScreen.h>
#include <Wire.h>
#include <SPI.h>
#include "TinyArcade.h"
#include "GameTutorialSprites.h"

TinyScreen display = TinyScreen(TinyScreenPlus);

typedef struct {
  int x;
  int y;
  int width;
  int height;
  int collisions;
  const unsigned int *bitmap;
} ts_sprite;

ts_sprite ball = {offscreen, offscreen, 4, 4, 0, ballBitmap};
ts_sprite platform = {41, 56, 14, 4, 0, platformBitmap};
ts_sprite gameLogo = {offscreen, offscreen, 94, 21, 0, tinyBrickLogoBitmap};
ts_sprite lives = {offscreen, offscreen, 16, 2, 0, livesBitmap};

ts_sprite redBrick1 = { 1,  1, 10, 4, 0, redBrickBitmap};
ts_sprite redBrick2 = {13,  1, 10, 4, 0, redBrickBitmap};
ts_sprite redBrick3 = {25,  1, 10, 4, 0, redBrickBitmap};
ts_sprite redBrick4 = {37,  1, 10, 4, 0, redBrickBitmap};
ts_sprite redBrick5 = {49,  1, 10, 4, 0, redBrickBitmap};
ts_sprite redBrick6 = {61,  1, 10, 4, 0, redBrickBitmap};
ts_sprite redBrick7 = {73,  1, 10, 4, 0, redBrickBitmap};
ts_sprite redBrick8 = {85,  1, 10, 4, 0, redBrickBitmap};

ts_sprite yelBrick1 = { 1,  7, 10, 4, 0, yellowBrickBitmap};
ts_sprite yelBrick2 = {13,  7, 10, 4, 0, yellowBrickBitmap};
ts_sprite yelBrick3 = {25,  7, 10, 4, 0, yellowBrickBitmap};
ts_sprite yelBrick4 = {37,  7, 10, 4, 0, yellowBrickBitmap};
ts_sprite yelBrick5 = {49,  7, 10, 4, 0, yellowBrickBitmap};
ts_sprite yelBrick6 = {61,  7, 10, 4, 0, yellowBrickBitmap};
ts_sprite yelBrick7 = {73,  7, 10, 4, 0, yellowBrickBitmap};
ts_sprite yelBrick8 = {85,  7, 10, 4, 0, yellowBrickBitmap};

ts_sprite grnBrick1 = { 1, 13, 10, 4, 0, greenBrickBitmap};
ts_sprite grnBrick2 = {13, 13, 10, 4, 0, greenBrickBitmap};
ts_sprite grnBrick3 = {25, 13, 10, 4, 0, greenBrickBitmap};
ts_sprite grnBrick4 = {37, 13, 10, 4, 0, greenBrickBitmap};
ts_sprite grnBrick5 = {49, 13, 10, 4, 0, greenBrickBitmap};
ts_sprite grnBrick6 = {61, 13, 10, 4, 0, greenBrickBitmap};
ts_sprite grnBrick7 = {73, 13, 10, 4, 0, greenBrickBitmap};
ts_sprite grnBrick8 = {85, 13, 10, 4, 0, greenBrickBitmap};

int backgroundColor = TS_16b_Black;
unsigned int frame = 0;
int ballDirection = 2;
int score = 0;
bool start = 0;
int gameLives = 3;
const uint16_t ALPHA = 0x1111;
int offscreen = -100;

int numBricks = 24;
int amtSprites = 28;
ts_sprite * spriteList[28] = {&redBrick1, &redBrick2, &redBrick3, &redBrick4, &redBrick5, &redBrick6, &redBrick7, &redBrick8,
                              &yelBrick1, &yelBrick2, &yelBrick3, &yelBrick4, &yelBrick5, &yelBrick6, &yelBrick7, &yelBrick8,
                              &grnBrick1, &grnBrick2, &grnBrick3, &grnBrick4, &grnBrick5, &grnBrick6, &grnBrick7, &grnBrick8,
                              &ball, &platform, &gameLogo, &lives,
                             };


unsigned long frameStart = 0;
unsigned long frameTime = 18;

void setup() {
  arcadeInit();
  display.begin();
  display.setBitDepth(TSBitDepth16);
  display.setBrightness(15);
  display.setFlip(false);

  USBDevice.init();
  USBDevice.attach();
  SerialUSB.begin(9600);
}

void loop() {
  unsigned long timer = millis();                          
  while (millis() - frameStart < frameTime) {              
    delay(1);                                      
  }
  frameStart = millis();                                    
  showLogo();
  drawBuffer();
  frame++;
  movePlatform();
  checkPushbuttons();
  moveBall();
  ballWallCollision();
  ballBrickCollision();
  ballPlatformCollision();
  if (score == numBricks) resetBricks();
  if (gameLives == 0) resetBricks();
  timer = millis() - timer;  
}

void drawBuffer() {                                                             //Routine for drawing the pixel data to the screen
  uint8_t lineBuffer[96 * 64 * 2];                                              //8-bit integer named lineBuffer, equivalent to twice the number of pixels comprising the OLED (For 16-bit colors?)
  display.startData();                                                          //Activates the OLED driver chip to ready it for receiving commands (drives CS line HIGH)
  for (int y = 0; y < 64; y++) {                                                //Increments through the y-axis of the screen pixel-by-pixel
    for (int b = 0; b < 96; b++) {                                              //Increments throught the x-axis of the screen pixel-by-pixel. (Nested for loop using variable b)
      lineBuffer[b * 2] = backgroundColor >> 8;                                 //Loads the lineBuffer variable byte by byte (>>8 is bitshift Right 8 bits, or, load bits, then move them 8 positions over)
      lineBuffer[b * 2 + 1] = backgroundColor;                                  //Because we start at 0, we must also ensure to set the very last bit to the specified color
    }
    for (int spriteIndex = 0; spriteIndex < amtSprites; spriteIndex++) {        //Increment through each sprite and draw them to the screen
      ts_sprite *cs = spriteList[spriteIndex];                                  //The pointer (named cs) will store the ADDRESSES of the spriteList items
      if (y >= cs->y && y < cs->y + cs->height) {                               //This if statement compares the active pixel's y value to the y values of the active sprite
        int endX = cs->x + cs->width;                                           //The integer named endX is set equal to the active sprite's xValue+Width (The xValue on the rightmost side of the Sprite)
        if (cs->x < 96 && endX > 0) {                                           //This if statement checks to see if the active sprite's left side is on the screen and if the active sprite's right side is on the screen (No part is off screen)
          int xBitmapOffset = 0;                                                //Initializes the variable named xBitmapOffset equal to 0
          int xStart = 0;                                                       //Initializes the variable named xStart equal to 0
          if (cs->x < 0) xBitmapOffset -= cs->x;                                //If the x coordinate value of the active sprite is less than 0, set xBitmapOffset equal to itself - the x coordinate value of the active bitmap
          if (cs->x > 0) xStart = cs->x;                                        //If the x coordinate value of the active sprite is greater than 0, set xStart equal to the x coordinate value of the active bitmap
          int yBitmapOffset = (y - cs->y) * cs->width;                          //Initializes the variable named yBitmapOffset to (the yValue of the active pixel - the y-coordinate value of the active sprite * the width of the active sprite)
          for (int x = xStart; x < endX; x++) {                                 //Draws the screen starting at the lowest x-coordinate value and ending at the greatest x value
            unsigned int color = cs->bitmap[xBitmapOffset + yBitmapOffset++];   //The color to write to the active pixel is equal to the color of that pixel of the active sprite
            if (color != ALPHA) {                                               //As long as the color is NOT ALPHA, write that color to the lineBuffer byte by byte. ALPHAs will end up being the background color
              lineBuffer[(x) * 2] = color >> 8;                                 //Load byte, shift right, load byte, shift right, and so on...
              lineBuffer[(x) * 2 + 1] = color;                                  //Write the very last pixel to its specified color
            }
          }
        }
      }
    }
    display.writeBuffer(lineBuffer, 96 * 2);                                    //Write all of the pixel data (saved in lineBuffer) to the screen (update what is being displayed on screen)
  }
  display.endTransfer();                                                        //Deactivate the OLED driver chip now that it has been updated (drives CS pin LOW)
}

void showLogo() {
  if (!start) {
    gameLogo.x = -94; gameLogo.y = 23;
    for (int i = 0; i <= 94; i++) {
      gameLogo.x += 1;
      drawBuffer();
    }
    while (!checkButton(TAButton1)) {
      delay(1);
    }
    for (int i = 0; i <= 100; i++) {
      gameLogo.x += 1;
      drawBuffer();
    }
    start = 1;
    ball.x = random(40, 48);
    ball.y = random(38, 46);
    ballDirection = 2;
    lives.x = 89; lives.y = 61;
    drawBuffer();
    delay(100);
  }
}

void restart() {
  if (gameLives > 0) {
    platform.x = 41; platform.y = 56;
    start = 1;
    ball.x = random(40, 48);
    ball.y = random(38, 46);
    ballDirection = 2;
    drawBuffer();
    delay(1000);
  }
}

void movePlatform() {
  //Gets Joystick Data and moves platform accordingly
  if ((platform.x + platform.width) < 95) {
    if (checkJoystick(TAJoystickRight)) platform.x += 1;
  }
  if (platform.x > 0) {
    if (checkJoystick(TAJoystickLeft))  platform.x -= 1;
  }
}

void checkPushbuttons() {
  //Gets Pushbutton Data, and resets brick locations and game variables
  //if (checkButton(TAButton1)) ball.x = 0;
  if (checkButton(TAButton2)) resetBricks();
}

#define zmax(a,b) ((a)>(b)?(a):(b))
#define zmin(a,b) ((a)<(b)?(a):(b))

bool testBitmapCollision(ts_sprite *s1, ts_sprite *s2) {
  if ((s1->x < s2->x + s2->width) && (s1->x + s1->width > s2->x))
    if ((s2->y < s1->y + s1->height) && (s2->y + s2->height > s1->y))
      return true;
  return false;
}

bool testPixelCollision(ts_sprite *s1, ts_sprite *s2) {
  if (!testBitmapCollision(s1, s2))return false;
  int startX = zmax(s1->x, s2->x);
  int endX = zmin(s1->x + s1->width, s2->x + s2->width);
  int startY = zmax(s1->y, s2->y);
  int endY = zmin(s1->y + s1->height, s2->y + s2->height);
  for (int y = startY; y < endY; y++) {
    for (int x = startX; x < endX; x++) {
      if (s1->bitmap[(y - s1->y)*s1->width + (x - s1->x)] != ALPHA && s2->bitmap[(y - s2->y)*s2->width + (x - s2->x)] != ALPHA)
        return true;
    }
  }
  return false;
}

void ballBrickCollision() {
  for (int i = 0; i < numBricks; i++) {
    ts_sprite *brick = spriteList[i];
    if (testPixelCollision(&ball, brick)) {
      brick->x = offscreen;
      score += 1;
      ballBrickBounce(brick);
    }
  }
}

void ballBrickBounce(ts_sprite *brick) {
  if (ballDirection == 0) ballDirection = 1;
  if (ballDirection == 1) ballDirection = 0;
  if (ballDirection == 2) ballDirection = 1;
  if (ballDirection == 3) ballDirection = 0;
}

void moveBall() {
  if (ballDirection == 0) {
    ball.x -= 1;
    ball.y += 1;
  } else if (ballDirection == 1) {
    ball.x += 1;
    ball.y += 1;
  } else if (ballDirection == 2) {
    ball.x += 1;
    ball.y -= 1;
  } else if (ballDirection == 3) {
    ball.x -= 1;
    ball.y -= 1;
  }
}

void ballWallCollision() {
  if (ball.x <= 0 && ballDirection == 0) ballDirection = 1;
  else if (ball.x <= 0 && ballDirection == 3) ballDirection = 2;
  else if ((ball.x + ball.width) >= 95 && ballDirection == 1) ballDirection = 0;
  else if ((ball.x + ball.width) >= 95 && ballDirection == 2) ballDirection = 3;
  else if (ball.y <= 0 && ballDirection == 2) ballDirection = 1;
  else if (ball.y <= 0 && ballDirection == 3) ballDirection = 0;
  else if (ball.y >= 64) {
    gameLives -= 1;
    lives.x += 4;
    restart();
  }
  //else if ((ball.y + ball.height) >= 63 && ballDirection == 0) ballDirection = 3;
  //else if ((ball.y + ball.height) >= 63 && ballDirection == 1) ballDirection = 2;
}

void ballPlatformCollision() {
  if ( ((ball.y + ball.height) == (platform.y)) && ((ball.x <= platform.x + platform.width) && (ball.x + ball.width >= platform.x)) ) {
    if (ballDirection == 0) ballDirection = 3;
    if (ballDirection == 1) ballDirection = 2;
  }
}

void resetBricks() {
  ball.x = offscreen;
  lives.x = offscreen;
  platform.x = 41; platform.y = 56;
  gameLives = 3;
  score = 0;
  start = 0;
  int rstX[] = {1, 13, 25, 37, 49, 61, 73, 85};
  int rstY[] = {1, 7, 13};
  for (int i = 0; i <= numBricks; i++) {
    ts_sprite *rst = spriteList[i];
    if (i <= (numBricks / 3) - 1) {
      rst->x = rstX[i];
      rst->y = rstY[0];
    } else if (i >= (numBricks / 3) && i <= (2 * (numBricks / 3)) - 1) {
      rst->x = rstX[i - (numBricks / 3)];
      rst->y = rstY[1];
    } else if (i >= (2 * (numBricks / 3)) && i <= (numBricks - 1)) {
      rst->x = rstX[i - (2 * (numBricks / 3))];
      rst->y = rstY[2];
    }
  }
}