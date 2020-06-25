//  ------------------------------------------------------------
//  Set Limits
//  set soil moisture and lux limits
//  ------------------------------------------------------------

void  set_limits()
{
  //button_clr_action() changes the limit_index variable as follows
    
    if (button_clr_flag == 1)                   
    {
      button_clr_flag = 0;
      date_time_index = 0; //if we're setting limits then we can't set the time
      limit_counter   = limit_counter_reload;
      limit_index     = limit_index + 1;

      if (limit_index > 3)
      {
        limit_index = 0;
      }
    }

  //test the limit_counter to see if "time's up" or not
  //if so, set limit_index = 0 to do nothing
  //else, go on
  
   if (limit_counter > 0)
   {
      limit_counter = limit_counter - 1;
   }
   else
   {
      limit_index = 0;
   }

//here is where we decide what action to take 

  switch (limit_index)            
    {
         case 0:
          //do nothing                 
          break;  
              
        case 1:
          //set soil 1 moisture limit
          set_soil_1_moisture_limit();
           break;
          
        case 2:
          //set soil 2 moisture limit
          set_soil_2_moisture_limit();
          break;
          
        case 3:
          //set lux limit
          set_lux_limit();
          break;
            
        default:
          // if nothing else matches, do the default
          // default is optional
          limit_index = 0;  //go back to the beginning
        break;
     }

}


//  ------------------------------------------------------------
//  Set soil 1 moisture limit
//  increment or decrement soil_1_moisture_limit as necessary
//  ------------------------------------------------------------
  void  set_soil_1_moisture_limit()
  {
    pixel                   =  soil_1_start_x;            //starting pixel for rectangle
    line                    =  24;                        //starting line for rectangle
    draw_red_rectangle_around_soil_icon_area();           //draw a red rectangle to show that 
                                                          //we may be changing the limit line
                                                          //see misc_routines()    
    if (button_plus_flag == 1)
    {
      button_plus_flag = 0;
      limit_counter    = limit_counter_reload;
          
      if (soil_1_moisture_limit < soil_moisture_limit_max)
      {
        soil_1_moisture_limit = soil_1_moisture_limit + 1;
      }
    }

    if (button_minus_flag == 1)
    {
      button_minus_flag = 0;
      limit_counter     = limit_counter_reload;
          
      if (soil_1_moisture_limit > soil_moisture_limit_min)
      {
        soil_1_moisture_limit = soil_1_moisture_limit - 1;
      }  
    }   
 }

//  ------------------------------------------------------------
//  Set soil 2 moisture limit
//  increment or decrement soil_2_moisture_limit as necessary
//  ------------------------------------------------------------  
  void  set_soil_2_moisture_limit()
  {
    pixel                   =  soil_2_start_x;            //starting pixel for rectangle
    line                    =  24;                        //starting line for  rectangle
    draw_red_rectangle_around_soil_icon_area();           //draw a red rectangle to show that 
                                                          //we may be changing the limit line
                                                          //see misc_routines()
    if (button_plus_flag == 1)
    {
      button_plus_flag = 0;
      limit_counter    = limit_counter_reload;

      if (soil_2_moisture_limit < soil_moisture_limit_max)
      {
        soil_2_moisture_limit = soil_2_moisture_limit + 1;
      }
    }

    if (button_minus_flag == 1)
    {
      button_minus_flag = 0;
      limit_counter     = limit_counter_reload;

      if (soil_2_moisture_limit > soil_moisture_limit_min)
      {
        soil_2_moisture_limit = soil_2_moisture_limit - 1;
      }    
    }

  }
//  ------------------------------------------------------------
//  Set lux limit
//  increment or decrement lux_plot_level as necessary
//  ------------------------------------------------------------
  void  set_lux_limit()
  {
    draw_red_rectangle_around_lux_plot_area();  //see misc_routines()
    
    if (button_plus_flag == 1)
    {
      button_plus_flag = 0;
      limit_counter    = limit_counter_reload;
      clear_lux_plot_area();
      update_lux_plot_area();

      if (lux_top_plot_level < lux_top_plot_level_max)
      {
        lux_top_plot_level  =  lux_top_plot_level + lux_top_plot_level_min;
      }
    }

    if (button_minus_flag == 1)
    {
      button_minus_flag = 0;
      limit_counter    = limit_counter_reload;
      clear_lux_plot_area();
      update_lux_plot_area();
     

      if (lux_top_plot_level > lux_top_plot_level_min + lux_top_plot_level_min - 1)
      {
        lux_top_plot_level  =  lux_top_plot_level - lux_top_plot_level_min;
      }
    } 

}
