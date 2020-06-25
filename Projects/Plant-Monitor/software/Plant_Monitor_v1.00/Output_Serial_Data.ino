//  ------------------------------------------------------------
//  Output Serial Data
//  ------------------------------------------------------------

//  output serial data to USB and BLE
//  the data has been chosen between log and playback in get_measurements()

void  output_serial_data()                    //output to USB - MakerPlot style with "," between values
{
    SerialUSB.print(soil_1_moisture_percent); //real-time soil_1_moisture_percent    
        SerialUSB.print(",");
    SerialUSB.print(soil_2_moisture_percent); //real-time soil_2_moisture_percent  
        SerialUSB.print(",");
    SerialUSB.print(lux);                     //real-time lux light value
        SerialUSB.print(",");

    mp_lux_array_ptr = mp_lux_array_ptr + 1;   //output the lux array one element per pass thru this routine
    if (mp_lux_array_ptr > lux_array_ptr_max)
    {
      mp_lux_array_ptr = lux_array_ptr_min;    //test for over-run    
    }
        
    SerialUSB.println(lux_array[mp_lux_array_ptr]); //note a "println" to generate a CR character for MakerPlot
  
}
