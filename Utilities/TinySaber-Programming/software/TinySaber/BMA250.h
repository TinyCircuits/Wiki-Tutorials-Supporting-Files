#ifndef BMA250_h
#define BMA250_h

#include <Arduino.h>

#define BMA250_I2CADDR 0x18<<1
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
#define BMA250_int_latch_and_reset 0x8F
#define BMA250_tap_threshold 0x08
#define BMA250_d_tap_int 0x10
#define BMA250_d_tap_en 0x10

#define BMA250_G_RANGE 0x0F
#define BMA250_BANDWIDTH 0x10
#define BMA250_INT_MODE 0x21
#define BMA250_INT_EN 0x16
#define BMA250_TAP_TH 0x2B

class BMA250 {
  public:
    BMA250();
    void begin(uint8_t range, uint8_t bw);
    int clearInterrupt();
    int read();
    int X, Y, Z, A, doubleTap, rawTemp;
};

#endif //BMA250_h
