/*
  Buzzer & RGB LED Christmas Ornament

  This project is assembled with the following Wirelings:
  PORT 0: Buzzer Wireling
  PORT 1: RGB LED Wireling
  PORT 2: 
  PORT 3: Rotary Switch Wireling

  By rotating the Rotary Switch Wireling, the
  song played by the Buzzer Wireling will change.
  An RGB LED will cycle through a few pre-defined
  colors every 1.5 seconds.

  Hardware by: TinyCircuits
  Written by: Hunter Hykes for TinyCircuits

  Initialized: 12/16/2020
  Last Updated: 12/18/2020
*/

#include <Wireling.h> // For interfacing with Wirelings
#include <SX1505.h>   // For interfacing with Rotary Switch Wireling
#include <FastLED.h>  // For RGB LED
#include "pitches.h"  // Definitions of notes as frequencies
#include "songs.h"    // Definitions of pitches and note durations for each song

// Universal Serial Monitor Config
#if defined(ARDUINO_ARCH_AVR)
#define SerialMonitorInterface Serial
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#endif

/* * * * * * * * * * Buzzer * * * * * * * * * */
#define buzzerPin A0 // Corresponds to Wireling Port 0 (A1 = Port 1, A2 = Port 2, A3 = Port 3)

/* * * * * * * * * * RGB LED * * * * * * * * * */
#define LED_PIN A1
#define NUM_LEDS 1 // This is the number of RGB LEDs connected to the pin
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
int numClrs = 4; // number of colors listed in the following colors[] array
CRGB colors[] = {
  CRGB(255,   0,   0), // RED
  CRGB(  0, 255,   0), // GREEN
  CRGB(127, 127,   0), // YELLOW
  CRGB(  0,   0, 255)  // BLUE
}; // colors to cycle through with the LED
int clr = 0; // counter to iterate through colors
int brightness = 64; // Brightness is on a scale of 0-255, 64 is 25% brightness
unsigned long LED_time = 1.5 * 1000; // LED will change every 1.5 seconds
unsigned long time_start;

/* * * * * * * * * * Rotary * * * * * * * * * */
#define ROT_PORT 3
#define ROT_INT A3
TinyRotary rotary = TinyRotary(); 
uint8_t rotaryValue = 0;
bool changeSong = 0;

void setup(void) {
  Wireling.begin();

  /* * * * * * Buzzer Init * * * * */
  pinMode(buzzerPin, OUTPUT); // Buzzer
  
  /* * * * * * RGB LED Init * * * * */
  FastLED.addLeds<WS2812, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);
  pinMode(LED_PIN, OUTPUT);
  
  /* * * * * * Rotary Init * * * * */
  Wireling.selectPort(ROT_PORT); 
  rotary.begin();
  rotary.setInterrupt(); // configure the interrupt settings for the onboard SX1505
  pinMode(ROT_INT, INPUT_PULLUP); // pullup since active low, keep pulled high if idle
  attachInterrupt(ROT_INT, ISR_Rotary, FALLING); // interrupt is active low, react on falling edge

  SerialMonitorInterface.begin(9600);
  time_start = millis(); // initialize the start time of the timer
}

void loop() {
  if(changeSong) { // if the rotary value changes
    // SerialMonitorInterface.println(rotaryValue); // print the value acquired by the ISR
    changeSong = 0;           // we are done changing songs, allow the new one to play
    delay(100);               // short delay so change in song is noticeable
    play_song(rotaryValue);   // sing the new song
  }

  updateLED(); // keep updating LED even if no song is playing
}

void play_song(int s) {
  // 0 = off
  switch(s) {
    case 1:
      SerialMonitorInterface.println("'Jingle Bells'");
      sing(jingle_melody, jingle_tempo, 104);
      break;
    case 2:
      SerialMonitorInterface.println("'We Wish You a Merry Christmas'");
      sing(wish_melody, wish_tempo, 52);
      break;
    case 3:
      SerialMonitorInterface.println("'Santa Claus is Coming to Town'");
      sing(santa_melody, santa_tempo, 112);
      break;
    case 4:
      SerialMonitorInterface.println("'Rudolph the Red-Nosed Reindeer'");
      sing(rudolph_melody, rudolph_tempo, 147);
      break;
    case 5:
      SerialMonitorInterface.println("'Carol Of The Bells'");
      sing(cotb_melody, cotb_tempo, 87);
      break;
    case 6:
      SerialMonitorInterface.println("'Deck the Halls'");
      sing(deck_melody, deck_tempo, 68);
      break;
    case 7:
      SerialMonitorInterface.println("'God Rest Ye Merry Gentlemen'");
      sing(rest_melody, rest_tempo, 68);
      break;
    case 8:
      SerialMonitorInterface.println("'O Christmas Tree'");
      sing(tree_melody, tree_tempo, 62);
      break;
    case 9:
      SerialMonitorInterface.println("'The First Noel'");
      sing(noel_melody, noel_tempo, 73);
      break;
    case 0:
      // off for now
      break;
    default:
      // do nothing
      break;
  }
}

// play the specified song on the Buzzer Wireling
// continues to update the LED every (LED_time) seconds
void sing(int* melody, int* tempo, int numNotes) {
  // Iterate over the notes of the melody:
  for (int thisNote = 0; (thisNote < numNotes) && (!changeSong); thisNote++) {
    // To calculate the note duration, take one second divided by the note type.
    // Ex. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / tempo[thisNote];
    tone(buzzerPin, melody[thisNote], noteDuration);
  
    // To distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(8); // Stop the tone playing

    updateLED(); // update LED while song is playing too
  }
}

// returns true when the specified interval (in ms) has passed
bool ms_timer(unsigned long interval) {
  unsigned long time_now = millis();
  if (millis() >= time_start + interval) { // if the timer is expired
    time_start = millis(); // set the start time for the next interval
    return 1;
  } else {
    return 0;
  }
}

// change the color of the RGB LED every (LED_time) seconds
// by iterating through the colors in the colors[] array
void updateLED() {
  if(ms_timer(LED_time)) {
    // uncomment for loop if using more than one RGB LED in the strand
    // for(int i = 0; i < NUM_LEDS; i++) {
      leds[0] = colors[clr];
      FastLED.show();
    // }
    clr++; // increment colors[] array iterator
    if(clr == numClrs) {
      // reset clr to 0 if clr exceeds the size of the color array
      clr = 0;
    }
  }
}

// interrupt handler for the Rotary Switch Wireling
void ISR_Rotary() {
  changeSong = 1;                     // let the program know to change the song
  rotaryValue = rotary.getPosition(); // get the new value
  rotary.clearInt();                  // clear the interrupt flag
}
