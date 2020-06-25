//  Plant Monitor_v1.00
//  copyright LearnOnLine, inc
//  author:  John Gavlik
//           with contributions from tinycircuits.com (noted within)
//  Created  12/30/2019
//  Updated  02/09/2020

//
//--------------Program Description ---------
//
//  The Plant Monitor program is a graphical presentation of 2 soil monitor wirelings and 1 ambient light wireling
//  on the TinyScreen+ color OLED
//  It also uses a buzzer to alert the user to low moisture and low light levels
//  Hardware:
//            TinyScreen+ (1)
//            Wireling Adapter (1)
//              Soil Monitor Wireling (2)
//              Ambient Light Sensor Wireling (1)
//              Buzzer Wireling (1)
//                1 beep  - soil #1 sensor low
//                2 beeps - soil #2 sensor low
//                3 beeps - low light level
//            Custom plastic enclosure (1)
//            Assorted Wireling cables (4)
//            USB cable (micro to A) (1)
//


//
//--------------Libraries---------------------
//

#include <Wire.h>
#include <SPI.h>
#include <TinyScreen.h>
#include <RTCZero.h>
#include <Wireling.h>

//Library must be passed the board type
//TinyScreenPlus for TinyScreen+
TinyScreen display = TinyScreen(TinyScreenPlus);

//
//--------------General variables---------------------
//--------------Global for Debugging------------------
//
byte    tempByte;


//
//--------------RTC variables---------------------
//--------------Global for Debugging------------------
//
/* Create an rtc object */
RTCZero rtc;

/* Change these values to set the current initial date */
byte  day                = 1;   //just some values that arn't displayed
byte  month              = 1;
byte  year               = 20;

/* Change these values to set the current initial time */
byte  seconds;                  //these time values are over written by user setting time
byte  minutes;                  //including 24hr or am/pm
byte  hours;                    //see set_time() for details

                                //For set_time()
byte  am_pm_24           = 0;   //0 = am, 1 = pm, 2 = 24hr

//These are the x/y coordinates for time on the OLED 96x x 64y screen
const byte  hours_x      = 18;  //OLED x for hours display
const byte  hours_y      = 0;   //OLED y for hours display

const byte  minutes_x    = 34;  //OLED x for minutes display
const byte  minutes_y    = 0;   //OLED y for minutes display

const byte  seconds_x    = 49;  //OLED x for seconds display
const byte  seconds_y    = 0;   //OLED y for seconds display

const byte  am_pm_24_x   = 64;  //OLED x for am/pm/24hr display
const byte  am_pm_24_y   = 0;   //OLED y for am/pm/2rhr display

//
//--------------OLED Variables---------------------
//--------------Global for Debugging------------------
//
byte  pixel;                        //horizontal pixel from 0 to 95
byte  line;                         //vertical line from 0 to 63

//
//--------------Time Variables---------------------
//--------------Global for Debugging------------------
//
int   loop_time;                   //time around the main Loop set in Setup
int   loop_time_increment;         //number of milliseconds for each loop - set in Setup

//
//--------------Measurement Variables---------------------
//--------------Global for Debugging------------------
//

const byte  soil_1_start_x = 3;    //horizontal pixel starting point for soil 1 icon
const byte  soil_2_start_x = 78;   //horizontal pixel starting point for soil 2 icon

byte   soil_1_moisture_percent;    //soil 1 moisture percent (0-99)  
byte   soil_2_moisture_percent;    //soil 2 moisture percent (0-99) 

byte   soil_moisture_percent;      //used in draw_soil_icon()
byte   soil_moisture_percent_moving; //used in draw_soil_icon() to convert line # to soil moisture percent

byte   soil_1_temp;                //soil 1 temperature in degrees C
byte   soil_2_temp;                //soil 2 temperature in degrees C


int   lux;                         //immediate lux value (0-60K)
int   lux_ave_value;               //average lux value for storage to the lux_array @ lux_ptr

int   lux_ave_divisor;             //count of the number of times lux is averaged
int   lux_to_y_value;              //conversion from lux value to y-position on plot

 
const byte lux_x_plot_min = 25;    //min and max x/y for Lux line plots
const byte lux_x_plot_max = 72;   
const byte lux_y_plot_min = 62;    //this is no mistake 
const byte lux_y_plot_max = 24;    //this is no mistake 

            

//
//--------------Limit Variables---------------------
//--------------Global for Debugging------------------
//

byte   limit_index;                //used in set_limits()
                                   //0 = do nothing
                                   //1 = inc/dec soil_1 limit line    - reload limit_counter
                                   //2 = inc/dec soil_2 limit_line    - reload limit_counter
                                   //3 = inc/dec max lux for plotting - reload limit_counter
                         

byte   limit_counter;             //used to time set_limit() aciivity
                                  //automatically gets out of set_limit() when limit_counter == 0
                                  //counted down each time thru and reloaded
                                  //for every button increment/decrement
                                  
const byte  limit_counter_reload = 10*4;  //10 second reload time based on 4 updates/second

const byte  soil_moisture_limit_min = 20; //min and max soil_moisture_limit values for limit_lines
const byte  soil_moisture_limit_max = 80;

byte   soil_1_moisture_limit;     //minimum to activate buzzer - set in setup() - can be changed in set_limits()
byte   soil_2_moisture_limit;     //minimum to activate buzzer - set in setup() - can be changed in set_limits()

int    soil_1_moisture_limit_counter; //counts all occurrances of soil_1_moisture_percent below soil_1_moisture_limit - buzzer()
int    soil_2_moisture_limit_counter; //counts all occurrances of soil_2_moisture_percent below soil_2_moisture_limit - buzzer()

byte   soil_moisture_limit;       //used in draw_soil_icon()
byte   limit_line;                //white line in soil icon to denote boundry between green and red


const int lux_top_plot_level_min = 10;    //min top lux
const int lux_top_plot_level_max = 1000;  //max top lux

int    lux_top_plot_level;        //level to use for top of lux plot - set in set_limits()
int    total_lux_this_day;        //lux_top_level_plot_level times the sum of all y_values in lux_array



//
//---------------Array Variables-----------------------
//--------------Global for Debugging------------------
//

int    lux_array[48];             //light array to store 12 hours of light levels @ 15 min/sample
byte   lux_array_ptr;             //pointer to lux_array[]

byte   lux_array_ptr_last;        //last pointer value for computing average lux 
const byte lux_plot_offset = 24;  //add this to the lux_array_ptr for plotting update_plot_area()

const byte   lux_array_ptr_min = 0;  //min and max array pointers
const byte   lux_array_ptr_max = 47;

byte   mp_lux_array_ptr;          //mp is for MakerPlot - see output_serial_data() 

//
//--------------Button Variables---------------------
//--------------Global for Debugging------------------
//
                              //see Button_Actions for details
                                  
byte  buttons_get;            //value for this sample of buttons
byte  buttons_last;           //last button_get value
const byte  buttons_none = 0; //no buttons pushed value
                               
const byte  button_set   = TSButtonUpperRight;  //set time
const byte  button_clr   = TSButtonLowerRight;  //set soil moisture / lux limits and clear buzzzer 
const byte  button_plus  = TSButtonUpperLeft;   //increment time/limit value
const byte  button_minus = TSButtonLowerLeft;   //decrement time/limit value


byte  button_set_counter;      //single button counters
byte  button_clr_counter;         
byte  button_plus_counter;       
byte  button_minus_counter;


const byte  push_action = 0;  //counter value to trigger push action
const byte  hold_action = 3;  //counter value to trigger hold action
                              //based on loop_time = 250 milliseconds

bool  hold_action_flag;       //0 = cleared, 1 when hold_action value from above is exceeded
                              //used to clear button counters when all buttons are released
                              //so that inadvertant "push" does not occur

                              
byte  date_time_index;        //0 = display date and time
                              //1 = set month
                              //2 = set day
                              //3 = set year
                              //4 = set am/pm/24hr
                              //5 = set hours
                              //6 = set minutes
                              //7 = set seconds
                              //8 = set rtc to above and set date_time_index = 0

bool  button_set_flag;        //set in button_set_action()    cleared in set_time()
bool  button_clr_flag;        //set in button_clr_action()    cleared in update_buzzer()
bool  button_plus_flag;       //set in button_plus_action()   cleared in set_time()
bool  button_minus_flag;      //set in button_minus_action()  cleared in set_time()
                              
                                                            
//
//---------------Wireling Port and A2D pins-----------------
//               from tinycircuits.com
//                                                        relative to  display.setFlip(false);                              
#define LUX_PORT      3       //light sensor                                        (far-right)
                              //the soil ports are swapped to make up for where
                              //they are physically 
#define SOIL_1_PORT   1       //soil 1 moisture/temp monitor                        (top-left
#define SOIL_2_PORT   2       //soil 2 moisture/temp monitor                        (top- right)
#define pin (uint8_t) A0      //buzzer  - uses A0 - no port as it is not addressed  (far-left)


//
//---------------Light Sensor Variables-----------------
//               from tinycircuits.com
//
#define TSL2572_I2CADDR     0x39    //hex address of light sensor
#define GAIN_1X             0       //gain levels
#define GAIN_8X             1
#define GAIN_16X            2
#define GAIN_120X           3

#define GAIN_DIVIDE_6       true    //only use this with 1x and 8x gain settings
int gain_val              = 0;      //initial gain setting

float luxLevel;                     //lux as a float here

//
//---------------Moisture Sensor Variables-----------------
//               from tinycircuits.com
//
#define MINCAPREAD          710
#define MAXCAPREAD          975
#define ANALOGREADMAX       1023
#define THERMISTORNOMINAL   10000
#define TEMPERATURENOMINAL  25
#define BCOEFFICIENT        3380
#define SERIESRESISTOR      35000

int moistureLevel;
float tempLevel;

//
//---------------Buzzer Variables-----------------
//

byte  buzzer_flag;                  //0 = OFF, 1 = ON

const byte  buzzer_start_time = 16; //24hr start time for buzzer if below minimums - set_limits()
const byte  buzzer_stop_time  = 17; //24hr stop  time for buzzer if below minimums - set_limits()

int   lux_total;                   //total lux count for each element in lux_array 
const int lux_buzzer_min = 48*5;   //lux_total must be greater than this to avoid the buzzer beeping - see update_buzzer()     

//#define pin (uint8_t) A0         //buzzer  - uses A0 - no port as it is not addressed  (far-right)
const unsigned int soil_1_frequency      = 2700;     
const unsigned int soil_2_frequency      = 3000;
const unsigned int lux_level_frequency   = 3300;
unsigned int   frequency;          //frequency choice from above
const unsigned long duration = 200;//tone duration


//  ------------------------------------------------------------
//  Simple templated averaging class based on Running Average 
//  by Rob Tillaart: http://arduino.cc/playground/Main/RunningAverage
//  ------------------------------------------------------------

template <const unsigned int N>
class RunningAverageFloat
{
  public:
    void addValue(float val) 
    {
      _ar[_index] = val;
      _index++;
      if (_index == N) _index = 0;
    };
    void fillValue(float val) 
    {
      for (unsigned int i = 0; i < N; i++)_ar[i] = val;
    };
    float getAverage() 
    {
      float sum = 0.0;
      for (unsigned int i = 0; i < N; i++)sum += _ar[i];
      return sum / (float)N;
    };
  protected:
    int _index = 0;
    float _ar[N];
};

RunningAverageFloat<35> moistureAverage;
RunningAverageFloat<35> temperatureAverage;
RunningAverageFloat<35> luxAverage;


//
//---------------Setup-----------------------
//

void setup() 
{
  display.begin();                //OLED settings
  display.setBrightness(10);
  display.setFlip(false);        //flip the OLED display                          
  display.setFont(thinPixel7_10ptFontInfo);
//  display.setFont(liberationSans_10ptFontInfo);
//  display.setFont(liberationSans_14ptFontInfo);
//  display.setFont(liberationSans_16ptFontInfo);
//  display.setFont(liberationSansNarrow_8ptFontInfo);
//  display.setFont(liberationSansNarrow_10ptFontInfo);
//  display.setFont(liberationSansNarrow_12ptFontInfo);
//  display.setFont(liberationSansNarrow_14ptFontInfo);
//  display.setFont(liberationSansNarrow_16ptFontInfo);
//  display.setFont(liberationSansNarrow_22ptFontInfo);

  Wire.begin();
  Wireling.begin();               // Enable power & select port
  delay(200);                     // boot sensor
    
                                  //fillValue on light and moisture sensors
  Wireling.selectPort(LUX_PORT);
  TSL2572Init(GAIN_16X);
  luxAverage.fillValue(Tsl2572ReadAmbientLight());

  Wireling.selectPort(SOIL_1_PORT);
  moistureAverage.fillValue(readMoisture());
  temperatureAverage.fillValue(readTemp());

  Wireling.selectPort(SOIL_2_PORT);
  moistureAverage.fillValue(readMoisture());
  temperatureAverage.fillValue(readTemp());




  
  
  SerialUSB.begin(9600);          //set the serial USB port to 9600 baud
  rtc.begin();                    //initialize RTC

  hours   = rtc.getHours();       // Get the time
  minutes = rtc.getMinutes();
  seconds = rtc.getSeconds();

  display.clearScreen();          //clear the entire screen
  clear_lux_plot_area();          //and the lux plot area
                                  //plus draw a white rectangle around it
  clear_lux_array();              //and clear the lux array that contains data for it


  lux_top_plot_level  = lux_top_plot_level_min; //can be adjusted in set_lux_limit() 
  
  button_set_flag     = 0;        //clear the button flags
  button_clr_flag     = 0;
  button_plus_flag    = 0;
  button_minus_flag   = 0;

  soil_1_moisture_limit = 35;     //buzzer below this value - can be changed in set_limits()
  soil_2_moisture_limit = 35;     //buzzer below this value - can be changed in set_limits()

  limit_index         = 0;        //initial state of set_limits() switch case
  buzzer_flag         = 1;        //buzzer is initially ON



  date_time_index     = 0;        //show the time
  lux_array_ptr_last  = 99;       //so lux plot will reset immediately

  mp_lux_array_ptr    = lux_array_ptr_max; //mp stands for "MakerPlot"
                                  //this ensures that mp_lux_array_ptr will start at lux_array_ptr_min
                                  //see output_serial_data() for details

  initialize_lux_plot_area();     //initialize the lux plot area
  update_lux_plot_area();         //update the lux plot area
  
  loop_time_increment = 250;      //250 milliseconds per loop
  loop_time           = millis(); //initial loop_time before we begin the loop (next)  
}

//
//---------------Loop-----------------------
//

void loop() 
{
    while (millis() < loop_time + loop_time_increment)
      {
      //   //wait until loop_time_increment has elapsed before going on
      }
     loop_time = millis();       //re-initialize loop_time  
//
//  IMPORTANT!.................................................
//    Keep functions below in the order as they are now
//  IMPORTANT!.................................................

  get_measurements();       //get soil moisture and light level values
  update_plot_area();       //update plot area     
  update_buttons();         //update button conditions
  set_time();               //set and display the time  
  set_limits();             //set soil moisture and light threshold limits
  update_buzzer();          //update buzzer                           
  output_serial_data();     //output serial data via USB port to MakerPlot software
///  debug();                  //-----------remove----------     
}
