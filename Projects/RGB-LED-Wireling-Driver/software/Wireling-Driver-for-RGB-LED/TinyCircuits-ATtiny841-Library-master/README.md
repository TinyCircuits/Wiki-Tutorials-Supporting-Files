# TinyCircuits ATtiny841 Library

This Arduino library is intended for use with TinyCircuits' **[ASD2302 Dual Motor Driver TinyShield](https://tinycircuits.com/collections/motors-1/products/dual-motor-tinyshield)**, **[ASD2303 Servo Controller TinyShield](https://tinycircuits.com/collections/motors-1/products/servo-tinyshield)**, and **[AST1003 Capacitive Touch Slider Wireling](https://tinycircuits.com/collections/wireling-sensors/products/capacitive-touch-slider-wireling)**. 

These boards use a ATtiny841 microcontroller with firmware that allows the 16 bit PWM outputs to be controlled through I²C. 
The Arduino library itself allows for easy interface using the TinyDuino/Wireling platform. Included with the library is the ATtiny841 firmware-
it is intended to be hackable and allow for extra functionality to be implemented(full ATtiny841 register access through I²C).

*Support this library by buying products from **[TinyCircuits](https://tinycircuits.com/)***


## CapTouchWireling Class

This class inherits ATtiny841Lib functions and implements specific functionality to use inputs for capacitive touch pad inputs:

* **uint8_t begin()** Initialize the firmware, returns 0 on success 
* **bool update()** Update all sensor values, return true if a finger was previously present but is no longer present(detect a tap) 
* **bool isTouched()** Return the current status of the sensor, true when a finger is present
* **uint16_t capTouchRead(uint8_t pin)** Return the capacitive touch reading on input *pin*(pads 0 through 5)
* **int16_t getPosition(int range=100)** If a finger is present, return the position with a value of 0 to *range*(default 100)
* **int16_t getMaxReading()** If a finger is present, return the maximum sensor reading
* **int16_t getMagnitude()** If a finger is present, return the magnitude of all sensor readings
* **uint32_t duration()** If a finger is present return the current duration it has been present. If no longer present, return the total time of the previous tap


## ServoDriver Class

This class inherits ATtiny841Lib functions and implements specific functionality to set the PWM registers to control hobby servos:

* **uint8_t begin(uint16_t PWMperiod)** Initialize the firmware with a specific PWM period(typically 20000), returns 0 on success.
* **void useResetPin(uint8_t pin)** Tell the library what pin to use for the ATtiny841 reset signal. *Note: This signal is only used on the Servo Driver TinyShield*
* **void setServo(uint8_t servo, int16_t val)** Set *servo* (1, 2, 3, 4) PWM to *val*(0 to +PWMperiod, but typically 1000 to 2000)
* **void sleep()** Powerdown the ATtiny841 until another command is sent


## MotorDriver Class

This class inherits ATtiny841Lib functions and implements specific functionality to set the PWM registers to control the motor drivers:

* **uint8_t begin(uint16_t PWMperiod)** Initialize the firmware with a specific PWM period, returns 0 on success
* **void setMotor(uint8_t motor, int16_t val)** Set *motor*(1 or 2) PWM to *val*(-PWMperiod to +PWMperiod)
* **void sleep()** Powerdown the ATtiny841 until another command is sent


## ATtiny841Lib Class

This base class implements communication functionality with the ATtiny841 firmware and handles generic command writing and reading: 

* **void writeByte(uint8_t b1(, uint8_t b2(, uint8_t b3)))** Transfer a byte sequence to the ATtiny841 firmware, up to 3 bytes
* **void writeCommand(uint8_t cmd, uint16_t val(, uint16_t val1(, uint16_t val2(, uint16_t val3))))** Write a command sequence to the ATtiny841 firmware, passing up to four unsigned integer arguments
* **uint8_t read(uint8_t reg)** return the byte read at register *reg* on the ATtiny841
* **void setFailsafe(uint16_t ms)** set the failsafe timeout in ms. If this time elapses without a command being received, the ATtiny841 will stop the PWM outputs
* **void setTimerPrescaler(uint8_t prescaler)** Set the timer prescaler for the PWM outputs
* **void powerDown()** Tell the ATtiny841 firmware to enter powerdown mode
