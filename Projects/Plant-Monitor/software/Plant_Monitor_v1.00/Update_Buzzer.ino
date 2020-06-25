//  ------------------------------------------------------------
//  Update Buzzer
//  ------------------------------------------------------------

void  update_buzzer()
{ //if we're between 4 pm and 5 pm, sound the buzzer if it's == 1
  if (rtc.getHours() >= buzzer_start_time && rtc.getHours() < buzzer_stop_time)                                                                                
  {
    if (limit_index != 0) //if this is true, then we're trying to fix the limits
    {
      buzzer_flag = 0;    //so turn off the buzzer flag
    }
    
    if (rtc.getSeconds()%10 == 0) //beep every 10 seconds if buzzer_flag == 1
    {
      maybe_sound_beeps();
     }    
  }

}

//  ------------------------------------------------------------
//  maybe_sound_beeps if below minimums and buzzer_flag == 1
//  ------------------------------------------------------------
void  maybe_sound_beeps()
{
    if (soil_1_moisture_percent < soil_1_moisture_limit)
    {
      pixel =  soil_1_start_x;            //starting pixel for soil 1 icon
      line  =  24;                        //starting line for  soil 1 icon
      draw_red_rectangle_around_soil_icon_area();
      
      frequency = soil_1_frequency;
      beep();
      delay (1000);
    }


    if (soil_2_moisture_percent < soil_2_moisture_limit)
    {
      pixel =  soil_2_start_x;            //starting pixel for soil 2 icon
      line  =  24;                        //starting line for  soil 2 icon
      draw_red_rectangle_around_soil_icon_area();
      
      frequency = soil_2_frequency;
      beep();
      beep();
      delay(1000);
    }


    
    get_lux_total();
    
    if (lux_total < lux_buzzer_min)
    {
      draw_red_rectangle_around_lux_plot_area();
      frequency = lux_level_frequency;
      beep();
      beep();
      beep();
      delay(1000);
    }   
}

//  ------------------------------------------------------------
//  get_lux_total value from parsing the lux_array
//  ------------------------------------------------------------ 
void  get_lux_total()
{
  lux_total = 0;
  for (byte i = 0; i<= lux_array_ptr_max; i++)
  {
    lux_total = lux_total + lux_array[i];
  }
}

//  ------------------------------------------------------------
//  sound the beep at the specified frequency
//  if the buzzer_flag == 1, else, don't
//  ------------------------------------------------------------ 
void  beep()
{
  if (buzzer_flag == 1)
  {
    tone (A0,frequency,duration);
    delay(250);    
  }

}
