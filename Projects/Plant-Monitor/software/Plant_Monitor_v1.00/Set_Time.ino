//  ------------------------------------------------------------
//  Set Time
//
//  Here is where date and time are set abd then transfered to the rtc
//  based on date_time_index var which is controlled by the button_set
//  in Update_Buttons
//  and is initialized in Setup() 
//
//byte  date_time_index;        //0 = display rtc date and time
//                              //1 = set month (not here)
//                              //2 = set day   (not here)
//                              //3 = set year  (not here)
//                              //4 = set am/pm/24hr
//                              //5 = set hours
//                              //6 = set minutes
//                              //7 = set seconds
//                              //8 = set rtc to above
//


//  ------------------------------------------------------------
//  set time

//  rtc.setHours(hours);
//  rtc.setMinutes(minutes);
//  rtc.setSeconds(seconds);

  // Set the date
//  rtc.setDay(day);
//  rtc.setMonth(month);
//  rtc.setYear(year);

  // you can use also
  //rtc.setTime(hours, minutes, seconds);
  //rtc.setDate(day, month, year);

//  ------------------------------------------------------------ 
 
void  set_time()
{

//button_set_action() changes the date_time_index variable as follows
    
    if (button_set_flag == 1)                   
    {
      button_set_flag = 0;
      limit_index     = 0; //if we're setting time then we can't set limits
      date_time_index = date_time_index + 1;

      if (date_time_index >= 1 && date_time_index <=3)
      {
        date_time_index = 4;  //skip over date in this version
      }

      if (date_time_index > 8)
      {
        date_time_index = 0;  //display the time
      }
    }


//now execute the button_set_action() from above

     switch (date_time_index)            
      {
         case 0:
          //display time
              display_time();       
          break;  
              
        case 1:
          //set month - not in this version
           break;
          
        case 2:
          //set day - not in this version
          break;
          
        case 3:
          //set year - not in this version
          break;
          
        case 4:
          set_am_pm_24h();    //set am/pm/24hr
          break;
          
        case 5:
          set_hours();        //set hours
          break;
          
        case 6:
          set_minutes();      //set minutes
          break;
          
        case 7:
          set_seconds();      //set seconds
          break;
          
        case 8:
          store_to_rtc();     //store above to rtc and set case = 0
          break;
          
        default:
          // if nothing else matches, do the default
          // default is optional
          date_time_index = 0;  //so just display time
        break;
      }
 }

//  ------------------------------------------------------------
//  set Am / Pm / 24
//
//  Here is where am, pm or 24hr is selected by the user
//  Once this is done the RTC is adjusted accordingly for hours
//  ------------------------------------------------------------  

void  set_am_pm_24h()
{  
  //See if the user changed the am, pm, 24
  
    if (button_plus_flag == 1 || button_minus_flag == 1)                
    {
      button_plus_flag  = 0;                          //clear the flags
      button_minus_flag = 0;

        am_pm_24 = am_pm_24 + 1;                      //increment the am_pm_24 counter
                                                      //0 = am
                                                      //1 = pm
                                                      //2 = 24
                                                        
        if (am_pm_24 > 2)                             //test for roll over
        {
          am_pm_24 = 0;
        }
    }

  //Now display what the user choose
  

        if (am_pm_24 == 0 && rtc.getSeconds()%2 == 0)
        {
          display.setCursor(am_pm_24_x,am_pm_24_y);//set cursor for am, pm or 24
          display.fontColor(TS_8b_Green,TS_8b_White); //am flash color
          display.print("Am");                        //display am           
        }
        
        if (am_pm_24 == 0 && rtc.getSeconds()%2 != 0)
        {
          display.setCursor(am_pm_24_x,am_pm_24_y);//set cursor for am, pm or 24
          display.fontColor(TS_8b_Green,TS_8b_Black); //am font colors
          display.print("Am");                        //display am           
        }

        if (am_pm_24 == 1 && rtc.getSeconds()%2 == 0)
        {
          display.setCursor(am_pm_24_x,am_pm_24_y);//set cursor for am, pm or 24
          display.fontColor(TS_8b_Blue,TS_8b_White); //pm flash colors
          display.print("Pm");                       //display pm          
        }               
        
        if (am_pm_24 == 1 && rtc.getSeconds()%2 != 0)
        {
          display.setCursor(am_pm_24_x,am_pm_24_y);//set cursor for am, pm or 24
          display.fontColor(TS_8b_Blue,TS_8b_Black); //pm font colors
          display.print("Pm");                       //display pm           
        }

        if (am_pm_24 == 2 && rtc.getSeconds()%2 == 0)
        {
          display.setCursor(am_pm_24_x,am_pm_24_y);//set cursor for am, pm or 24
          display.fontColor(TS_8b_Yellow,TS_8b_White); //24 flash colors
          display.print("24");                         //display 24          
        }
        
        if (am_pm_24 == 2 && rtc.getSeconds()%2 != 0)
        {
          display.setCursor(am_pm_24_x,am_pm_24_y);//set cursor for am, pm or 24
          display.fontColor(TS_8b_Yellow,TS_8b_Black); //24hr font colors
          display.print("24");                         //display 24           
        }

   //Adjust hours based on what the user chose for am/pm/24
   if (am_pm_24 != 2 && hours == 0)
   {
      hours = 1;         //we can't have zero hours for am or pm
   }
}


//  ------------------------------------------------------------
//  set hours
//  ------------------------------------------------------------  

void  set_hours()
{
  //First, display the am, pm or 24 as selected above
  
      display.setCursor(am_pm_24_x,am_pm_24_y);//set cursor for am, pm or 24 below

      if (am_pm_24 == 0)
      {
          display.fontColor(TS_8b_Green,TS_8b_Black); //am font colors
          display.print("Am");                        //display am          
      }

      if (am_pm_24 == 1)
      {
          display.fontColor(TS_8b_Blue,TS_8b_Black); //pm font colors
          display.print("Pm");                       //display pm         
      }

      if (am_pm_24 == 2)
      {
          display.fontColor(TS_8b_Yellow,TS_8b_Black); //24hr font colors
          display.print("24");                         //display 24          
      }

  //If we're using am or pm then hours can't be more than 12
      if (am_pm_24 != 2 && hours > 12)
      {
        hours = hours - 12;
      }

  //And based on the am, pm or 24 choice, flash the hour based on the button_plus and button_minus pushes

      if (am_pm_24 != 2)                  //am hours are from 12:00:00 am (midnight) to 11:59:59 am
      {                                   //pm hours are from 12:00:00 pm (noon)     to 11:59:59 pm
                                    
        if (button_plus_flag == 1)                     
        {
          button_plus_flag = 0;

          hours = hours + 1;
          if (hours > 12)
          {
            hours = 1;
          }
        }

        if (button_minus_flag == 1)
        {
          button_minus_flag = 0;

          hours = hours - 1;
          
          if (hours  > 200)   //byte roll over
          {
            hours = 12;
          }

          if (hours == 0)     //roll over from 23 to 00 in 24hr time
          {
            hours = 12;       //there is no "00" hours in am/pm
          }
        }
      }

     if (am_pm_24 == 2)                   //24hr are from 00:00:00 (midnight) to 23:59:59
      {                                  
        if (button_plus_flag == 1)                     
        {
          button_plus_flag = 0;

          hours = hours + 1;
          if (hours > 23)
          {
            hours = 0;
          }
        }

        if (button_minus_flag == 1)
        {
          button_minus_flag = 0;

          hours = hours - 1;
          if (hours > 200)    //byte roll over
          {
            hours = 23;
          }
        }
      }


      
      if (am_pm_24 == 0 && rtc.getSeconds()%2 == 0)
        {
          display.setCursor(hours_x,hours_y);         //set cursor for hours
          display.fontColor(TS_8b_Green,TS_8b_White); //am flash color
          print2digits(hours);                        //display hours          
        }
        
        if (am_pm_24 == 0 && rtc.getSeconds()%2 != 0)
        {
          display.setCursor(hours_x,hours_y);         //set cursor for hours
          display.fontColor(TS_8b_Green,TS_8b_Black); //am font colors
          print2digits(hours);                        //display hours          
        }

        if (am_pm_24 == 1 && rtc.getSeconds()%2 == 0)
        {
          display.setCursor(hours_x,hours_y);         //set cursor for hours
          display.fontColor(TS_8b_Blue,TS_8b_White); //pm flash colors
          print2digits(hours);                        //display hours            
        }               
        
        if (am_pm_24 == 1 && rtc.getSeconds()%2 != 0)
        {
          display.setCursor(hours_x,hours_y);         //set cursor for hours
          display.fontColor(TS_8b_Blue,TS_8b_Black); //pm font colors
          print2digits(hours);                        //display hours           
        }

        if (am_pm_24 == 2 && rtc.getSeconds()%2 == 0)
        {
          display.setCursor(hours_x,hours_y);         //set cursor for hours
          display.fontColor(TS_8b_Yellow,TS_8b_White); //24 flash colors
          print2digits(hours);                        //display hours           
        }
        
        if (am_pm_24 == 2 && rtc.getSeconds()%2 != 0)
        {
          display.setCursor(hours_x,hours_y);         //set cursor for hours
          display.fontColor(TS_8b_Yellow,TS_8b_Black); //24hr font colors
          print2digits(hours);                        //display hours           
        }
}
 
 

//  ------------------------------------------------------------
//  set minutes
//  ------------------------------------------------------------ 

void  set_minutes()
{
 //First, display hours as selected above 
      display.setCursor(hours_x,hours_y);             //set cursor for hours
       
      if (am_pm_24 == 0)
      {
          display.fontColor(TS_8b_Green,TS_8b_Black); //am font colors
          print2digits(hours);                        //display hours         
      }

      if (am_pm_24 == 1)
      {
          display.fontColor(TS_8b_Blue,TS_8b_Black); //pm font colors
          print2digits(hours);                       //display hours         
      }

      if (am_pm_24 == 2)
      {
          display.fontColor(TS_8b_Yellow,TS_8b_Black); //24hr font colors
          print2digits(hours);                         //display hours          
      }

 //Next, adjust minutes per user input 
 
       if (button_plus_flag == 1)                     
        {
          button_plus_flag = 0;

          minutes = minutes + 1;
          if (minutes > 59)
          {
            minutes = 0;
          }
        }

       if (button_minus_flag == 1)                     
        {
          button_minus_flag = 0;

          minutes = minutes - 1;
          if (minutes > 200)          //roll over
          {
            minutes = 59;
          }
        }
        
 //Finally, display the minutes

      
      
      if (am_pm_24 == 0 && rtc.getSeconds()%2 == 0)
        {
          display.setCursor(minutes_x,minutes_y);     //set cursor for minutes
          display.fontColor(TS_8b_Green,TS_8b_White); //am flash color
          print2digits(minutes);                      //display minutes          
        }
        
        if (am_pm_24 == 0 && rtc.getSeconds()%2 != 0)
        {
          display.setCursor(minutes_x,minutes_y);     //set cursor for minutes
          display.fontColor(TS_8b_Green,TS_8b_Black); //am font colors
          print2digits(minutes);                      //display minutes         
        }

        if (am_pm_24 == 1 && rtc.getSeconds()%2 == 0)
        {
          display.setCursor(minutes_x,minutes_y);     //set cursor for minutes
          display.fontColor(TS_8b_Blue,TS_8b_White); //pm flash colors
          print2digits(minutes);                     //display minutes           
        }               
        
        if (am_pm_24 == 1 && rtc.getSeconds()%2 != 0)
        {
          display.setCursor(minutes_x,minutes_y);     //set cursor for minutes
          display.fontColor(TS_8b_Blue,TS_8b_Black); //pm font colors
          print2digits(minutes);                     //display minutes          
        }

        if (am_pm_24 == 2 && rtc.getSeconds()%2 == 0)
        {
          display.setCursor(minutes_x,minutes_y);     //set cursor for minutes
          display.fontColor(TS_8b_Yellow,TS_8b_White); //24 flash colors
          print2digits(minutes);                       //display minutes          
        }
        
        if (am_pm_24 == 2 && rtc.getSeconds()%2 != 0)
        {
          display.setCursor(minutes_x,minutes_y);     //set cursor for minutes
          display.fontColor(TS_8b_Yellow,TS_8b_Black); //24hr font colors
          print2digits(minutes);                       //display minutes           
        }

}



//  ------------------------------------------------------------
//  set seconds
//  ------------------------------------------------------------  

void  set_seconds()
{
 //First, display minutes as selected above 
      display.setCursor(minutes_x,minutes_y);         //set cursor for minutes
       
      if (am_pm_24 == 0)
      {
          display.fontColor(TS_8b_Green,TS_8b_Black); //am font colors
          print2digits(minutes);                      //display minutes         
      }

      if (am_pm_24 == 1)
      {
          display.fontColor(TS_8b_Blue,TS_8b_Black); //pm font colors
          print2digits(minutes);                     //display minutes         
      }

      if (am_pm_24 == 2)
      {
          display.fontColor(TS_8b_Yellow,TS_8b_Black); //24hr font colors
          print2digits(minutes);                       //display minutes          
      }

 //Next, adjust seconds per user input 
 
       if (button_plus_flag == 1)                     
        {
          button_plus_flag = 0;

          seconds = seconds + 1;
          if (seconds > 59)
          {
            seconds = 0;
          }
        }

       if (button_minus_flag == 1)                     
        {
          button_minus_flag = 0;

          seconds = seconds - 1;
          if (seconds > 200)          //roll over
          {
            seconds = 59;
          }
        }
        
 //Finally, display the seconds

      
      
      if (am_pm_24 == 0 && rtc.getSeconds()%2 == 0)
        {
          display.setCursor(seconds_x,seconds_y);     //set cursor for seconds
          display.fontColor(TS_8b_Green,TS_8b_White); //am flash color
          print2digits(seconds);                      //display seconds          
        }
        
        if (am_pm_24 == 0 && rtc.getSeconds()%2 != 0)
        {
          display.setCursor(seconds_x,seconds_y);     //set cursor for seconds
          display.fontColor(TS_8b_Green,TS_8b_Black); //am font colors
          print2digits(seconds);                      //display seconds          
        }

        if (am_pm_24 == 1 && rtc.getSeconds()%2 == 0)
        {
          display.setCursor(seconds_x,seconds_y);     //set cursor for seconds
          display.fontColor(TS_8b_Blue,TS_8b_White); //pm flash colors
          print2digits(seconds);                     //display seconds            
        }               
        
        if (am_pm_24 == 1 && rtc.getSeconds()%2 != 0)
        {
          display.setCursor(seconds_x,seconds_y);     //set cursor for seconds
          display.fontColor(TS_8b_Blue,TS_8b_Black); //pm font colors
          print2digits(seconds);                     //display seconds          
        }

        if (am_pm_24 == 2 && rtc.getSeconds()%2 == 0)
        {
          display.setCursor(seconds_x,seconds_y);     //set cursor for seconds
          display.fontColor(TS_8b_Yellow,TS_8b_White); //24 flash colors
          print2digits(seconds);                       //display seconds           
        }
        
        if (am_pm_24 == 2 && rtc.getSeconds()%2 != 0)
        {
          display.setCursor(seconds_x,seconds_y);     //set cursor for seconds
          display.fontColor(TS_8b_Yellow,TS_8b_Black); //24hr font colors
          print2digits(seconds);                       //display seconds          
        }   
}



//  ------------------------------------------------------------
//  store to rtc
//  ------------------------------------------------------------ 

void store_to_rtc()
{
    // Set the date
//  rtc.setDay(day);
//  rtc.setMonth(month);
//  rtc.setYear(year);

//  Before going on adjust am/pm hours to 24hr for storage to rtc

    if (am_pm_24 == 0)  //am
    {
      if (hours == 12)
      {
        hours = 0;
      }
    }

    if (am_pm_24 == 1) //pm
    {
      if (hours < 12)
      {
        hours = hours + 12;
      }
    }

     // Set the time
  rtc.setHours(hours);
  rtc.setMinutes(minutes);
  rtc.setSeconds(seconds);

  date_time_index = 0;    //this will allow the rtc to be displayed next time thru

  display.drawRect(hours_x,hours_y,63,10,TSRectangleFilled ,TS_8b_White); //flash the time area to let user know it's set
  delay(200);
  display.drawRect(hours_x,hours_y,63,10,TSRectangleFilled ,TS_8b_Black);  

  clear_lux_array();      //clear lux array to start fresh

  // you can use also
  //rtc.setTime(hours, minutes, seconds);
  //rtc.setDate(day, month, year); 
}

 
