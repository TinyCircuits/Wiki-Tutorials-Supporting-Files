//*******************************************************************
// TinyCircuits - A1454 Allegro Linear Hall-Effect Sensor Library 
// Written by: Ben Rose for TinyCircuits 
//
// Initiated: Wed. 8/ 8/2017 @  2:30PM 
//   Updated: Jan 2020
//*******************************************************************

#include <avr/pgmspace.h>
#include <stdlib.h> 
#include <Arduino.h> 
#include <Wire.h> 
#include "TinyCircuits_A1454.h"

//Constructor 
TinyCircuits_A1454::TinyCircuits_A1454() {
  //Nothing Here 
}

//Begins I2C Communication
void TinyCircuits_A1454::begin() {

}

//Read the 12-bit signed Magnetic Field Value 						
int16_t TinyCircuits_A1454::readMag() { 
	int16_t x; int16_t y; 
	
	Wire.beginTransmission(A1454_ADDRESS); 				
	Wire.write(A1454_OUTPUT); 								
	Wire.endTransmission(); 								
	
	Wire.requestFrom(A1454_ADDRESS, 4);						
	Wire.read(); 
	Wire.read();
	x = Wire.read();
	y = Wire.read(); 
	
  x <<= 12; 
  x |= (y << 4); 
  x >>= 4;
	return x; 												
}

//Read the 12-bit signed Junction Temperature Value 
int16_t TinyCircuits_A1454::readTemp() {
	int16_t x; int16_t y; 
	
	Wire.beginTransmission(A1454_ADDRESS); 					
	Wire.write(A1454_TEMPOUT); 								
	Wire.endTransmission(); 								
	
	Wire.requestFrom(A1454_ADDRESS, 4);						
	Wire.read(); 
	Wire.read(); 
	x = Wire.read();
	y = Wire.read(); 

  x <<= 12; 
  x |= (y << 4); 
  x >>= 4;
  
	x /= 8; // temperature offset
	x += 25; 
	return x; 												
}

//Check awake or sleep status 
uint8_t TinyCircuits_A1454::readMode(void) {
	Wire.beginTransmission(A1454_ADDRESS); 					
	Wire.write(A1454_SLEEP); 								
	Wire.endTransmission(); 								
	
	Wire.requestFrom(A1454_ADDRESS, 4);						
	Wire.read(); 
	Wire.read(); 
	Wire.read();
	
	return Wire.read(); 											
}
