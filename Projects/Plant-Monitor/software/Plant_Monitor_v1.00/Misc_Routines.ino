//  ------------------------------------------------------------
//  Misc Routines
//  ------------------------------------------------------------


void print2digits(byte number) 
{
  // print a 0 before if the number is < than 10
      if (number < 10) 
      {
        display.print("0");                         
      }
      display.print(number);
}  


void  clear_soil_icon_area()
{
  //clear the soil_moisture icon area
    display.clearWindow(pixel,line,19,50);
}

void  draw_red_rectangle_around_soil_icon_area()
{
    display.drawRect(pixel,line,15,31,TSRectangleNoFill ,TS_8b_Red);    
}


void  draw_brown_rectangle_around_lux_plot_area()
{
    display.drawRect(22,23,52,41,TSRectangleNoFill ,TS_8b_Brown); 
}


void  draw_red_rectangle_around_lux_plot_area()
{
    display.drawRect(22,23,52,41,TSRectangleNoFill ,TS_8b_Red); 
}

void  draw_black_rectangle_around_lux_plot_area()
{
    display.drawRect(22,23,52,41,TSRectangleNoFill ,TS_8b_Black); 
}

void  get_limit_line()
{
  //get the limit line in terms of the soil icon lines (63,14)
    limit_line = map(soil_moisture_limit,0,100,63,14); 
}

void  draw_limit_line()
{
  //draw the limit line inside the body
    display.drawLine(pixel+1,limit_line,pixel+13,limit_line,TS_8b_White); 
}

void  display_lux_top_plot_level()
{
     display.fontColor(TS_8b_Brown,TS_8b_Black);           //set the font colors
     display.setCursor(24,24);                             //display luz_top_plot_level inside the plot area
     display.print (lux_top_plot_level);                   //print the latest lux value     
}

void  initialize_lux_plot_area()
{
      buzzer_flag = 1;                        //set the buzzer flag for later testing in update_buzzer()
      clear_lux_array();
      lux_array_ptr_last = 99;                //just to make sure to start the day off right
      lux_array_ptr = lux_array_ptr_min;      //just to make sure to start the day off right
      clear_lux_plot_area();
}
