//  ------------------------------------------------------------
//  update_plot_area 
//  ------------------------------------------------------------

void  update_plot_area()
{                         
      update_soil_1_icon();
      update_soil_2_icon();
      update_lux_plot();
}



//  ------------------------------------------------------------
//  update soil 1 plot
//
//pixel   0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20  
//line 14          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            99%
//line 15          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            97%
//line 16          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            95%
//line 17          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            93%
//line 18          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            91%
//line 19          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            89%
//line 20          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            87%
//line 21          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            85%
//line 22          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            83%
//line 23          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            81%
//line 24          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            79%
//line 25          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            77%
//line 26          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            75%
//line 27          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            73%
//line 28          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            71%
//line 29          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            69%
//line 30          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            67%
//line 31          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            65%
//line 32          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            63%
//line 33          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            61%
//line 34          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            59%
//line 35          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            57%
//line 36          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            55%
//line 37          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            53%
//line 38          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            51%
//line 39          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            49%
//line 40          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            47%
//line 41          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            45%
//line 42          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            43%
//line 43          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            41%
//line 44          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            39%
//line 45          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            37%
//line 46          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            35%
//line 47          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            33%
//line 48          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            31%
//line 49          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            29%
//line 50          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            27%
//line 51          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            25%
//line 52          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            23%
//line 53          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            21%
//line 44          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            19%
//line 55          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            17%
//line 56          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            15%
//line 57             -  -  -  -  -  -  -  -  -  -  -  -  -             13%
//line 58                -  -  -  -  -  -  -  -  -  -  -             11%
//line 59                   -  -  -  -  -  -  -  -  -              9%
//line 60                      -  -  -  -  -  -  -              7%
//line 61                         -  -  -  -  -              5%
//line 62                            -  -  -              3%
//line 63                               -              1%
//        
//  ------------------------------------------------------------
void  update_soil_1_icon()
{   
    display.setCursor(4,0);                              //display soil_1 number above icon
    display.fontColor(TS_8b_White,TS_8b_Black);
    display.print("#1");  

    soil_moisture_percent   =  soil_1_moisture_percent;   //acquired soil moisture percentage
    soil_moisture_limit     =  soil_1_moisture_limit;     //user defined limit     
    pixel                   =  soil_1_start_x;            //starting pixel for soil icon
    line                    =  14;                        //starting line for soil icon
    draw_soil_icon();                                     //draw the soil icon based on the above
    
    display.setCursor(0,11);                             //display soil_1_moisture_percent just below
    
    if (soil_moisture_percent < soil_moisture_limit)
    {
     display.fontColor(TS_8b_Red,TS_8b_Black);     
    }
    else
    {
      display.fontColor(TS_8b_Green,TS_8b_Black);       
    }
    
    print2digits(soil_1_moisture_percent);
    display.print("%");  
    
}


//  ------------------------------------------------------------
//  update soil_2 plot
//pixel  75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95  
//line 14          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            99%
//line 15          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            97%
//line 16          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            95%
//line 17          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            93%
//line 18          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            91%
//line 19          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            89%
//line 20          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            87%
//line 21          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            85%
//line 22          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            83%
//line 23          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            81%
//line 24          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            79%
//line 25          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            77%
//line 26          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            75%
//line 27          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            73%
//line 28          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            71%
//line 29          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            69%
//line 30          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            67%
//line 31          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            65%
//line 32          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            63%
//line 33          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            61%
//line 34          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            59%
//line 35          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            57%
//line 36          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            55%
//line 37          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            53%
//line 38          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            51%
//line 39          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            49%
//line 40          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            47%
//line 41          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            45%
//line 42          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            43%
//line 43          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            41%
//line 44          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            39%
//line 45          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            37%
//line 46          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            35%
//line 47          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            33%
//line 48          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            31%
//line 49          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            29%
//line 50          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            27%
//line 51          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            25%
//line 52          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            23%
//line 53          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            21%
//line 44          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            19%
//line 55          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            17%
//line 56          -  -  -  -  -  -  -  -  -  -  -  -  -  -  -            15%
//line 57             -  -  -  -  -  -  -  -  -  -  -  -  -             13%
//line 58                -  -  -  -  -  -  -  -  -  -  -             11%
//line 59                   -  -  -  -  -  -  -  -  -              9%
//line 60                      -  -  -  -  -  -  -              7%
//line 61                         -  -  -  -  -              5%
//line 62                            -  -  -              3%
//line 63                               -              1%
//        
//  ------------------------------------------------------------
void  update_soil_2_icon()
{
    display.setCursor(79,0);                              //display soil_2 number above icon
    display.fontColor(TS_8b_White,TS_8b_Black);
    display.print("#2");


    soil_moisture_percent   =  soil_2_moisture_percent;   //acquired soil moisture percentage
    soil_moisture_limit     =  soil_2_moisture_limit;     //user defined limit    
    pixel                   =  soil_2_start_x;            //starting pixel for soil icon
    line                    =  14;                        //starting line for soil icon
    draw_soil_icon();                                     //draw the soil icon based on the above
    
     display.setCursor(75,11);                             //display soil_2_moisture_percent just below

    if (soil_moisture_percent < soil_moisture_limit)
    {
    display.fontColor(TS_8b_Red,TS_8b_Black);     
    }
    else
    {
      display.fontColor(TS_8b_Green,TS_8b_Black);       
    }
    
    print2digits(soil_2_moisture_percent); 
    display.print("%");  

  //the following checks to see if we're below or above the soil_moisutre_limit
  //and then activates or deactivates the buzzer_flag  

}


//  ------------------------------------------------------------
//  update lux plot
//  ------------------------------------------------------------
void  update_lux_plot()
{
    if (limit_index != 3)                           //we're NOT updating the lux_top_plot_level
    {
      draw_brown_rectangle_around_lux_plot_area();  //just like it says
    }
    else
    {
      draw_black_rectangle_around_lux_plot_area();  //if here, we ARE updating the lux_top_plot_level
                                                    //so get rid of the rectangle we drew in set_lux_limit()
    }

  
    if (lux_array_ptr != lux_array_ptr_last)        //if the lux array pointer changed
    {                                               //every 15 minutes then
      lux_array_ptr_last = lux_array_ptr;           //re-initialize the following
      lux_ave_divisor    = 1;                      
      lux_ave_value      = 0;
      lux_total          = 0;
    }

    lux_total       = lux_total + lux;              //add lux to lux_total
    lux_ave_value   = lux_total / lux_ave_divisor;  //average the total lux value
    lux_array[lux_array_ptr] = lux_ave_value;       //and store it to the array
    lux_ave_divisor = lux_ave_divisor + 1;          //and increment the divisor for the next time around
    
    //display real-time LUX value above plot area
    display.setCursor(20,12);                             //display real-time LUX value above plot area
    display.fontColor(TS_8b_Brown,TS_8b_Black);           //set the font and background colors
    display.print("LUX ");                                //print LUX
    display.drawRect(42,12,34,10,TSRectangleFilled ,TS_8b_Black); //clear lux number area    
    display.print (lux);                                  //print the latest lux value

  //determine the lux_array_ptr based on time between 6am and 6 pm in 15 minute increments
   if (rtc.getHours() >= 6 && rtc.getHours() < 18)
   {  //just below is using "seconds" to compute lux_array_ptr
      //take off 15 minutes (900 seconds) to allow for lux_array_ptr (0 - 47) to change between 6am to 5:45:00pm in 15 minute intervals 
      lux_array_ptr = map((rtc.getHours()*3600 + rtc.getMinutes()*60 + rtc.getSeconds()),(6*3600),(18*3600 -900),lux_array_ptr_min,lux_array_ptr_max);  

      update_lux_plot_area();     //update this area with the lux plot
   }

  //clear the lux_array and plot area just before normal plotting commences
   if (rtc.getHours() == 5 && rtc.getMinutes() == 59 && rtc.getSeconds() == 59)
   {

      initialize_lux_plot_area(); //initialize the lux plot area
    
   }
}




//  ------------------------------------------------------------
//  clear_lux_plot_area
//  ------------------------------------------------------------

void  clear_lux_plot_area()
{
    //clear the lux plot area with a filled black rectangle  
    display.drawRect(22,23,52,41,TSRectangleFilled ,TS_8b_Black); 
}

//  ------------------------------------------------------------
//  update_lux_plot_area @ lux_array_ptr
//
//  tempByte contains the x position of the lux_array_ptr
//  lux_plot_offset is the horizontal offset for plotting
//  ------------------------------------------------------------

void  update_lux_plot_area()
{   
    for (tempByte = lux_array_ptr_min; tempByte <= lux_array_ptr_max; tempByte++)
    {
      //at the lux_array_ptr + lux_plot_offset where the x part of the plot line should be...
      //first erase the plot line by drawing a black line from top to bottom of the plot area
      display.drawLine(tempByte + lux_plot_offset, lux_y_plot_min, tempByte + lux_plot_offset, lux_y_plot_max,TS_8b_Black);

      lux_ave_value = lux_array[tempByte];    //get the lux_ave_value from the lux_array @ tempByte and
      lux_to_y();                              //convert it to lux_to_y_value for plotting

      //now draw the plot iine depending on morning (green) or afternoon (blue)
      if (tempByte <= (lux_array_ptr_max/2))
      {
            //morning - then draw GREEN line from the bottom of the plot area up to the lux value
            display.drawLine(tempByte + lux_plot_offset, lux_y_plot_min, tempByte + lux_plot_offset,lux_to_y_value,TS_8b_Green);    
      }
      else
      {
           //afternoon - then draw a BLUE line from the bottom of the plot area up to the lux value
            display.drawLine(tempByte + lux_plot_offset, lux_y_plot_min, tempByte + lux_plot_offset,lux_to_y_value,TS_8b_Blue);           
      }
    }

    display_lux_top_plot_level();  //last, display the lux_top_plot_level number in the upper-left corner of the lux plot area
                                   //next, redraw the lux plot
}

//  ------------------------------------------------------------
//  lux_to_y
//  convert the lux_ave_value to a y value for plotting
//  ------------------------------------------------------------
void  lux_to_y()
{
    lux_to_y_value = map(lux_ave_value,0,lux_top_plot_level,lux_y_plot_min,lux_y_plot_max); //get the average y value to plot
    
    if (lux_to_y_value < lux_y_plot_max)            //yes, this is right since the y-axis values are reversed
    {
      lux_to_y_value = lux_y_plot_max;              //in case of over-run
    }
}

//  ------------------------------------------------------------
//  clear_lux_array()
//  ------------------------------------------------------------
void  clear_lux_array()
{
        for (lux_array_ptr = lux_array_ptr_min; lux_array_ptr <= lux_array_ptr_max; lux_array_ptr++) 
        { 
          lux_array[lux_array_ptr] = 0;   
        }
 }

//  ------------------------------------------------------------
//  draw soil icon
//
//  Enter with...
//  pixel = starting x position
//  line  = starting y position
//  soil_moisture_percent = soil_1_moisture_percent or soil_2_moisture_percent
//  soil_moisture_limit   = soil_1_moisture_limit   or soil_2_moisture_limit
//
//  Exit with...
//  appropriate lines drawn according to soil_moisture percent
//  limit line drawn on icon
//  ------------------------------------------------------------
void  draw_soil_icon()
{
//clear the soil_moisture icon area
    clear_soil_icon_area();
      
//get the limit line in terms of the soil icon lines (63,14)
    get_limit_line(); 
    
//draw green or red lines based on soil_moisture && limit_line
//green above limit_line && red below limit_line
//start at the bottom and work up 

//this is the bottomm point of the icon - line 63
    line = 63;  
    if (soil_moisture_percent >= 1)
     {
      display.drawPixel(pixel+7,line,TS_8b_Red); //1%  (Red)
     }
//the points begin to expand into a larger and larger line, etc, etc
    line = 62;    
    if (soil_moisture_percent >= 3)
     {
      display.drawLine(pixel+6,line,pixel+8,line,TS_8b_Red); //3% (Red)
     }            

    line = 61;       
    if (soil_moisture_percent >= 5)
     {
       display.drawLine(pixel+5,line,pixel+9,line,TS_8b_Red); //5% (Red)
     }

    line = 60;          
    if (soil_moisture_percent >= 7)
     {
      display.drawLine(pixel+4,line,pixel+10,line,TS_8b_Red); //7% (Red)
     }

    line = 59;    
    if (soil_moisture_percent >= 9)
     {
      display.drawLine(pixel+3,line,pixel+11,line,TS_8b_Red); //9% (Red)
     }

    line = 58;    
    if (soil_moisture_percent >= 11)
    {
      display.drawLine(pixel+2,line,pixel+12,line,TS_8b_Red); //11% (Red)
    }

    line = 57;          
    if (soil_moisture_percent >= 13)
    {
     display.drawLine(pixel+1,line,pixel+13,line,TS_8b_Red); //13% (Red)
    }  


//now draw the main vertical body of the icon starting from line 56 (bottom) to line 14 (top)
   for (line = 56; line >= 14; line--)
    {
       soil_moisture_percent_moving = map(line,56,14,15,99);  //create the soil_moisture_percent_moving value
                                                              //with line 56 = 15% up to line 14 = 99%  

      if (soil_moisture_percent >= soil_moisture_percent_moving && soil_moisture_percent_moving < soil_moisture_limit)
      {
            display.drawLine(pixel+1,line,pixel+13,line,TS_8b_Red); 
      }
  
      if (soil_moisture_percent >= soil_moisture_percent_moving && soil_moisture_percent_moving >= soil_moisture_limit)
      {
            display.drawLine(pixel+1,line,pixel+13,line,TS_8b_Green); 
      }
   }

//finally, draw the soil moisture limit line inside the body - see misc_routines()
   draw_limit_line();
   
}
