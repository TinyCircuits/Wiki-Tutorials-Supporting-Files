//  ------------------------------------------------------------
//  Get Measurements
//  ------------------------------------------------------------

void  get_measurements()
{
    get_soil_1_moisture();
    get_soil_2_moisture();
    get_lux_level();
}


//  ------------------------------------------------------------
//  Get Soil 1 Moisture from soil sensor via I2C
//  ------------------------------------------------------------

void  get_soil_1_moisture()
{
//  Moisture Sensor Stuff
//  from tinycircuits.com 

  Wireling.selectPort(SOIL_1_PORT);
  soil_1_moisture_percent = readMoisture();
    
//  moistureAverage.addValue(readMoisture()); // get moisture
//  moistureLevel = moistureAverage.getAverage();
//  soil_1_moisture_percent = moistureLevel;
/////////  soil_1_temp             = tempLevel;
}


//  ------------------------------------------------------------
//  Get Soil 2 Moisture from soil sensor via I2C
//  ------------------------------------------------------------

void  get_soil_2_moisture()
{
//  Moisture Sensor Stuff
//  from tinycircuits.com 

  Wireling.selectPort(SOIL_2_PORT);
  soil_2_moisture_percent = readMoisture(); 
  
//  moistureAverage.addValue(readMoisture()); // get moisture
//  moistureLevel = moistureAverage.getAverage();  
//  soil_2_moisture_percent = moistureLevel;
//////////  soil_2_temp             = tempLevel;
}

//  ------------------------------------------------------------
//  Get LUX Level from light sensor TSL2572 via I2C
//  from tinycircuits.com
//  ------------------------------------------------------------

void  get_lux_level()
{
//  Light Sensor Stuff
//  from tinycircuits.com 
  Wireling.selectPort(LUX_PORT);
  TSL2572Init(GAIN_16X);
  luxAverage.addValue(Tsl2572ReadAmbientLight());
  luxLevel = luxAverage.getAverage();
  luxAverage.fillValue(Tsl2572ReadAmbientLight());
  
///  float AmbientLightLux = Tsl2572ReadAmbientLight();
  lux = Tsl2572ReadAmbientLight();
}


//  ------------------------------------------------------------
//  TSL2572nit(uint8_t gain)
//  from tinycircuits.com
//  ------------------------------------------------------------

// Initializes the light sensor to be ready for output
void TSL2572Init(uint8_t gain) 
{
  Tsl2572RegisterWrite( 0x0F, gain );//set gain
  Tsl2572RegisterWrite( 0x01, 0xED );//51.87 ms
  Tsl2572RegisterWrite( 0x00, 0x03 );//turn on
  if (GAIN_DIVIDE_6)
    Tsl2572RegisterWrite( 0x0D, 0x04 );//scale gain by 0.16
  if (gain == GAIN_1X)gain_val = 1;
  else if (gain == GAIN_8X)gain_val = 8;
  else if (gain == GAIN_16X)gain_val = 16;
  else if (gain == GAIN_120X)gain_val = 120;
}

//  ------------------------------------------------------------
//  Tsl2572RegisterWrite( byte regAddr, byte regData )
//  from tinycircuits.com
//  ------------------------------------------------------------
void Tsl2572RegisterWrite( byte regAddr, byte regData )
{
  Wire.beginTransmission(TSL2572_I2CADDR);
  Wire.write(0x80 | regAddr); 
  Wire.write(regData);
  Wire.endTransmission(); 
}

//  ------------------------------------------------------------
//  Tsl2572ReadAmbientLight()
//  from tinycircuits.com
//  ------------------------------------------------------------

float Tsl2572ReadAmbientLight()
{     
  uint8_t data[4]; 
  int c0,c1;
  float lux1,lux2,cpl;

  Wire.beginTransmission(TSL2572_I2CADDR);
  Wire.write(0xA0 | 0x14);
  Wire.endTransmission();
  Wire.requestFrom(TSL2572_I2CADDR,4);
  for(uint8_t i=0;i<4;i++)
    data[i] = Wire.read();
     
  c0 = data[1]<<8 | data[0];
  c1 = data[3]<<8 | data[2];
  
  //see TSL2572 datasheet
  cpl = 51.87 * (float)gain_val / 60.0;
  if(GAIN_DIVIDE_6) cpl/=6.0;
  lux1 = ((float)c0 - (1.87 * (float)c1)) / cpl;
  lux2 = ((0.63 * (float)c0) - (float)c1) / cpl;
  cpl = max(lux1, lux2);
  return max(cpl, 0.0);
}


//  ------------------------------------------------------------
//  Read Moisture from soil sensors
//  from tinycircuits.com
//  ------------------------------------------------------------
int readMoisture()
{
  Wire.beginTransmission(0x30);   //0x30 is the hex address for the moisture sensor
  Wire.write(1);
  Wire.endTransmission();
  delay(5);
  int c=0;
  Wire.requestFrom(0x30, 2);
    if(Wire.available()==2)
    { 
      c = Wire.read();
      c <<= 8;
      c |= Wire.read();
      c = constrain(c, MINCAPREAD, MAXCAPREAD);
      c = map(c, MINCAPREAD, MAXCAPREAD, 0, 100);
    }
  return c;
}

//  ------------------------------------------------------------
//  Read Temperature from soil sensors
//  from tinycircuits.com
//  ------------------------------------------------------------
float readTemp() 
{
  Wire.beginTransmission(0x30);
  Wire.write(2);
  Wire.endTransmission();
  delay(5);
  int c = 0;
  Wire.requestFrom(0x30, 2);
    if (Wire.available() == 2)
    {
      c = Wire.read();
      c <<= 8;
      c |= Wire.read();
      //https://learn.adafruit.com/thermistor/using-a-thermistor thanks!
      uint32_t adcVal = ANALOGREADMAX - c;
      uint32_t resistance = (SERIESRESISTOR * ANALOGREADMAX) / adcVal - SERIESRESISTOR;
      float steinhart = (float)resistance / THERMISTORNOMINAL;     // (R/Ro)
      steinhart = log(steinhart);                  // ln(R/Ro)
      steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
      steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
      steinhart = 1.0 / steinhart;                 // Invert
      steinhart -= 273.15;                         // convert to C
      return steinhart;
    }
  return c;
}
