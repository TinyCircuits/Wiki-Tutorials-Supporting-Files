import board
import busio
import tinycircuits_wireling
from digitalio import DigitalInOut
from PIL import Image, ImageDraw, ImageFont
import adafruit_ssd1306
from time import sleep
import requests
import datetime

# Create the I2C interface.
i2c = busio.I2C(board.SCL, board.SDA)
# A reset line may be required if there is no auto-reset circuitry
reset_pin = DigitalInOut(board.D10) # reset pin for Port0

# Initialize and enable power to Wireling Pi Hat
wireling = tinycircuits_wireling.Wireling()
OLED96_port = 0
wireling.selectPort(OLED96_port)

# Create OLED Display Class
display = adafruit_ssd1306.SSD1306_I2C(128, 64, i2c, addr=0x3c, reset=reset_pin) # 0.96" Screen
 
# Initialize display.
display.fill(0)
display.show()
 
#font = ImageFont.load_default()
font = ImageFont.truetype('/home/pi/.fonts/Minecraftia.ttf', 8)
bigfont = ImageFont.truetype('/home/pi/.fonts/Minecraftia.ttf', 10)

while True:
  print("Updating...")
  # request forecast data via ThingSpeak -- be sure to use your own URLs here!
  weather = requests.get('https://api.thingspeak.com/apps/thinghttp/send_request?api_key=BBO9M818XFR8E7XC')
  # print("got weather")
  tempHi = requests.get('https://api.thingspeak.com/apps/thinghttp/send_request?api_key=XF2TASIILNX6Q0FR')
  tempHiTime = requests.get('https://api.thingspeak.com/apps/thinghttp/send_request?api_key=6I4B4MKJ1Y8TKS4K')
  # print("got tempHi")
  tempLo = requests.get('https://api.thingspeak.com/apps/thinghttp/send_request?api_key=G3AV1R6XCHOSA0UL')
  tempLoTime = requests.get('https://api.thingspeak.com/apps/thinghttp/send_request?api_key=VGYR2JM4SY317PNN')
  # print("got tempLo")
  precip = requests.get('https://api.thingspeak.com/apps/thinghttp/send_request?api_key=DBT6HQUSKR4SRNW7')
  # print("got precip")
  hum = requests.get('https://api.thingspeak.com/apps/thinghttp/send_request?api_key=98UW9FEAVW8PC99W')
  # print("got hum")
  wind = requests.get('https://api.thingspeak.com/apps/thinghttp/send_request?api_key=Q08VJ2Y9VEEYXRG2')
  # print("got wind")
  time = datetime.datetime.now()

  # Create blank image for writing to screen
  display.fill(0)
  image = Image.new('1', (display.width, display.height))
  draw = ImageDraw.Draw(image)

  # CONDITION
  print ("Weather: " + weather.text)
  draw.text((0, 0), weather.text, font=font, fill=255)

  # TEMPERATURE
  print ("Temperature (Lo/Hi): " + tempLo.text + " @" + tempLoTime.text + " / " + tempHi.text + " @" + tempHiTime.text)
  draw.text((0, 20), tempLo.text + " @" + tempLoTime.text + " / " + tempHi.text + " @" + tempHiTime.text, font=font, fill=255)

  # PRECIPITATION
  print ("Precipitation: " + precip.text + "in")
  draw.text((0, 30), "Precip: " + precip.text + "in", font=font, fill=255)

  # HUMIDITY
  print ("Humidity: " + hum.text + "%")
  draw.text((0, 40), "Hum: " + hum.text + "%", font=font, fill=255)

  # WIND
  print ("Wind: " + wind.text + "mph")
  draw.text((64, 40), "Wind: " + wind.text + "mph", font=font, fill=255)
 
  # TIME STAMP
  print ("Last Updated " + time.strftime("%b %d %H:%M:%S"))
  draw.text((0, 54), "Updated " + time.strftime("%b %d %H:%M:%S"), font=font, fill=255)

  # update OLED display
  display.image(image)
  display.show()

  sleep(60) # update every 60s