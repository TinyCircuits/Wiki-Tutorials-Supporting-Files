#include <Arduino.h>
#define TS(x) _BV(x)

const int numSensors = 6;


class capTouch {
  public:

    int capTouchCal[numSensors];
    int capTouchCurrent[numSensors];
    uint8_t overCalCount[numSensors];

    uint8_t primary = 0;
    uint8_t isTouch = 0;
    uint8_t initialTouchPos = 0;
    uint8_t finalTouchPos = 0;
    uint8_t currentTouchedPositions = 0;
    uint8_t allTouchedPositions = 0;
    uint32_t touchTimer = 0;
    
    capTouch();
    void begin();
    int scaleToRange(int range);
    int getMaxReading();
    int getMagnitude();
    uint8_t read();
    uint32_t duration();
    unsigned int getCapReading(uint8_t pin);
    unsigned int capRead(uint8_t pin);

};



