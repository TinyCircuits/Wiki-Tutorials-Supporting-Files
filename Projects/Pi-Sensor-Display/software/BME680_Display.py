from busio import I2C
import adafruit_bme680
import time
import board
import tinycircuits_wireling

# For OLED
import busio
from digitalio import DigitalInOut
from PIL import Image, ImageDraw, ImageFont
import adafruit_ssd1306

# Initialize and enable power to Wireling Pi Hat
wireling = tinycircuits_wireling.Wireling()
OLED96_port = 0
BME680_port = 1
wireling.selectPort(BME680_port)

# Create library object using our Bus I2C port
i2c = busio.I2C(board.SCL, board.SDA)
bme680 = adafruit_bme680.Adafruit_BME680_I2C(i2c, 0x76)

# change this to match the location's pressure (hPa) at sea level
bme680.sea_level_pressure = 1013.25

# A reset line may be required if there is no auto-reset circuitry
reset_pin = DigitalInOut(board.D10)

# use the 0.96" OLED Screen
wireling.selectPort(OLED96_port)
display = adafruit_ssd1306.SSD1306_I2C(128, 64, i2c, addr=0x3c, reset=reset_pin) # 0.96" Screen

# Load a font
font = ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf', 14)

while True:
    # Select the sensor port to get data
    wireling.selectPort(BME680_port)

    tmpC = bme680.temperature
    tmpF = (tmpC * 1.8) + 32

    tmp = "Temp: %0.1f F" % tmpF
    voc = "Gas: %d ohm" % bme680.gas
    hum = "Hum: %0.1f %%" % bme680.humidity
    prs = "Pres: %0.3f hPa" % bme680.pressure
    alt = "Alt: %0.2f m" % bme680.altitude

    # Select the OLED port for displaying data
    wireling.selectPort(OLED96_port)

    # Create blank image for drawing.
    display.fill(0)
    image = Image.new('1', (display.width, display.height))
    draw = ImageDraw.Draw(image)

    # Draw the text
    draw.text((0, 0), tmp, font=font, fill=255)
    draw.text((0, 12), voc, font=font, fill=255)
    draw.text((0, 24), hum, font=font, fill=255)
    draw.text((0, 36), prs, font=font, fill=255)
    draw.text((0, 48), alt, font=font, fill=255)

    # Display image
    display.image(image)
    display.show()
    time.sleep(.25)