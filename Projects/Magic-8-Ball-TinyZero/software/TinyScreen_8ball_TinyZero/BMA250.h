#ifndef BMA250_h
#define BMA250_h

#define BMA250_I2CADDR 0x18
#define BMA250_update_time_64ms 0x08
#define BMA250_update_time_32ms 0x09
#define BMA250_update_time_16ms 0x0A
#define BMA250_update_time_8ms 0x0B
#define BMA250_update_time_4ms 0x0C
#define BMA250_update_time_2ms 0x0D
#define BMA250_update_time_1ms 0x0E
#define BMA250_update_time_05ms 0xF
#define BMA250_range_2g 0x03
#define BMA250_range_4g 0x05
#define BMA250_range_8g 0x08
#define BMA250_range_16g 0x0C
 
#include <inttypes.h>

class BMA250 {
public:
BMA250();
int begin(uint8_t, uint8_t);
void read();
int16_t X,Y,Z;
int8_t rawTemp;
int8_t tempC;
uint8_t I2Caddress=0;
};
#endif
