/*************************************************************************
 * Vbatt reading Tutorial:
 * Calculates the Voltage of a Battery for a TinyCircuit processor board
 * 
 * Hardware by: TinyCircuits
 * Code by: Ben Rose for TinyCircuits
 *
 * Initiated: Mon. 6/20/2018 
 * Updated: Tue. 07/03/2018
 ************************************************************************/

#include <TinyScreen.h> 

TinyScreen display = TinyScreen(TinyScreenPlus);
int background = TS_16b_Black;

void setup() {
  SerialUSB.begin(9600);
  while (!SerialUSB);

  // Setup for the TinyScreen
  display.begin();
  display.setBrightness(15); //Full brightness
  display.setFlip(true);
  display.clearScreen();
  display.setFont(thinPixel7_10ptFontInfo);
}

void loop() {
  // Get battery voltage and print to Serial Monitor
  float battVoltageReading = getBattVoltage();
  SerialUSB.print(battVoltageReading);

  // Print to TinyScreen+
  display.fontColor(TS_8b_Blue, background); // Because blue is cool
  display.setCursor(0, 16);
  display.print("Batt V = ");
  display.print(battVoltageReading);
  
  SerialUSB.println('\t');
  delay(1000);
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