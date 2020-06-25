//  ------------------------------------------------------------
//  Display Time
//  ------------------------------------------------------------

void display_time()
{
  // The rtc runs on 24hr time so we must adjust hours if am or pm is set
  
      display.setCursor(am_pm_24_x,am_pm_24_y);      //set cursor for am, pm or 24 below         
                                                    
     if (am_pm_24 != 2)                              //test for am, pm, 24hr
     {                                               //if here it's am or pm

        if (rtc.getHours() == 0)                     //midnight
        {
          hours = 12;                                
          display.fontColor(TS_8b_Green,TS_8b_Black); //am font color and background 
          display.print("Am");                          
        }
  
        if (rtc.getHours() >= 1 && rtc.getHours() < 12) //from 1 am to 11:59:59 am
        {
          hours = rtc.getHours();                   
          display.fontColor(TS_8b_Green,TS_8b_Black); //am font color and background 
          display.print("Am");                          
        }
  
        if (rtc.getHours() == 12)                     //high noon
        {
          hours = rtc.getHours();                   
          display.fontColor(TS_8b_Blue,TS_8b_Black); //pm font color and background 
          display.print("Pm");                          
        }
  
        if (rtc.getHours() >= 13 && rtc.getHours() < 24) //from 1 pm to 11:59:59 pm
        {
          hours = rtc.getHours() - 12;              
          display.fontColor(TS_8b_Blue,TS_8b_Black); //pm font color and background 
          display.print("Pm");                          
        }
 
     }
     else                                            //24hr time
     {
          hours = rtc.getHours();                       //get the rtc hours (in 24hr time)
          display.fontColor(TS_8b_Yellow,TS_8b_Black); //24hr font color and background
          display.print("24");   
     }

      display.setCursor(hours_x,hours_y);            //set cursor for hours
      print2digits(hours); 
      display.print(":");

      display.setCursor(minutes_x,minutes_y);        //set cursor for minutes
      minutes = rtc.getMinutes();
      print2digits(minutes);
      display.print(":");

      display.setCursor(seconds_x,seconds_y);        //set cursor for seconds
      seconds = rtc.getSeconds();
      print2digits(seconds); 


                                                     //don't think next line is needed, but....
      display.fontColor(TS_8b_White,TS_8b_Black);    //this is the default font color and background      
}




 
