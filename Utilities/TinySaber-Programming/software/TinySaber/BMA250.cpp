#include "BMA250.h"
#include <Arduino.h>
#include <util/delay.h>

#define SDA_PORT PORTA
#define SDA_PIN 7
#define SCL_PORT PORTA
#define SCL_PIN 6

#define I2C_CPUFREQ 8000000
#define I2C_FASTMODE 1
#include "SoftI2CMaster.h"

BMA250::BMA250()
{
}

void BMA250::begin(uint8_t range, uint8_t bw)
{
  //enable I2C pullups
  PUEA |= (_BV(7) | _BV(6));
  i2c_init();
  _delay_ms(5);
  //Setup the range measurement setting
  i2c_start(BMA250_I2CADDR | I2C_WRITE);
  i2c_write(BMA250_G_RANGE);
  i2c_write(range);
  i2c_stop();
  _delay_ms(1);
  //Setup the bandwidth
  i2c_start(BMA250_I2CADDR | I2C_WRITE);
  i2c_write(BMA250_BANDWIDTH);
  i2c_write(bw);
  i2c_stop();
  _delay_ms(1);
  //Setup the double tap interrupt
  i2c_start(BMA250_I2CADDR | I2C_WRITE);
  i2c_write(0x2A);
  i2c_write((1 << 6)  | (1 << 2) | (1 << 0));
  i2c_stop();
  _delay_ms(1);
  i2c_start(BMA250_I2CADDR | I2C_WRITE);
  i2c_write(BMA250_TAP_TH);
  i2c_write(BMA250_tap_threshold);
  i2c_stop();
  _delay_ms(1);
  i2c_start(BMA250_I2CADDR | I2C_WRITE);
  i2c_write(BMA250_INT_EN);
  i2c_write(BMA250_d_tap_en);
  i2c_stop();
  _delay_ms(1);
  clearInterrupt();
}

int BMA250::clearInterrupt()
{
  i2c_start(BMA250_I2CADDR | I2C_WRITE);
  i2c_write(BMA250_INT_MODE);
  i2c_write(BMA250_int_latch_and_reset);
  i2c_stop();
  doubleTap = 0;
  return 0;
}

int BMA250::read()
{
  //Set register index
  i2c_start(BMA250_I2CADDR | I2C_WRITE);
  i2c_write(0x02);
  i2c_stop();
  //Request seven data bytes
  i2c_start(BMA250_I2CADDR | I2C_READ);
  //Receive acceleration measurements as 16 bit integers
  X = i2c_read(false);
  X |= i2c_read(false) << 8;
  Y = i2c_read(false);
  Y |= i2c_read(false) << 8;
  Z = i2c_read(false);
  Z |= i2c_read(false) << 8;
  //The following shifts preserve the sign bits
  X >>= 6; Y >>= 6; Z >>= 6;
  //Receive temperature measurement
  rawTemp = i2c_read(false);
  uint8_t tapInt = i2c_read(true);
  i2c_stop();

  if (tapInt & BMA250_d_tap_int)
    doubleTap = 1;

  /*
    long sum = pow(X, 2);
    sum += pow(Y, 2);
    sum += pow(Z, 2);
    A = sqrt(sum);
    if (A == 0 || A == 1)
    return 1;
  */
  return 0;
}
